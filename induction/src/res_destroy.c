#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <dlfcn.h>

#include "induction.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"

#define DEFAULT_TIMEOUT_SEC 120

/* cbdata struct manipulation */

i_resource_destroy_cbdata* i_resource_destroy_cbdata_create ()
{
  i_resource_destroy_cbdata *cbdata;

  cbdata = (i_resource_destroy_cbdata *) malloc (sizeof(i_resource_destroy_cbdata));
  if (!cbdata)
  { i_printf (1, "i_resource_destroy_cbdata_create failed to malloc cbdata"); return NULL; }
  memset (cbdata, 0, sizeof(i_resource_destroy_cbdata_create));

  return cbdata;
}

void i_resource_destroy_cbdata_free (void *cbdataptr)
{
  i_resource_destroy_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;

  if (cbdata->msg_callback)
  { i_msgproc_callback_remove_by_reqid (cbdata->socket, cbdata->msgid); }

  free (cbdata);
}

/* Destroy a resource */

i_resource_destroy_cbdata* i_resource_destroy (i_resource *self, i_resource_address *res_addr, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata)
{
  char *res_addr_str;
  i_resource_destroy_cbdata *cbdata;

  if (!self || !res_addr) return NULL;
  
  res_addr_str = i_resource_address_struct_to_string (res_addr);
  if (!res_addr_str)
  { i_printf (1, "i_resource_destroy failed to create res_addr_str from res_addr"); return NULL; }

  cbdata = i_resource_destroy_cbdata_create ();
  if (!cbdata)
  { i_printf (1, "i_resource_destroy failed to create cbdata struct"); return NULL; }
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;
  cbdata->socket = self->core_socket;

  cbdata->msgid = i_message_send (self, MSG_RES_DESTROY, res_addr_str, strlen(res_addr_str)+1, NULL, MSG_FLAG_REQ, 0);
  free (res_addr_str);
  if (cbdata->msgid == -1)
  { i_printf (1, "i_resource_destroy failed to send MSG_RES_DESTROY msg"); i_resource_destroy_cbdata_free (cbdata); return NULL; }

  cbdata->msg_callback = i_msgproc_callback_add (self, cbdata->socket, cbdata->msgid, DEFAULT_TIMEOUT_SEC, 0, i_resource_destroy_msg_callback, cbdata);
  if (!cbdata->msg_callback)
  { i_printf (1, "i_resource_destroy failed to add msg_callback"); i_resource_destroy_cbdata_free (cbdata); return NULL; }

  return cbdata;
}

int i_resource_destroy_msg_callback (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  i_resource_destroy_cbdata *cbdata = passdata;
  
  if (!self || !cbdata) return -1;

  cbdata->msg_callback = NULL;

  if (!msg)
  { i_printf (1, "i_resource_destroy_msg_callback recvd NULL msg (timeout)"); i_resource_destroy_failed (self, cbdata); return -1; }
  if (msg->flags & MSG_FLAG_ERROR)
  { i_printf (1, "i_resource_destroy_msg_callback recvd error msg"); i_resource_destroy_failed (self, cbdata); return -1; }

  if (cbdata->callback_func)
  { cbdata->callback_func (self, 0, passdata); }

  i_resource_destroy_cbdata_free (cbdata);

  return 0;
}

int i_resource_destroy_failed (i_resource *self, i_resource_destroy_cbdata *cbdata)
{
  if (!self || !cbdata) return -1;

  if (cbdata->callback_func)
  { cbdata->callback_func (self, -1, cbdata->passdata); }

  i_resource_destroy_cbdata_free (cbdata);

  return 0;
}

