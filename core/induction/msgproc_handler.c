#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "induction.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "hashtable.h"

/* Struct Manipulation */

i_msgproc_handler* i_msgproc_handler_create ()
{
  i_msgproc_handler *hdlr;

  hdlr = (i_msgproc_handler *) malloc (sizeof(i_msgproc_handler));
  if (!hdlr)
  { i_printf (1, "i_msgproc_handler_create failed to malloc hdlr struct"); return NULL; }
  memset (hdlr, 0, sizeof(i_msgproc_handler));

  return hdlr;
}

void i_msgproc_handler_free (void *hdlrptr)
{ 
  i_msgproc_handler *hdlr = hdlrptr;

  if (!hdlr) return;

  free (hdlr);
}

/* Handler manipulation */

i_msgproc_handler* i_msgproc_handler_add (i_resource *self, i_socket *sock, int type, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata), void *passdata)
{
  int num;
  i_hashtable_key *key;
  i_msgproc_handler *hdlr;

  if (!self || !sock || !sock->msgproc->handler_table || !callback_func) return NULL;

  hdlr = i_msgproc_handler_create ();
  if (!hdlr)
  { i_printf (1, "i_msgproc_handler_add failed to create hdlr struct"); return NULL; }
  hdlr->type = type;
  hdlr->socket = sock;
  hdlr->callback_func = callback_func;
  hdlr->passdata = passdata;

  key = i_hashtable_create_key_int (type, sock->msgproc->handler_table->size);
  if (!key) 
  { i_printf (1, "i_msgproc_handler_add failed to create key"); i_msgproc_handler_free (hdlr); return NULL; }

  num = i_hashtable_put (sock->msgproc->handler_table, key, hdlr);
  i_hashtable_free_key (key);
  if (num != 0) 
  { i_printf (1, "i_msgproc_handler_add failed to put hdlr in handler_table"); i_msgproc_handler_free (hdlr); return NULL; }

  return hdlr;
}

int i_msgproc_handler_set_default (i_resource *self, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata), void *passdata)
{
  /* The default handler is the handler of last resort */

  i_msgproc_handler *hdlr;

  if (!sock->msgproc)
  { i_printf (1, "i_msgproc_handler_set_default called for a socket that has not been i_msgproc enabled"); return -1; }
  
  hdlr = i_msgproc_handler_create ();
  if (!hdlr)
  { i_printf (1, "i_msgproc_handler_set_default failed to create hdlr struct"); return -1; }
  hdlr->callback_func = callback_func;
  hdlr->passdata = passdata;

  if (sock->msgproc->default_handler) 
  { i_msgproc_handler_free (sock->msgproc->default_handler); sock->msgproc->default_handler = NULL; }
  sock->msgproc->default_handler = hdlr;

  return 0;
}

int i_msgproc_handler_set_fatal (i_resource *self, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata), void *passdata)
{
  /* The fatal handler called when a fatal error occurs in i_msgproc */

  i_msgproc_handler *hdlr;

  if (!sock->msgproc)
  { i_printf (1, "i_msgproc_handler_set_fatal called for a socket that has not been i_msgproc enabled"); return -1; }

  hdlr = i_msgproc_handler_create ();
  if (!hdlr)
  { i_printf (1, "i_msgproc_handler_set_fatal failed to create hdlr struct"); return -1; }
  hdlr->callback_func = callback_func;
  hdlr->passdata = passdata;

  if (sock->msgproc->fatal_handler)
  { i_msgproc_handler_free (sock->msgproc->fatal_handler); sock->msgproc->fatal_handler = NULL; }
  sock->msgproc->fatal_handler = hdlr;

  return 0;

}

int i_msgproc_handler_remove_by_type (i_resource *self, i_socket *sock, int type)
{
  int num;
  i_hashtable_key *key;

  key = i_hashtable_create_key_int (type, sock->msgproc->handler_table->size);
  if (!key)
  { i_printf (1, "i_msgproc_handler_remove_by_type failed to create hashtable key"); return -1; }

  num = i_hashtable_remove (sock->msgproc->handler_table, key);
  i_hashtable_free_key (key);

  return num;
}

int i_msgproc_handler_remove (i_resource *self, i_msgproc_handler *hdlr)
{ return i_msgproc_handler_remove_by_type (self, hdlr->socket, hdlr->type); }
