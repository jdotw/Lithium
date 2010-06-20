#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "induction.h"
#include "message.h"
#include "socket.h"
#include "msgproc.h"
#include "hashtable.h"
#include "construct.h"
#include "module.h"

#define HANDLER_TABLE_SIZE 5
#define CALLBACK_TABLE_SIZE 5

/* Struct Manipulation */

i_msgproc* i_msgproc_create ()
{
  i_msgproc *msgproc;

  msgproc = (i_msgproc *) malloc (sizeof(i_msgproc));
  if (!msgproc)
  { i_printf (1, "i_msgproc_create failed to malloc msgproc struct"); return NULL; }
  memset (msgproc, 0, sizeof(i_msgproc));

  msgproc->handler_table = i_hashtable_create (HANDLER_TABLE_SIZE);
  if (!msgproc->handler_table)
  { i_printf (1, "i_msgproc_create failed to create msgproc->handler_table"); i_msgproc_free (msgproc); return NULL; }
  i_hashtable_set_destructor (msgproc->handler_table, i_msgproc_handler_free);
  
  msgproc->callback_table = i_hashtable_create (CALLBACK_TABLE_SIZE);
  if (!msgproc->callback_table)
  { i_printf (1, "i_msgproc_create failed to create msgproc->callback_table"); i_msgproc_free (msgproc); return NULL; }
  i_hashtable_set_destructor (msgproc->callback_table, i_msgproc_callback_free);

  return msgproc;
}

void i_msgproc_free (void *msgprocptr)
{
  i_msgproc *msgproc = msgprocptr;

  if (!msgproc) return;
  if (msgproc->handler_table)
  { i_hashtable_free (msgproc->handler_table); }
  if (msgproc->callback_table)
  { i_hashtable_free (msgproc->callback_table); }
  if (msgproc->default_callback)
  { i_msgproc_callback_free (msgproc->default_callback); }
  if (msgproc->default_handler)
  { i_msgproc_handler_free (msgproc->default_handler); }
  if (msgproc->fatal_handler)
  { i_msgproc_handler_free (msgproc->fatal_handler); }
  if (msgproc->msg_read_cbdata)
  { i_message_read_cancel (msgproc->socket); }
  if (msgproc->read_preempt_callback)
  { i_socket_callback_remove (msgproc->read_preempt_callback); }

  free (msgproc);
}

/* Enable/Disable */

int i_msgproc_enable (i_resource *self, i_socket *sock)
{
  /* Enable message processing on a particular socket */

  if (sock->msgproc) 
  { i_printf (1, "i_msgproc_enable warning, msgproc already enabled on socket"); return 0; }

  sock->msgproc = i_msgproc_create ();
  if (!sock->msgproc)
  { i_printf (1, "i_msgproc_enable failed to create msgproc struct"); return -1; }
  sock->msgproc->socket = sock;

  sock->msgproc->read_preempt_callback = i_socket_callback_add (self, SOCKET_CALLBACK_READ_PREEMPT, sock, i_msgproc_read_preempt_callback, NULL);
  if (!sock->msgproc->read_preempt_callback) 
  { i_printf (1, "l_incoming_resource_incoming_callback failed to add read_preempt socket callback"); return 0; }

  return 0;
}

int i_msgproc_disable (i_resource *self, i_socket *sock)
{
  /* Disable message processing on a particular socket */
  
  if (!sock->msgproc) 
  { i_printf (1, "i_msgproc_enable warning, no msgproc present on socket"); return 0; }

  i_msgproc_free (sock->msgproc);
  sock->msgproc = NULL;

  return 0;
}

/* Fatal Error Handling */

int i_msgproc_fatal (i_resource *self, i_socket *sock)
{
  int (*handler_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata) = NULL;
  void *handler_passdata = NULL;
  
  /* Check for a fatal_handler */
  
  if (sock->msgproc->fatal_handler && sock->msgproc->fatal_handler->callback_func) 
  { 
    /* Fatal error occurred, and a fatal_handler is present and valid */
    handler_func = sock->msgproc->fatal_handler->callback_func;
    handler_passdata = sock->msgproc->fatal_handler->passdata;
  }

  /* Disable i_msgproc */

  i_msgproc_disable (self, sock);

  /* Call the appropriate handler if applicable */
  
  if (handler_func)
  {
    handler_func (self, sock, NULL, handler_passdata);
  }
  else
  {
    if (sock == self->core_socket)
    {
      /* The self->core_socket has experienced a fatal error.
       * Shutdown the module and exit
       */
      i_printf (1, "i_msgproc_fatal a fatal error occurred in i_msgproc for self->core_socket. Shutting down and terminating resource");
      if (self && self->construct && self->construct->module && self->construct->module->shutdown) 
      { self->construct->module->shutdown (self); }
      exit (1);
    }
  }

  return 0;
}

  
