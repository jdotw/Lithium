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
#include "list.h"
#include "socket.h"
#include "timer.h"
#include "hashtable.h"
#include "construct.h"

#define DEFAULT_TIMEOUT_SEC 120

static i_list *static_incoming_list = NULL;

/* cbdata struct manipulation */

i_resource_incoming_cbdata* i_resource_incoming_cbdata_create ()
{
  i_resource_incoming_cbdata *cbdata;

  cbdata = (i_resource_incoming_cbdata *) malloc (sizeof(i_resource_incoming_cbdata));
  if (!cbdata)
  { i_printf (1, "i_resource_incoming_cbdata_create failed to malloc cbdata struct"); return NULL; }
  memset (cbdata, 0, sizeof(i_resource_incoming_cbdata));

  return cbdata;
}

void i_resource_incoming_cbdata_free (void *cbdataptr)
{
  i_resource_incoming_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;

  if (cbdata->key)
  { i_hashtable_free_key (cbdata->key); }
  if (cbdata->socket)
  { i_socket_free (cbdata->socket); }
  if (cbdata->write_op)
  { i_socket_write_cancel (cbdata->write_op); }
  if (cbdata->read_op)
  { i_socket_read_cancel (cbdata->read_op); }
  if (cbdata->timeout_timer)
  { i_timer_remove (cbdata->timeout_timer); }

  free (cbdata);
}

/* List manipulation functions */

int i_resource_local_incoming_list_add (i_resource *self, i_resource *res)
{
  int num;

  if (!static_incoming_list)
  { 
    static_incoming_list = i_list_create ();
    if (!static_incoming_list)
    { i_printf (1, "i_resource_local_incoming_list_add failed to create list"); return -1; }
  }

  num = i_list_enqueue (static_incoming_list, res);
  if (num != 0)
  { 
    i_printf (1, "i_resource_local_incoming_list_add failed to enqueue res");
    if (static_incoming_list->size < 1) 
    { i_list_free (static_incoming_list); static_incoming_list = NULL; }
    return -1;
  }

  return 0;
}

int i_resource_local_incoming_list_remove (i_resource *res)
{
  int num;

  if (static_incoming_list)
  {
    num = i_list_search (static_incoming_list, res);
    if (num == 0)
    { 
      i_list_delete (static_incoming_list); 
      if (static_incoming_list->size < 1)
      { i_list_free (static_incoming_list); static_incoming_list = NULL; }
    }
    else
    { return -1; }
  }

  return 0;
}

/* Incoming resource attachment functions */

i_resource_incoming_cbdata* i_resource_local_incoming (i_resource *self, i_hashtable *res_table, int listener, int (*callback_func) (i_resource *self, i_resource *res, void *passdata), void *passdata)
{
  /* Accept the connection and attempt to read
   * the id (long) from the resource
   */
  
  i_resource_incoming_cbdata *cbdata;
  
  cbdata = i_resource_incoming_cbdata_create ();
  if (!cbdata)
  { i_printf (1, "i_resource_local_incoming failed to create cbdata struct"); return NULL; }
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;
  cbdata->res_table = res_table;
  
  cbdata->socket = i_socket_accept_unix (listener);
  if (!cbdata->socket) 
  { i_printf (1, "i_resource_local_incoming unable to accept connection"); i_resource_incoming_cbdata_free (cbdata); return NULL; }
  
  cbdata->read_op = i_socket_read (self, cbdata->socket, sizeof(long), i_resource_local_incoming_read_callback, cbdata);
  if (!cbdata->read_op)
  { i_printf (1, "i_resource_local_incoming failed to request read of ID"); i_resource_incoming_cbdata_free (cbdata); return NULL; }
  
  cbdata->timeout_timer = i_timer_add (self, DEFAULT_TIMEOUT_SEC, 0, i_resource_local_incoming_timeout_callback, cbdata);
  if (!cbdata->timeout_timer)
  { i_printf (1, "i_resource_local_incoming failed to add timeout timer"); i_resource_incoming_cbdata_free (cbdata); return NULL; }

  return cbdata;
}

