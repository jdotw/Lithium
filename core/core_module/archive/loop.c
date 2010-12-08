#include <stdio.h>
#include <stdlib.h>
#include <induction.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "loop.h"
#include "resource.h"
#include "status.h"

i_list *global_sockfdset_set_list;                                                  /* FIX : Ugly solution to ensure mem clean up on fork */

void l_core_loop (i_resource *self, i_hashtable *res_table)							/* The main loop of the core */
{
  l_status_set (self, STATUS_NORMAL);     /* Initial Status */
  
  while (1)
  {
    int num;
    int highest_sockfd = 0;
    fd_set sockfdset;
    struct timeval timeout;
    i_list *queued_list;

    FD_ZERO (&sockfdset);

    highest_sockfd = i_resource_sockfdset_from_table (res_table, &sockfdset);
    if (self->core_socket->sockfd > highest_sockfd) highest_sockfd = self->core_socket->sockfd;
    FD_SET (self->core_socket->sockfd, &sockfdset);

    timeout.tv_sec = LOOP_SELECT_TIMEOUT_SEC;
    timeout.tv_usec = LOOP_SELECT_TIMEOUT_USEC;

    num = select (highest_sockfd+1, &sockfdset, NULL, NULL, &timeout);		/* Select the sockfds */

    if (num)									/* Select was triggered, not timedout */
    {
      if (FD_ISSET(self->core_socket->sockfd, &sockfdset)) 
      { i_resource_incomming (res_table, self->core_socket->sockfd); }
			
      global_sockfdset_set_list = i_resource_sockfdset_setlist (res_table, &sockfdset);
      if (global_sockfdset_set_list)	
      {
        i_resource *res;

        for (i_list_move_head(global_sockfdset_set_list); (res = i_list_restore(global_sockfdset_set_list)) != NULL; i_list_move_next(global_sockfdset_set_list))
        {
          l_resource_msg (self, res_table, res);
        }

        i_list_free (global_sockfdset_set_list);
        global_sockfdset_set_list = NULL;
      }
    }

    i_resource_sleep_check_all (self, res_table);					/* See if any resources need waking up */
    waitpid (0, NULL, WNOHANG);                                     /* Clean up any defunct processes */

    i_printf (2, "i_core_loop iteration complete");

  }
}
