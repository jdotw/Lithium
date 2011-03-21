#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <induction.h>

#include "loop_child.h"
#include "incoming.h"
#include "process.h"

#define CLIENT_LOOP_TIMEOUT_SEC 5
#define CLIENT_LOOP_TIMEOUT_USEC 0

int l_client_handler_loop_child (i_resource *self, i_form *config_form)
{
  int sockfd;
  char *data;

  data = i_form_generic_get (config_form, "socket");
  if (!data) { i_printf (1, "l_client_handler_loop_child unable to get 'socket' from config_form. failed."); return -1; }
  memcpy (&sockfd, data, sizeof(int));

  /* 
   * This loop is for a child process of the client_handler_core. 
   * It is responsible for handling a single clients interaction 
   */
  while (1)
  {
    fd_set sockfdset;
    int highestsockfd = 0;
    int num;
    struct timeval timeout;

    timeout.tv_sec = CLIENT_LOOP_TIMEOUT_SEC;
    timeout.tv_usec = CLIENT_LOOP_TIMEOUT_USEC;

    FD_ZERO (&sockfdset);

    FD_SET (sockfd, &sockfdset);
    if (sockfd > highestsockfd) highestsockfd = sockfd;

    FD_SET (self->core_socket->sockfd, &sockfdset);
    if (self->core_socket->sockfd > highestsockfd) highestsockfd = self->core_socket->sockfd;

    num = select (highestsockfd+1, &sockfdset, NULL, NULL, &timeout);

    if (num)
    {
      if (FD_ISSET(self->core_socket->sockfd, &sockfdset))
        l_client_process_client_core (self, sockfd);                        /* Process a message from client_handler_core */

      if (FD_ISSET(sockfd, &sockfdset))
        l_client_process_client (self, sockfd);                             /* Process a message from our attached client */
    }

    i_printf (2, "l_client_handler_loop_child iteration complete");
    
  }
}