int i_resource_local_incoming_read_callback (i_resource *self, i_socket *sock, i_socket_data *data, int result, void *passdata)
{
  /* Called when the id long has been read */

  i_resource *res;
  long recvd_long;
  i_resource_incoming_cbdata *cbdata = passdata;

  cbdata->read_op = NULL;

  if (result != SOCKET_RESULT_SUCCESS || !data || data->datasize != sizeof(long))
  {
    i_printf (1, "i_resource_local_incoming_read_callback failed to read ID from resource");
    i_resource_local_incoming_failed (self, cbdata);
    return -1;
  }

  memcpy (&recvd_long, data->data, sizeof(long));

  for (i_list_move_head(static_incoming_list); (res=i_list_restore(static_incoming_list))!=NULL; i_list_move_next(static_incoming_list))
  { if (res->construct && res->construct->id == recvd_long) cbdata->res = res; }
  if (!cbdata->res)
  {
    i_printf (1, "i_resource_local_incoming_read_callback failed to find matching resource");
    i_resource_local_incoming_failed (self, cbdata);
    return -1;
  }

  if (cbdata->res->state != RES_STATE_WAITING_REGISTER)
  { 
    i_printf (1, "i_resource_local_incoming_read_callback recvd ID for a resource which was not in RES_STATE_WAITING_REGISTER state");
    i_resource_local_incoming_failed (self, cbdata);  
    return -1;
  } 

  cbdata->write_op = i_socket_write (self, cbdata->socket, &cbdata->res->construct->id, sizeof(long), i_resource_local_incoming_write_callback, cbdata);
  if (!cbdata->write_op)
  {
    i_printf (1, "i_resource_local_incoming_read_callback failed to write ID to resource");
    i_resource_local_incoming_failed (self, cbdata);              
    return -1;
  }

  return 0;
}

int i_resource_local_incoming_write_callback (i_resource *self, i_socket *sock, int result, void *passdata)
{
  /* Called when the ID was successfully written to
   * the resource. 
   */

  i_resource_incoming_cbdata *cbdata = passdata;

  cbdata->write_op = NULL;
  
  cbdata->res->core_socket = cbdata->socket;
  cbdata->socket = NULL;
  cbdata->res->state = RES_STATE_RUN;
  i_resource_local_incoming_list_remove (cbdata->res);
  gettimeofday (&cbdata->res->life_sign, NULL);

  if (self->type != RES_CLIENT_HANDLER_CORE)
  {
    i_printf (2, "i_resource_local_incoming_write_callback successfully attached resource %i:%i:%s", 
      cbdata->res->type, cbdata->res->ident_int, cbdata->res->ident_str);
  }

  if (cbdata->callback_func)
  { cbdata->callback_func (self, cbdata->res, cbdata->passdata); }

  i_resource_incoming_cbdata_free (cbdata);

  return 0;
}

int i_resource_local_incoming_timeout_callback (i_resource *self, i_timer *timer, void *passdata)
{
  i_resource_incoming_cbdata *cbdata = passdata;

  cbdata->timeout_timer = NULL;

  if (cbdata->res)
  { i_printf (0, "i_resource_local_incoming_timeout_callback timeout occurred waiting for resource %i:%i:%s to attach",
    cbdata->res->type, cbdata->res->ident_int, cbdata->res->ident_str); }
  else
  { i_printf (1, "i_resource_local_incoming_timeout_callback timeout occurred waiting for unknown resource to attach"); }

  i_resource_local_incoming_failed (self, cbdata);

  return 0;
}

/* Generic Failure */

int i_resource_local_incoming_failed (i_resource *self, i_resource_incoming_cbdata *cbdata)
{
  if (!self || !cbdata) return -1;

  if (cbdata->callback_func)
  { cbdata->callback_func (self, cbdata->res, cbdata->passdata); }

  i_resource_incoming_cbdata_free (cbdata);

  return -1;
}
