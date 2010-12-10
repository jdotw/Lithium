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
#include <libxml/parser.h>

#include "induction.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "path.h"
#include "form.h"
#include "construct.h"
#include "xml.h"

/* cbdata struct manipulation */

i_resource_register_cbdata* i_resource_register_cbdata_create ()
{
  i_resource_register_cbdata *cbdata;

  cbdata = (i_resource_register_cbdata *) malloc (sizeof(i_resource_register_cbdata));
  if (!cbdata)
  { i_printf (1, "i_resource_register_cbdata_create failed to malloc cbdata struct"); return NULL; }
  memset (cbdata, 0, sizeof(i_resource_register_cbdata));

  return cbdata;
}

void i_resource_register_cbdata_free (void *cbdataptr)
{
  i_resource_register_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;
  if (cbdata->write_op)
  { i_socket_write_cancel (cbdata->write_op); }
  if (cbdata->read_op)
  { i_socket_read_cancel (cbdata->read_op); }
  if (cbdata->socket)
  { i_socket_free (socket); }

  free (cbdata);
}

/* Registration process */

int i_resource_register (i_resource *self, char *core_socket_file, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata)
{
  /* Register with the core that has just spawned the resource.
   *
   * First, the resource will open a socket to the core. 
   * The the resource will send its 'long' resource id.
   * The resource will then block and wait for a response from the core.
   * The response should be the resource id sent back to it.
   *
   */

  char *socket_path;
  i_resource_register_cbdata *cbdata;

  /* Create the cbdata struct */

  cbdata = i_resource_register_cbdata_create ();
  if (!cbdata)
  { i_printf (1, "i_resource_register failed to create cbdata struct"); return -1; }
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;

  /* Connect to the core */

  socket_path = i_path_socket ("lithium", core_socket_file);
  cbdata->socket = i_socket_create_unix (socket_path, SOCKET_CONNECT);
  free (socket_path);
  if (!cbdata->socket)
  { i_printf (1, "i_resource_register failed to connect to core"); i_resource_register_cbdata_free (cbdata); return -1; }

  /* Send ID to the core */

  cbdata->write_op = i_socket_write (self, cbdata->socket, &self->construct->id, sizeof(long), i_resource_register_write_callback, cbdata);
  if (!cbdata->write_op)
  { i_printf (1, "i_resource_register failed to send id to core"); i_resource_register_cbdata_free (cbdata); return -1; }

  return 0;
}

int i_resource_register_write_callback (i_resource *self, i_socket *sock, int result, void *passdata)
{
  i_resource_register_cbdata *cbdata = passdata;

  cbdata->write_op = NULL;
  
  if (result != SOCKET_RESULT_SUCCESS)
  {
    i_printf (1, "i_resource_register_write_callback recvd result != SOCKET_RESULT_SUCCESS");
    i_resource_register_failed (self, cbdata);
    return -1;
  }

  cbdata->read_op = i_socket_read (self, cbdata->socket, sizeof(long), i_resource_register_read_callback, cbdata);
  if (!cbdata->read_op)
  { 
    i_printf (1, "i_resource_register_write_callback failed to request read of ID from core"); 
    i_resource_register_failed (self, cbdata);
    return -1;
  }

  return 0;
}

int i_resource_register_read_callback (i_resource *self, i_socket *sock, i_socket_data *data, int result, void *passdata)
{
  int num;
  long recvd_long;
  i_resource_register_cbdata *cbdata = passdata;

  cbdata->read_op = NULL;

  if (!data)
  {
    i_printf (1, "i_resource_register_read_callback recvd NULL data");
    i_resource_register_failed (self, cbdata);
    return -1;
  }

  if (data->datasize != sizeof(long))
  {
    i_printf (1, "i_resource_register_read_callback recvd datasize != sizeof(long)");
    i_resource_register_failed (self, cbdata);
    return -1;
  }

  memcpy (&recvd_long, data->data, sizeof(long));

  if (recvd_long != self->construct->id)
  { 
    i_printf (1, "i_resource_register_read_callback recvd ID != self->construct->id");
    i_resource_register_failed (self, cbdata);
    return -1;
  }

  /* Enable and config i_msgproc */
  
  num = i_msgproc_enable (self, cbdata->socket);
  if (num != 0)
  {
    i_printf (1, "i_resource_register_read_callback failed to enable i_msgproc on socket");
    i_resource_register_failed (self, cbdata);
    return -1;
  }

  if (self->type != RES_CLIENT_HANDLER_CHILD && self->type != RES_CLIENT_HANDLER_CORE)
  {    
    i_msgproc_handler_add (self, cbdata->socket, MSG_FORM_GET, i_form_get_handler, NULL);
    i_msgproc_handler_add (self, cbdata->socket, MSG_FORM_SEND, i_form_send_handler, NULL);
    i_msgproc_handler_add (self, cbdata->socket, MSG_XML_GET, i_xml_get_handler, NULL);
  }
  
  /* Call callback */

  if (self->type != RES_CLIENT_HANDLER_CHILD)
  { i_printf (2, "i_resource_register_read_callback successfully registered with core"); }
    
  self->core_socket = cbdata->socket;
  cbdata->socket = NULL;
  
  if (cbdata->callback_func)
  { cbdata->callback_func (self, 0, cbdata->passdata); }

  i_resource_register_cbdata_free (cbdata);

  return 0;
}

/* Generic Failure Function */

int i_resource_register_failed (i_resource *self, i_resource_register_cbdata *cbdata)
{
  if (!self || !cbdata) return -1;

  if (cbdata->callback_func)
  { cbdata->callback_func (self, -1, cbdata->passdata); }

  i_resource_register_cbdata_free (cbdata);

  return 0;
}

