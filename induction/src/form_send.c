#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

#define DEFAULT_TIMEOUT_SEC 120

/* cbdata struct manipulation */

i_form_send_cbdata* i_form_send_cbdata_create ()
{
  i_form_send_cbdata *cbdata;

  cbdata = (i_form_send_cbdata *) malloc (sizeof(i_form_send_cbdata));
  if (!cbdata)
  { i_printf (1, "i_form_send_cbdata failed to malloc cbdata"); return NULL; }
  memset (cbdata, 0, sizeof(i_form_send_cbdata));

  return cbdata;
}

void i_form_send_cbdata_free (void *cbdataptr)
{
  i_form_send_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;
  if (cbdata->msg_callback)
  { i_msgproc_callback_remove_by_reqid (cbdata->socket, cbdata->msg_callback->reqid); }

  free (cbdata);
}

/* form_send functions */

i_form_send_cbdata* i_form_send (i_resource *self, i_resource_address *addr, i_entity_address *entaddr, char *form_name, time_t ref_sec, i_form *form, int (*callback_func) (i_resource *self, i_form *form, void *passdata), void *passdata)
{
  int form_datasize;
  int datasize;
  long msgid;
  char *data;
  char *dataptr;
  char *form_data;
  char *entaddrstr;
  i_form_send_cbdata *cbdata;

  /*
   * Data format is :
   *
   * int entaddrstr_len;
   * char *entaddrstr;
   * int form_name_length
   * char *form_name
   * long ref_sec
   * int form_datasize
   * char *form_data
   *
   */
  
  if (!self || !addr || !form_name) 
  { return NULL; }

  /* Create/setup data */

  /* Convert form to data */
  form_data = i_form_form_to_data (form, &form_datasize);
  if (!form_data) 
  { i_printf (1, "i_form_send failed to compile form to form_data"); return NULL; }

  /* Convert entaddr to string */
  entaddrstr = i_entity_address_string (NULL, entaddr);

  /* Calculate data size */
  datasize = (3*sizeof(int)) + sizeof(long) + form_datasize + (strlen(form_name)+1);
  if (entaddrstr) datasize += strlen (entaddrstr)+1;

  /* Malloc data */
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_form_send failed to malloc data"); free (form_data); return NULL; }
  dataptr = data;

  /* Compile data */
  
  dataptr = i_data_add_string (data, dataptr, datasize, entaddrstr);
  free (entaddrstr);
  if (!dataptr)
  { i_printf (1, "i_form_send failed to add form_name to data"); free (form_data); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, form_name);
  if (!dataptr)
  { i_printf (1, "i_form_send failed to add form_name to data"); free (form_data); free (data); return NULL; }

  dataptr = i_data_add_long (data, dataptr, datasize, &ref_sec);
  if (!dataptr)
  { i_printf (1, "i_form_send failed to add ref_sec to data"); free (form_data); free (data); return NULL; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, form_data, form_datasize);
  free (form_data);
  if (!dataptr)
  { i_printf (1, "i_form_send failed to add form_data to data"); free (data); return NULL; }

  /* Send message */
  msgid = i_message_send (self, MSG_FORM_SEND, data, datasize, addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid == -1) 
  { i_printf (1, "i_form_send failed to send message"); return NULL; }

  /* Create cbdata struct */
  cbdata = i_form_send_cbdata_create ();
  if (!cbdata)
  { i_printf (1, "i_form_send failed to create cbdata struct"); return NULL; }
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;
  cbdata->socket = self->core_socket;

  /* Add msg callback */
  cbdata->msg_callback = i_msgproc_callback_add (self, cbdata->socket, msgid, DEFAULT_TIMEOUT_SEC, 0, i_form_send_msg_callback, cbdata);
  if (!cbdata->msg_callback)
  { i_printf (1, "i_form_send failed to add message callback"); i_form_send_cbdata_free (cbdata); return NULL; }

  return cbdata;
}

