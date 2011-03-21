#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/hashtable.h>
#include <induction/respond.h>
#include <induction/auth.h>
#include <induction/form.h>

#include "main.h"
#include "child.h"
#include "msgproc_child.h"
#include "reqtime.h"

int l_child_register_callback (i_resource *self, int result, void *passdata)
{
  /* Called once the child process has registered with the parent */
  int num;
  i_socket *sock;

  /* Update reqtime */
//  l_reqtime_registered ();
  
  /* Create socket */
  sock = i_socket_create ();
  if (!sock)
  {
    i_printf (1, "l_child_register_callback failed to create blank socket");
    module_shutdown (self);
    exit (1);
    return -1;
  }
  sock->sockfd = atoi(self->ident_str);

  /* Enable i_msgproc and add default callback/handler for messages 
   * from the client 
   */
  
  num = i_msgproc_enable (self, sock);
  if (num != 0)
  {
    i_printf (1, "l_child_register_callback failed to enable i_msgproc on client socket");
    i_socket_free (sock);
    module_shutdown (self);
    exit (1);
    return -1;
  }
  i_msgproc_callback_set_default (self, sock, l_msgproc_child_default_from_client, sock);    /* Default callback for routing msgs to client res */
  i_msgproc_handler_set_default (self, sock, l_msgproc_child_default_from_client, sock);     /* Default handler for routing msgs to client res */
  i_msgproc_handler_set_fatal (self, sock, l_msgproc_child_fatal_from_client, sock);         /* Handler for fatal errors in msgs from client res */

  /* Add default callback/handler for messages from the handler_core */
  
  i_msgproc_callback_set_default (self, self->core_socket, l_msgproc_child_default_from_core, sock);    /* Default callback for routing msgs to client res */
  i_msgproc_handler_set_default (self, self->core_socket, l_msgproc_child_default_from_core, sock);     /* Default handler for routing msgs to client res */
  i_msgproc_handler_set_fatal (self, self->core_socket, l_msgproc_child_fatal_from_core, sock);         /* Handler for fatal errors in msgs from client res */

  return 0;
}

