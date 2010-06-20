#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/hashtable.h>
#include <induction/respond.h>
#include <induction/auth.h>

#include "main.h"
#include "incoming_child.h"
#include "msgproc_core.h"

/* Incoming child process */

int l_incoming_child (i_resource *self, i_socket *sock, void *passdata)
{
  /* Called when there is an incoming connection from a child process 
   *
   * ALWAYS RETURN 0 to keep the callback alive
   */

  i_resource_incoming_cbdata *incoming_op; 
  i_hashtable *client_table = passdata;

  incoming_op = i_resource_local_incoming (self, client_table, sock->sockfd, l_incoming_child_attach_callback, client_table);
  if (!incoming_op)
  { i_printf (1, "l_incoming_child failed to attach incoming resource"); return 0; }

  return 0;
}

int l_incoming_child_attach_callback (i_resource *self, i_resource *res, void *passdata)
{
  int num;
  i_hashtable *client_table = passdata;
  
  if (!res)
  { i_printf (1, "l_incoming_child_attach_callback failed to attach incoming resource"); return -1; }

  num = i_msgproc_enable (self, res->core_socket);
  if (num != 0)
  { 
    i_printf (1, "l_incoming_child_attach_callback failed to enable i_msgproc on res->core_socket"); 
    i_resource_local_destroy (self, client_table, RES_ADDR(res));
    return -1;
  }  
  i_msgproc_callback_set_default (self, res->core_socket, l_msgproc_core_default_from_child, res);     /* Default callback for routing msgs from child res */
  i_msgproc_handler_set_default (self, res->core_socket, l_msgproc_core_default_from_child, res);      /* Default handler for routing msgs from child res */
  i_msgproc_handler_set_fatal (self, res->core_socket, l_msgproc_core_fatal_from_child, res);          /* Handler for fatal errors in msgs from child res */

  return 0;
}

