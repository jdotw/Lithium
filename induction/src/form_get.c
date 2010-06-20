#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include "induction.h"
#include "entity.h"
#include "form.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"

/** \addtogroup form Web Forms
 * @{
 */

#define FORM_GET_TIMEOUT_SEC 60

/* Callback data struct manipulation */

i_form_get_cbdata* i_form_get_cbdata_create ()
{
  i_form_get_cbdata *cbdata;

  cbdata = (i_form_get_cbdata *) malloc (sizeof(i_form_get_cbdata));
  if (!cbdata)
  { i_printf (1, "i_form_get_cbdata_create failed to malloc cbdata struct"); return NULL; }
  memset (cbdata, 0, sizeof(i_form_get_cbdata));

  return cbdata;
}

void i_form_get_cbdata_free (void *cbdataptr)
{
  i_form_get_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;
  if (cbdata->resaddr) i_resource_address_free (cbdata->resaddr);
  if (cbdata->entaddr) i_entity_address_free (cbdata->entaddr);
  if (cbdata->form_name) free (cbdata->form_name);
  if (cbdata->msg_callback)
  { i_msgproc_callback_remove_by_reqid (cbdata->socket, cbdata->msg_callback->reqid); }
  free (cbdata);
}

/* Form get function and callback */

i_form_get_cbdata* i_form_get (i_resource *self, i_resource_address *resaddr, i_entity_address *entaddr, char *form_name, time_t ref_sec, void *form_passdata, int form_passdatasize, int (*callback_func) (i_resource *self, i_form *form, i_resource_address *resaddr, i_entity_address *entaddr, char *form_name, int result, void *passdata), void *passdata)
{
  /* Data format is as follows :
   *
   * int entaddr_strlen
   * char *entaddr_str
   * int form_name_size
   * char *form_name
   * long ref_sec
   * int form_passdatasize
   * void *form_passdata
   *
   */

  long msgid;
  int datasize;
  char *data;
  char *dataptr;
  char *entaddrstr;
  i_form_get_cbdata *cbdata;
  
  if (!self || !resaddr || !form_name || !callback_func) return NULL;

  /* Create cbdata */
  cbdata = i_form_get_cbdata_create ();
  if (!cbdata)
  { i_printf (1, "i_form_get failed to create cbdata"); return NULL; }
  cbdata->resaddr = i_resource_address_duplicate (resaddr);
  if (entaddr)
  { cbdata->entaddr = i_entity_address_duplicate (entaddr); }
  cbdata->form_name = strdup (form_name);
  cbdata->ref_sec = ref_sec;
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;
  cbdata->socket = self->core_socket;

  /* Convert entaddr to string */
  entaddrstr = i_entity_address_string (NULL, entaddr);

  /* Calculate Datasize */
  datasize = (3*(sizeof(int))) + (strlen(form_name)+1) + sizeof(long) + form_passdatasize;
  if (entaddrstr) datasize += strlen (entaddrstr)+1;
  
  /* Malloc data */
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_form_get failed to malloc data"); i_form_get_cbdata_free (cbdata); return NULL; }
  memset (data, 0, datasize);
  dataptr = data;

  /* Compile data */

  dataptr = i_data_add_string (data, dataptr, datasize, entaddrstr);
  free (entaddrstr);
  if (!dataptr)
  { i_printf (1, "i_form_get failed to add entaddrstr string to data"); i_form_get_cbdata_free (cbdata); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, form_name);
  if (!dataptr)
  { i_printf (1, "i_form_get failed to add form_name string to data"); i_form_get_cbdata_free (cbdata); free (data); return NULL; }

  dataptr = i_data_add_long (data, dataptr, datasize, (long *) &ref_sec);
  if (!dataptr)
  { i_printf (1, "i_form_get failed to add ref_sec long to data"); i_form_get_cbdata_free (cbdata); free (data); return NULL; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, form_passdata, form_passdatasize);
  if (!dataptr)
  { i_printf (1, "i_form_get failed to add form_passdata to data"); i_form_get_cbdata_free (cbdata); free (data); return NULL; }
  
  msgid = i_message_send (self, MSG_FORM_GET, data, datasize, resaddr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid == -1)
  { 
    i_printf (1, "i_form_get failed to send message to addr (%s:%s:%i:%i:%s)", resaddr->plexus, resaddr->node, resaddr->type, resaddr->ident_int, resaddr->ident_str);
    i_form_get_cbdata_free (cbdata);
    return NULL;
  }

  cbdata->msg_callback = i_msgproc_callback_add (self, cbdata->socket, msgid, FORM_GET_TIMEOUT_SEC, 0, i_form_get_callback, cbdata);
  if (!cbdata->msg_callback)
  { i_printf (1, "i_form_get failed to install message callback"); i_form_get_cbdata_free (cbdata); return NULL; }

  return cbdata;
}

