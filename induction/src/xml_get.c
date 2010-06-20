#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include "induction.h"
#include "entity.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"
#include "xml.h"

/** \addtogroup xml XML Functions
 * @{
 */

#define XML_GET_TIMEOUT_SEC 300

/* XML request send function and callback */

i_xml_request* i_xml_get (i_resource *self, i_resource_address *resaddr, i_entity_address *entaddr, char *xml_name, time_t ref_sec, i_xml *xml_out, int (*cbfunc) (i_resource *self, i_xml *xml, int result, void *passdata), void *passdata)
{
  /* Data xmlat is as follows :
   *
   * int entaddr_strlen
   * char *entaddr_str
   * int xml_name_strlen
   * char *xml_name
   * long ref_sec
   * int xmlout_datasize;
   * char *xmlout_data;
   *
   */

  long msgid;
  int datasize;
  int xmlout_datasize = 0;
  char *data;
  char *dataptr;
  char *xmlout_data = NULL;
  char *entaddrstr;
  i_xml_request *reqdata;
  
  /* Create request data struct */
  reqdata = i_xml_request_create ();
  reqdata->resaddr = i_resource_address_duplicate (resaddr);
  reqdata->entaddr = i_entity_address_duplicate (entaddr);
  reqdata->xml_name = strdup (xml_name);
  reqdata->ref_sec = ref_sec;
  reqdata->cbfunc = cbfunc;
  reqdata->passdata = passdata;
  reqdata->socket = self->core_socket;

  /* Convert entaddr to string */
  entaddrstr = i_entity_address_string (NULL, entaddr);

  /* Convert XML to data */
  if (xml_out)
  {
    xmlout_data = i_xml_data (xml_out, &xmlout_datasize); 
    if (!xmlout_data)
    { i_printf (1, "i_xml_get warning, failed to convert xml_out to data"); }
  }

  /* Calculate Datasize */
  datasize = (3*(sizeof(int))) + (strlen (entaddrstr)+1) + (strlen(xml_name)+1) + sizeof(long) + xmlout_datasize;
  
  /* Malloc data */
  data = (char *) malloc (datasize);
  memset (data, 0, datasize);
  dataptr = data;

  /* Compile data */

  dataptr = i_data_add_string (data, dataptr, datasize, entaddrstr);
  free (entaddrstr);
  if (!dataptr)
  { i_printf (1, "i_xml_get failed to add entaddrstr string to data"); i_xml_request_free (reqdata); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, xml_name);
  if (!dataptr)
  { i_printf (1, "i_xml_get failed to add xml_name string to data"); i_xml_request_free (reqdata); free (data); return NULL; }

  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &ref_sec);
  if (!dataptr)
  { i_printf (1, "i_xml_get failed to add ref_sec long to data"); i_xml_request_free (reqdata); free (data); return NULL; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, xmlout_data, xmlout_datasize);
  free (xmlout_data);
  if (!dataptr)
  { i_printf (1, "i_xml_get failed to add xmlout_data to data"); i_xml_request_free (reqdata); free (data); return NULL; }

  /* Send message */
  msgid = i_message_send (self, MSG_XML_GET, data, datasize, reqdata->resaddr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid == -1)
  { 
    i_printf (1, "i_xml_get failed to send message to addr");
    i_xml_request_free (reqdata);
    return NULL;
  }

  reqdata->msg_callback = i_msgproc_callback_add (self, reqdata->socket, msgid, XML_GET_TIMEOUT_SEC, 0, i_xml_get_msgcb, reqdata);
  if (!reqdata->msg_callback)
  { i_printf (1, "i_xml_get failed to install message callback"); i_xml_request_free (reqdata); return NULL; }

  return reqdata;
}

