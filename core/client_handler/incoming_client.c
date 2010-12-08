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
#include <induction/form.h>
#include <induction/construct.h>

#include "main.h"
#include "msgproc_child.h"

int l_incoming_client (i_resource *self, i_socket *sock, void *passdata)
{
  /* 
   * Called when there is a connection to accept on one of the
   * client listener sockets (unix/tcp)
   */
  
  i_socket *client_socket = NULL;
  i_resource *client_res;
  char *res_id_str = NULL;
  i_hashtable *client_table = passdata;

  i_printf (2, "l_incoming_client has an incoming client connection");

  /* FIX: A failure to create the socket here could create an
   * extremely-fast-loop if the condition is repeatable (i.e on each i_loop
   * iteration.
   */
  
  switch (sock->type)
  {
    case SOCKET_UNIX: client_socket = i_socket_accept_unix (sock->sockfd);      /* Accept the socket */
                      if (!client_socket) { i_printf (1, "l_client_incoming unable to accept unix socket"); return 0; }
                      break;
    case SOCKET_TCP: client_socket = i_socket_accept_tcp (sock->sockfd);        /* Accept the socket */
                     if (!client_socket) { i_printf (1, "l_client_incoming unable to accept unix socket"); return 0; }
                     break;
    default: i_printf (1, "l_incoming_client failed, unknown client type"); 
             return 0;
  }

  /* Create ident_str */
  asprintf (&res_id_str, "%i", client_socket->sockfd);

  /* Spawn the resource to handle this connection */
  client_res = i_resource_local_create (self, RES_ADDR(self), client_table, RES_CLIENT_HANDLER_CHILD, -1, res_id_str, "client_handler.so", self->root, NULL, NULL);
  i_socket_free (client_socket);
  if (res_id_str) free (res_id_str);
  if (!client_res)
  {
    i_printf (1, "l_client_incoming unable to create resource to handle connection");    
    return 0;
  }

  i_printf (2, "l_client_incoming_unix attached new client at %s:%s:%i:%i:%s (pid %i)", 
    client_res->plexus, client_res->node, client_res->type, client_res->ident_int, client_res->ident_str, client_res->construct->pid);

  return 0;
}