int i_form_send_msg_callback (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  i_form *form;
  i_form_send_cbdata *cbdata = passdata;

  /* State check */
  
  if (!msg || msg->flags & MSG_FLAG_ERROR)
  {
    i_printf (1, "i_form_send_msg_callback received NULL msg or an error message");
    i_form_send_failed (self, cbdata);
    return -1;
  }

  cbdata->msg_callback = NULL;

  /* Convert msg to form */
  
  form = i_form_data_to_form (msg->data, msg->datasize);
  if (!form) 
  { 
    i_printf (1, "i_form_send_msg_callback failed to convert recvd form from data to struct"); 
    i_form_send_failed (self, cbdata);
    return -1;
  }

  /* Run callback */

  if (cbdata->callback_func)
  { cbdata->callback_func (self, form, cbdata->passdata); }

  /* Finished */

  i_form_send_cbdata_free (cbdata);

  return 0;
}

int i_form_send_failed (i_resource *self, i_form_send_cbdata *cbdata)
{
  if (cbdata->callback_func)
  { cbdata->callback_func (self, NULL, cbdata->passdata); }

  i_form_send_cbdata_free (cbdata);

  return 0;
}

/* MSG_FORM_SEND handler */

int i_form_send_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  int num;
  int form_datasize;
  int offset;
  char *entaddrstr;
  char *form_name_raw;
  char *form_data;
  char *dataptr;
  i_form_reqdata *reqdata;

  /* Take the incomming message, extract the form/form_name/module_name, then 
   * pass the form onto the relevant function. The relevant function will
   * return either a return form or NULL on failure. Take the return form and
   * send it back to the client 
   *
   * ALWAYS RETURN 0 to keep the handler active
   */

  /* Find the resource */
  i_resource *resource;
  if (self->hosted)
  {
    /* Find the hosted resource */
    resource = i_resource_hosted_get (self, msg->dst);
    if (!resource)
    { i_printf (1, "i_form_send_handler failed to find hosted device"); return 0; }
  }
  else
  { resource = self; }

  /* Create/Setup the reqdata */
  reqdata = i_form_reqdata_create ();
  if (!reqdata)
  { i_printf (1, "i_form_send_handler failed to create reqdata struct"); i_form_handler_failed (self, msg, NULL); return 0; }
  reqdata->type = FORM_REQ_SEND;
  reqdata->msg_in = i_message_duplicate (msg);
  reqdata->auth = i_authentication_duplicate (msg->auth);
  gettimeofday (&reqdata->req_time, NULL);
  
  /* Interpret msg */
  
  dataptr = msg->data;

  entaddrstr = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_send_handler failed to find entaddrstr in msg->data"); i_form_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;
  if (entaddrstr) 
  { reqdata->entaddr = i_entity_address_struct (entaddrstr); free (entaddrstr); }
   
  form_name_raw = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_send_handler failed to find form_name in msg->data"); i_form_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;
  asprintf (&reqdata->form_name, "%s_submit", form_name_raw);
  free (form_name_raw);

  reqdata->ref_sec = i_data_get_long (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_send_handler failed to find ref_sec in msg->data"); i_form_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;

  form_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &form_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_form_send_handler failed to get form_data from msg->data"); i_form_handler_failed (self, msg, reqdata); return 0; }
  dataptr += offset;

  reqdata->form_in = i_form_data_to_form (form_data, form_datasize);
  free (form_data);
  if (!reqdata->form_in) 
  { i_printf (1, "i_form_send_handler failed to decode data to reqdata->form_in"); i_form_handler_failed (self, msg, reqdata); return 0; }

  /* Find the form function */

  reqdata->form_func = i_form_func_get (self, reqdata->form_name, &num);
  if (num != 0)
  { i_printf (2, "i_form_send_handler unable to find form %s", reqdata->form_name); i_form_handler_failed (self, msg, reqdata); return 0; }

  /* Call the form_func */  

  num = reqdata->form_func (resource, reqdata);
  if (num == -1) 
  { i_printf (1, "i_form_send_handler failed, reqdata->form_func returned -1"); i_form_handler_failed (self, msg, reqdata); return 0; }

  if (num == 1)
  { i_form_deliver (self, reqdata); }   /* Form is ready to be delivered */

  /* At this point, the fate of reqdata is unknown.
   * It could be still around, waiting for the form_func to do 
   * its work and call i_form_deliver, or it may have already
   * been freed by i_form_deliver if it was called prior to
   * form_func exiting. In anycase, we don't touch it here.
   */

  return 0;
}

/* @} */
