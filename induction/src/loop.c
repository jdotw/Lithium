#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "induction.h"
#include "socket.h"
#include "timer.h"
#include "build_config.h"
#include "loop.h"

/*
 * Functions relating to the generic i_loop loop.
 *
 * This loop should be used as the main loop structure for just about any 
 * lithium module. Timer and socket interaction is the main purpose of the
 * loop function. 
 */

#define DEFAULT_SELECT_TIMEOUT_SEC 60
#define DEFAULT_SELECT_TIMEOUT_USEC 0

/* Loop initialisation */

int i_loop_init ()
{
  /* Reset all settings and clear the loop */
  i_loop_fdset_init ();

  return 0;
}

/* Loop */

int i_loop (i_resource *self)
{
  int num;
  int select_num;
  struct timeval loop_start;

  /* Initialisation */

  if (!self) 
  { i_printf (1, "i_loop call with NULL self, exiting."); return -1; }

  /* Loop */
  
  while (1)
  {
    int highest_sockfd = 0;
    int use_timeout = 0;
    fd_set read_fdset;
    fd_set write_fdset;
    fd_set except_fdset;
    struct timeval timeout;

    FD_ZERO (&read_fdset);
    FD_ZERO (&write_fdset);
    FD_ZERO (&except_fdset);
    gettimeofday (&loop_start, NULL);
    
    /* Process timers */
    i_printf (2, "i_loop processing timers");
    num = i_timer_process (self);
    if (num != 0)
    { i_printf (1, "i_loop encountered a fatal error processing timers. exiting"); return -1; }
    i_printf (2, "i_loop finished processing timers");
    
    /* Run the fd_set preprocessors */
    num = i_loop_fdset_preprocessor (self, &read_fdset, &write_fdset, &except_fdset);
    if (num > highest_sockfd) highest_sockfd = num;

    /* Calculate select timeout */
    use_timeout = i_timer_select_timeout (self, &timeout);
    
    /* Call select */
    i_printf (2, "i_loop calling select");
    if (use_timeout == 1)
    { select_num = select (highest_sockfd+1, &read_fdset, &write_fdset, &except_fdset, &timeout); }
    else
    { select_num = select (highest_sockfd+1, &read_fdset, &write_fdset, &except_fdset, NULL); }
    i_printf (2, "i_loop finished calling select");

    /* Run the fd_set postprocessor if present */
    i_loop_fdset_postprocessor (self, select_num, &read_fdset, &write_fdset, &except_fdset);

#ifdef OS_DARWIN
    /* Call waitpid */
    waitpid (-1, NULL, WNOHANG);
#endif

    i_printf (2, "i_loop iteration complete");
  }
}
