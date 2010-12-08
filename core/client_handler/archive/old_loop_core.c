#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <induction.h>

#include "loop_core.h"
#include "incomming.h"
#include "process.h"
#include "client_handler.h"

i_list *global_sockfd_set_list = NULL;

int l_client_handler_loop_core (i_resource *self, i_hashtable *client_table)
{
  
  char *unix_sock_path;
  i_socket *unix_listener;
  i_socket *tcp_listener;
  i_socket *client_core_listener;

  /* Create the unis domain socket for client_handler_child connections */

  unix_sock_path = i_path_socket ("lithium", "client_handler_core");
  if (!unix_sock_path) 
  { 
    i_printf (1, "i_client_handler_loop unable to make unix_socket_path"); 
    return -1; 
  }

  client_core_listener = i_socket_create_unix (unix_sock_path, SOCKET_LISTEN);
  if (!client_core_listener) 
  { 
    i_printf (1, "i_client_handler_loop unable to create client_core unix socket"); 
    free (unix_sock_path); 
    return -1; 
  }
  free (unix_sock_path);
  
  /* Create the unix domain socket for client connections */
  
  unix_sock_path = i_path_socket("lithium", "client_handler");
  if (!unix_sock_path) 
  { 
    i_printf (1, "i_client_handler_loop unable to make unix_socket_path"); 
    i_socket_free (client_core_listener);
    return -1; 
  }
	
  unix_listener = i_socket_create_unix (unix_sock_path, SOCKET_LISTEN);
  if (!unix_listener) 
  { 
    i_printf (1, "i_client_handler_loop unable to create client unix socket"); 
    free (unix_sock_path); 
    i_socket_free (client_core_listener);
    return -1; 
  }
  chmod (unix_sock_path, 0777);
  free (unix_sock_path);

  /* Create the TCP socket for incomming client connections */

  tcp_listener = i_socket_create_tcp (NULL, CLIENT_TCP_PORT); /* FIX: Should be configurable */
  if (!tcp_listener) 
  { 
    i_printf (1, "i_client_handler_loop unable to create client tcp socket");
    i_socket_free (unix_listener); 
    i_socket_free (client_core_listener);
    return -1;
  }

  while (1)
  {
    int num;
    int highestsockfd = 0;
    struct timeval timeout;
    fd_set sockfdset;

    timeout.tv_sec = CLIENT_LOOP_TIMEOUT_SEC;
    timeout.tv_usec = CLIENT_LOOP_TIMEOUT_USEC;

    FD_ZERO (&sockfdset);

    /* Add all the sockets we need to listen to */
    
    highestsockfd = i_resource_sockfdset_from_table (client_table, &sockfdset);       /* Add all the clients in the client_table */
 
    FD_SET (unix_listener->sockfd, &sockfdset);                                            /* Add the listener */
    if (unix_listener->sockfd > highestsockfd) highestsockfd = unix_listener->sockfd;      /* Check highest sockfd */

    FD_SET (tcp_listener->sockfd, &sockfdset);
    if (tcp_listener->sockfd > highestsockfd) highestsockfd = tcp_listener->sockfd;

    FD_SET (client_core_listener->sockfd, &sockfdset);            /* Add the client_handler_core listener */
    if (client_core_listener->sockfd > highestsockfd) highestsockfd = client_core_listener->sockfd; /* Check the highest sockfd */

    FD_SET (self->core_socket->sockfd, &sockfdset);                                 /* Add the core socket */
    if (self->core_socket->sockfd > highestsockfd) highestsockfd = self->core_socket->sockfd;   /* Check highest sockfd */

    /* Listen to the sockets */
    
    num = select (highestsockfd+1, &sockfdset, NULL, NULL, &timeout);

    /* Action any sockets that trigger the select */
    
    if (num)
    {
      i_resource *client;

      /* Messages from the core first */
      
      if (FD_ISSET(self->core_socket->sockfd, &sockfdset))
        l_client_process_core (self, client_table);     /* Process a message from the core */

      /* Then process messages from clients */      

      global_sockfd_set_list = i_resource_sockfdset_setlist (client_table, &sockfdset); /* Form a list of all the set sockets */
      if (global_sockfd_set_list)
      {

        for (i_list_move_head(global_sockfd_set_list); (client=i_list_restore(global_sockfd_set_list)) != NULL; i_list_move_next(global_sockfd_set_list))
        {
          l_client_process_resource (self, client_table, client);   /* Process the waiting data */
        }

        i_list_free (global_sockfd_set_list);
        global_sockfd_set_list = NULL;
      }

      /* Then check for any incomming client resources */

      if (FD_ISSET (client_core_listener->sockfd, &sockfdset))
       i_resource_incomming (client_table, client_core_listener->sockfd); 
      
      /* Then check for any incomming clients */
      
      if (FD_ISSET (unix_listener->sockfd, &sockfdset))
        l_client_incomming (self, client_table, unix_listener->sockfd, CLIENT_UNIX);  /* New incomming client connection */

      if (FD_ISSET (tcp_listener->sockfd, &sockfdset))
        l_client_incomming (self, client_table, tcp_listener->sockfd, CLIENT_TCP);


    }

    waitpid (0, NULL, WNOHANG);                                                                         /* Clean up any defunct processes */
    i_printf (2, "i_client_handler_loop iteration complete");
  }

  return 0;
}
