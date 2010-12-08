#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/socket.h>
#include <induction/hashtable.h>
#include <induction/message.h>
#include <induction/log.h>
#include <induction/path.h>
#include <induction/msgproc.h>

#include "main.h"
#include "lithium.h"
#include "msgproc_core.h"
#include "incoming_child.h"
#include "incoming_client.h"
#include "child.h"

i_hashtable *global_client_table = NULL;
i_socket *global_client_socket = NULL;

void module_info ()
{
	i_printf (0, "Lithium - Client Handler Module");
}

int module_init (i_resource *self)
{
  int num;
  char *path_file;
  char *unix_sock_path;
  i_socket *tcp_listener;
  i_socket *client_core_listener;        
  i_hashtable *client_table = NULL;
  i_socket_callback *cb;

  if (self->type == RES_CLIENT_HANDLER_CORE)
  {
    /* Enable logging */

    num = i_log_enable (self, NULL);
    if (num != 0)
    { i_printf (1, "module_init warning, failed to enable i_log"); }

    /* Module is the client handler core */

    path_file = "core";

    /* Create the client table */
    
    client_table = i_hashtable_create (CLIENT_TABLE_SIZE);        /* FIX: Must be configurable */
    if (!client_table) { i_printf (1, "module_init(client_handler) unable to create client hash table. failed"); return -1; }
    i_hashtable_set_destructor (client_table, i_resource_free);
    global_client_table = client_table;

    /* Create the unix domain socket for client_handler_child connections */

    unix_sock_path = i_path_socket ("lithium", "client_handler_core");
    if (!unix_sock_path)
    {
      i_printf (1, "module_init(client_handler) unable to make unix_socket_path");
      return -1;
    }
    client_core_listener = i_socket_create_unix (unix_sock_path, SOCKET_LISTEN);
    if (!client_core_listener)
    {
      i_printf (1, "module_init(client_handler) unable to create client_core unix socket");
      free (unix_sock_path);
      return -1;
    }
    free (unix_sock_path);

    cb = i_socket_callback_add (self, SOCKET_CALLBACK_READ_PREEMPT, client_core_listener, l_incoming_child, client_table);
    if (!cb)
    {
      i_printf (1, "module_init(client_handler) unable to add callback for client_core_listener");
      i_socket_free (client_core_listener);
      return -1;
    }

    /* Create the TCP socket for incoming client connections */

    tcp_listener = i_socket_create_tcp (self, NULL, CLIENT_TCP_PORT, NULL, NULL);  /* FIX: Port should be configurable */
    if (!tcp_listener)
    {
      i_printf (1, "module_init(client_handler) unable to create client tcp socket");
      i_socket_free (client_core_listener);
      return -1;
    }                              

    cb = i_socket_callback_add (self, SOCKET_CALLBACK_READ_PREEMPT, tcp_listener, l_incoming_client, client_table);
    if (!cb)
    {
      i_printf (1, "module_init(client_handler) unable to add callback for unix_listener");
      i_socket_free (client_core_listener);
      i_socket_free (tcp_listener);
      return -1;
    }             

    i_msgproc_callback_set_default (self, self->core_socket, l_msgproc_core_default_from_core, client_table);    /* Default callback for routing msgs to child res */
    i_msgproc_handler_set_default (self, self->core_socket, l_msgproc_core_default_from_core, client_table);      /* Default handler for routing msgs to child res */

  }
  else
  {
    /* Module is a client handler child process */
    path_file = "client_handler_core";

    /* Set reqtime */
//    l_reqtime_start ();

    /* Register with core */
    num = i_resource_register (self, path_file, l_child_register_callback, NULL);
    if (num != 0) { i_printf (1, "module_init failed to register child process with client_handler core. terminating"); return -1; }   
  }

  i_printf (2, "module_init(client_handler) complete");

  return 0;
}

int module_entry (i_resource *self)
{
  i_printf (2, "module_entry(client_handler) entered");

  return 0;
}

int module_shutdown (i_resource *self)
{
  i_printf (2, "module_shutdown(client_handler) complete");

  if (global_client_socket && global_client_socket->sockfd > 0) 
  { close (global_client_socket->sockfd); }

  return 0;
}
