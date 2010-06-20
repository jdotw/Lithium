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
#include "construct.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "form.h"
#include "timer.h"
#include "data.h"
#include "cement.h"
#include "entity.h"

#define DEFAULT_TIMEOUT_SEC 120

/* cbdata struct manipulation */

i_resource_spawn_cbdata* i_resource_spawn_cbdata_create ()
{
  i_resource_spawn_cbdata *cbdata;

  cbdata = (i_resource_spawn_cbdata *) malloc (sizeof(i_resource_spawn_cbdata));
  if (!cbdata)
  { i_printf (1, "i_resource_spawn_cbdata_create failed to malloc cbdata struct"); return NULL; }
  memset (cbdata, 0, sizeof(i_resource_spawn_cbdata));

  return cbdata;
}

void i_resource_spawn_cbdata_free (void *cbdataptr)
{
  i_resource_spawn_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;
  if (cbdata->msg_callback)
  { i_msgproc_callback_remove_by_reqid (cbdata->socket, cbdata->msgid); }

  free (cbdata);
}

/* Spawn */

i_resource_spawn_cbdata* i_resource_spawn (i_resource *self, int type, int ident_int, char *ident_str, char *module_name, char *root, char *customer_id, i_entity *ent, int (*callback_func) (i_resource *self, i_resource_address *addr, void *passdata), void *passdata)
{
  /* Sends a message to the core to create a resource
   *
   * The message format is as follows 
   *
   * int type
   * int ident_int
   * int ident_str_len
   * char *ident_str
   * int module_name_len
   * char *module_name
   * int root_len
   * char *root
   * int customer_id_len
   * char *customer_id
   * int entdata_size
   * char *entdata;
   */

  int datasize;
  char *data;
  char *dataptr;
  char *entdata = NULL;
  int entdata_size = 0;
  i_resource_spawn_cbdata *cbdata;

  /* Convert entity to data */
  if (ent)
  { 
    entdata = i_entity_data (ent, &entdata_size);
    if (!entdata)
    { i_printf (1, "i_resource_spawn failed to convert entity to data"); return NULL; }
  }
  
  /* Calculate datasize */
  datasize = 7*sizeof(int);
  if (ident_str) datasize += strlen(ident_str)+1;
  if (module_name) datasize += strlen(module_name)+1;
  if (root) datasize += strlen(root)+1;
  if (customer_id) datasize += strlen (customer_id)+1;
  if (entdata) datasize += entdata_size;

  /* Malloc data */
  data = (char *) malloc (datasize);
  if (!data)
  {
    i_printf (1, "i_resource_spawn unable to malloc data (%i bytes)", datasize);
    if (entdata) free (entdata);
    return NULL;
  }
  memset (data, 0, datasize);
  dataptr = data;

  /* Type */
  dataptr = i_data_add_int (data, dataptr, datasize, &type);
  if (!dataptr) { i_printf (1, "i_resource_spawn failed to put type in data"); if (entdata) free (entdata); free (data); return NULL; }

  /* Ident int */
  dataptr = i_data_add_int (data, dataptr, datasize, &ident_int);
  if (!dataptr) { i_printf (1, "i_resource_spawn failed to put ident_int in data"); if (entdata) free (entdata); free (data); return NULL; }

  /* Ident str */
  dataptr = i_data_add_string (data, dataptr, datasize, ident_str);
  if (!dataptr) { i_printf (1, "i_resource_spawn failed to put ident_str in data"); if (entdata) free (entdata); free (data); return NULL; }

  /* Module name */
  dataptr = i_data_add_string (data, dataptr, datasize, module_name);
  if (!dataptr) { i_printf (1, "i_resource_spawn failed to put module_name in data"); if (entdata) free (entdata); free (data); return NULL; }

  /* Root */
  dataptr = i_data_add_string (data, dataptr, datasize, root);
  if (!dataptr) { i_printf (1, "i_resource_spawn failed to put root in data"); if (entdata) free (entdata); free (data); return NULL; }

  /* Customer ID */
  dataptr = i_data_add_string (data, dataptr, datasize, customer_id);
  if (!dataptr) { i_printf (1, "i_resource_spawn failed to put customer_id in data"); if (entdata) free (entdata); free (data); return NULL; }

  /* Entity data */
  dataptr = i_data_add_chunk (data, dataptr, datasize, entdata, entdata_size);
  if (entdata) free (entdata);
  if (!dataptr) { i_printf (1, "i_resource_spawn failed to put entdata in data"); free (data); return NULL; }

  /* Create cbdata */
  cbdata = i_resource_spawn_cbdata_create ();
  if (!cbdata)
  { i_printf (1, "i_resource_spawn failed to create cbdata callback"); free (data); return NULL; }
  cbdata->callback_func = callback_func;
  cbdata->passdata = passdata;
  cbdata->socket = self->core_socket;
  cbdata->self = self;

  /* Send message */
  cbdata->msgid = i_message_send (self, MSG_RES_SPAWN, data, datasize, NULL, MSG_FLAG_REQ, 0);
  free (data);
  if (cbdata->msgid == -1)
  { i_printf (1, "i_resource_spawn failed to send message to core"); i_resource_spawn_cbdata_free (cbdata); return NULL; }

  /* Add callbak */
  cbdata->msg_callback = i_msgproc_callback_add (self, cbdata->socket, cbdata->msgid, DEFAULT_TIMEOUT_SEC, 0, i_resource_spawn_msg_callback, cbdata);
  if (!cbdata->msg_callback)
  { i_printf (1, "i_resource_spawn failed to add msg_callback"); i_resource_spawn_cbdata_free (cbdata); return NULL; }

  return cbdata;
}

int i_resource_spawn_msg_callback (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  i_resource_address *addr;
  i_resource_spawn_cbdata *cbdata = passdata;

  if (!self || !cbdata) return -1;

  cbdata->msg_callback = NULL;
  
  if (!msg)
  { i_printf (1, "i_resource_spawn_msg_callback called with NULL msg"); i_resource_spawn_failed (self, cbdata); return -1; }

  if (msg->flags & MSG_FLAG_ERROR || !msg->data || msg->datasize < 1) 
  { i_printf (1, "i_resource_spawn_msg_callback recvd error msg"); i_resource_spawn_failed (self, cbdata); return -1; }

  addr = i_resource_address_string_to_struct ((char *)msg->data);
  if (!addr)
  { i_printf (1, "i_resource_spawn_msg_callback failed to convert msg->data to addr"); i_resource_spawn_failed (self, cbdata); return -1; }

  if (cbdata->cancelled == 0)
  {
    /* Normal completion */
    if (cbdata->callback_func)
    { cbdata->callback_func (self, addr, cbdata->passdata); }
  }
  else
  {
    /* Cancelation requested */
    i_resource_destroy (self, addr, NULL, NULL);
  }

  i_resource_address_free (addr);
  i_resource_spawn_cbdata_free (cbdata);

  return 0;
}

int i_resource_spawn_failed (i_resource *self, i_resource_spawn_cbdata *cbdata)
{
  /* Generic failure routine */
  
  if (!self || !cbdata) return -1;

  if (cbdata->cancelled == 0)
  {
    if (cbdata->callback_func)
    { cbdata->callback_func (self, NULL, cbdata->passdata); }
  }

  i_resource_spawn_cbdata_free (cbdata);

  return 0;
}

/* Cancel */

int i_resource_spawn_cancel (i_resource_spawn_cbdata *cbdata)
{
  if (!cbdata) return -1;
  
  cbdata->cancelled = 1;

  return 0;
}