int i_form_get_callback (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Non-persistent callback. Always return -1 */

  i_form *form;
  i_form_get_cbdata *cbdata = passdata;

  cbdata->msg_callback = NULL;

  if (!msg || !msg->data || msg->datasize < 1)
  {
    if (msg && msg->flags & MSG_FLAG_DENIED)
    {
      /* Auth failed */
      i_printf (1, "i_form_get_v2_callback authentication failed");
      cbdata->callback_func (self, NULL, cbdata->resaddr, cbdata->entaddr, cbdata->form_name, FORM_RESULT_DENIED, cbdata->passdata);
    }
    else
    {
      /* Generic error */
      i_printf (1, "i_form_get_v2_callback timed out or other error while waiting for response");
      cbdata->callback_func (self, NULL, cbdata->resaddr, cbdata->entaddr, cbdata->form_name, FORM_RESULT_FAILED, cbdata->passdata);
    }
    i_form_get_cbdata_free (cbdata);
    return -1;
  }

  form = i_form_data_to_form (msg->data, msg->datasize);
  if (!form)
  {
    i_printf (1, "i_form_get_v2_callback failed to convert msg->data to form");
    cbdata->callback_func (self, NULL, cbdata->resaddr, cbdata->entaddr, cbdata->form_name, FORM_RESULT_FAILED, cbdata->passdata);
    i_form_get_cbdata_free (cbdata);
    return -1;
  }

  if (cbdata->callback_func)
  { cbdata->callback_func (self, form, cbdata->resaddr, cbdata->entaddr, cbdata->form_name, FORM_RESULT_OK, cbdata->passdata); }
  i_form_free (form);
  i_form_get_cbdata_free (cbdata);

  return -1;
}

/* MSG_FORM_GET Handler */

int i_form_get_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{ 
  /* Always return 0 to keep the handler active */

  int num;
  int offset;
  char *entaddrstr;
  char *dataptr;
  i_form_reqdata *reqdata;

  if (!self || !msg)
  { return 0; }
  
  if (msg->datasize < (2 + sizeof(int)))  /* Minimum size is 2 \0's and some data */
  { i_printf (1, "i_form_get_handler failed, msg->datasize (%i) < minimum (%i)", msg->datasize, (2 + sizeof(int))); i_form_handler_failed (self, msg, NULL); return 0; }

  /* Find the resource */
  i_resource *resource;
  if (self->hosted)
  {
    /* Find the hosted resource */
    resource = i_resource_hosted_get (self, msg->dst);
    if (!resource)
    { i_printf (1, "i_form_get_handler failed to find hosted device"); return 0; }
  }
  else
  { resource = self; }

  /* Create/Setup the reqdata */

  reqdata = i_form_reqdata_create ();
  if (!reqdata)
  { i_printf (1, "i_form_get_handler failed to create reqdata struct"); i_form_handler_failed (self, msg, NULL); return 0; }
  reqdata->type = FORM_REQ_GET;
  reqdata->msg_in = i_message_duplicate (msg);
  reqdata->auth = i_authentication_duplicate (msg->auth);
  gettimeofday (&reqdata->req_time, NULL);
  
  /* Interpret the msg->data */
  
  dataptr = msg->data;

  entaddrstr = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_get_handler failed to get entaddrstr from msg->data"); i_form_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;
  if (entaddrstr)
  { reqdata->entaddr = i_entity_address_struct (entaddrstr); free (entaddrstr); }

  reqdata->form_name = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_get_handler failed to get form_name from msg->data"); i_form_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;

  reqdata->ref_sec = i_data_get_long (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_get_handler failed to get ref_sec from msg->data"); i_form_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;

  reqdata->form_passdata = i_data_get_chunk (msg->data, dataptr, msg->datasize, &reqdata->form_passdata_size, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_get_handler failed to get form_passdata from msg->data"); i_form_handler_failed (self, msg, NULL); return 0; }
  dataptr += offset;

  /* Find the form_func */
  reqdata->form_func = i_form_func_get (resource, reqdata->form_name, &num);
  if (num != 0)
  { i_printf (2, "i_form_get_handler unable to find form %s", reqdata->form_name); i_form_handler_failed (self, msg, reqdata); return 0; }

  /* Call the form_func */

  num = reqdata->form_func (self, reqdata);       /* Call the found form func */
  if (num == -1) 
  { i_printf (1, "i_form_get_handler failed, form_func returned -1"); i_form_handler_failed (self, msg, reqdata); return 0; }

  if (num == 1)
  {
    /* If 1 is returned, the reqdata->form_out is ready to be delivered */
    i_form_deliver (self, reqdata);
  }

  /* At this point, the fate of reqdata is unknown.
   * It could be still around, waiting for the form_func to do 
   * its work and call i_form_deliver, or it may have already
   * been freed by i_form_deliver if it was called prior to
   * form_func exiting. In anycase, we don't touch it here.
   */

  return 0;
}

/* @} */
