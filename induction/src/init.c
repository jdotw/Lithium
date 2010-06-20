#include <stdlib.h>
#include <unistd.h>

#include "induction.h"
#include "loop.h"
#include "timer.h"
#include "socket.h"
#include "log.h"
#include "init.h"

int i_init (i_resource *self)
{
  /* This sets all induction-related settings back to their defaults.
   *
   * Includes :
   *
   * i_loop settings
   * i_socket settings
   *
   * This is automatically called for all new module from construct_spawn
   */

  int num;

  num = i_loop_init ();
  if (num != 0) { i_printf (1, "i_init failed to initialise i_loop"); return -1; }

  num = i_timer_init ();
  if (num != 0) { i_printf (1, "i_init failed to initialise i_timer"); return -1; }
  
  num = i_socket_callback_init (self);
  if (num != 0) { i_printf (1, "i_init failed to initialise i_socket_callback"); return -1; }
  
  num = i_log_init (self);
  if (num != 0) { i_printf (1, "i_init failed ot initialise i_log"); return -1; }

  return 0;
}