int i_xml_get_msgcb (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called when a response to an MSG_XML_GET is received
   *
   * Non-persistent callback. Always return -1 
   */

  i_xml *xml;
  i_xml_request *reqdata = passdata;

  reqdata->msg_callback = NULL;

  /* Check message */
  if (!msg || !msg->data || msg->datasize < 1)
  {
    if (msg && msg->flags & MSG_FLAG_DENIED)
    {
      /* Auth failed */
      reqdata->cbfunc (self, NULL, XML_RESULT_DENIED, reqdata->passdata);
    }
    else
    {
      /* Generic error */
      reqdata->cbfunc (self, NULL, XML_RESULT_ERROR, reqdata->passdata);
    }
    i_xml_request_free (reqdata);

    return -1;
  }

  /* Performance Logging */
  struct timeval start_tv;
  gettimeofday (&start_tv, NULL);
  char *addr_str = NULL;
  if (reqdata->entaddr) addr_str = i_entity_address_string (NULL, reqdata->entaddr);
  if (self->perflog) i_printf (0, "PERF: i_xml_get_msgcb received %u bytes of %s XML for %s", msg->datasize, reqdata->xml_name, addr_str);

  /* Convert data to xml struct */
  xml = i_xml_struct (msg->data, msg->datasize, reqdata->xml_name);
  xml->req = reqdata;

  /* Performance Logging */
  struct timeval end_tv;
  gettimeofday (&end_tv, NULL);
  if (self->perflog) i_printf (0, "PERF: i_xml_get_msgcb took %li seconds to parse %u bytes of %s XML for %s", end_tv.tv_sec - start_tv.tv_sec, msg->datasize, reqdata->xml_name, addr_str);
  if (addr_str) free (addr_str);
  addr_str = NULL;

  /* Run callback */
  if (reqdata->cbfunc)
  { reqdata->cbfunc (self, xml, XML_RESULT_OK, reqdata->passdata); }

  /* Cleanup */
  i_xml_free (xml);
  i_xml_request_free (reqdata);

  return -1;
}

/* MSG_XML_GET Handler */

int i_xml_get_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{ 
  /* Called to process an incoming MSG_XML_GET message
   *
   * Always return 0 to keep the handler active 
   */

  int num;
  int offset;
  int xmlin_datasize;
  char *entaddrstr;
  char *dataptr;
  char *xmlin_data;
  i_xml_request *reqdata;

  if (msg->datasize < (2 + sizeof(int) + sizeof(long)))  /* Minimum size is 2 bytes, two ints and one long */
  { i_printf (1, "i_xml_get_handler failed, msg too short"); i_xml_handler_failed (self, msg, NULL); return 0; }

  /* Find the resource */
  i_resource *resource;
  if (self->hosted)
  {
    /* Find the hosted resource */
    resource = i_resource_hosted_get (self, msg->dst);
    if (!resource)
    { i_printf (1, "i_xml_get_handler failed to find hosted device"); return 0; }
  }
  else
  { resource = self; }

  /* Create/Setup the reqdata */
  reqdata = i_xml_request_create ();
  reqdata->type = XML_REQ_GET;
  reqdata->msg_in = i_message_duplicate (msg);
  reqdata->auth = i_authentication_duplicate (msg->auth);
  gettimeofday (&reqdata->req_tv, NULL);
  
  /* Interpret the msg->data */
  
  dataptr = msg->data;

  entaddrstr = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_xml_get_handler failed to get entaddrstr from msg->data"); i_xml_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;
  if (entaddrstr)
  { reqdata->entaddr = i_entity_address_struct (entaddrstr); free (entaddrstr); }

  reqdata->xml_name = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_xml_get_handler failed to get xml_name from msg->data"); i_xml_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;

  reqdata->ref_sec = i_data_get_long (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_xml_get_handler failed to get ref_sec from msg->data"); i_xml_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;

  xmlin_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &xmlin_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_xml_get_handler failed to get xmlin_data from msg->data"); i_xml_handler_failed (self, msg, reqdata); return 0; }
  if (xmlin_data)
  {
    reqdata->xml_in = i_xml_struct (xmlin_data, xmlin_datasize, NULL);
    if (!reqdata->xml_in)
    { i_printf (1, "i_xml_get_handler warning, failed to convert xmlin_data to xmlin"); }
    free (xmlin_data);
  }

  /* Find the xml_func */
  reqdata->xml_func = i_xml_func_get (self, reqdata->xml_name, &num);
  if (num != 0)
  { i_printf (1, "i_xml_get_handler unable to find xml %s", reqdata->xml_name); i_xml_handler_failed (self, msg, reqdata); return 0; }

  /* Call the xml_func */
  num = reqdata->xml_func (resource, reqdata);       /* Call the found xml func */
  if (num == -1) 
  { i_printf (2, "i_xml_get_handler failed, xml_func returned -1"); i_xml_handler_failed (self, msg, reqdata); return 0; }

  /* Check function return */
  if (num == 1)
  {
    /* If 1 is returned, the reqdata->xml_out is ready to be delivered */
    i_xml_deliver (self, reqdata);
  }

  /* At this point, the fate of reqdata is unknown.
   * It could be still around, waiting for the xml_func to do 
   * its work and call i_xml_deliver, or it may have already
   * been freed by i_xml_deliver if it was called prior to
   * xml_func exiting. In anycase, we don't touch it here.
   */

  return 0;
}


/* @} */
