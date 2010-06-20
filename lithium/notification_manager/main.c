#include <stdio.h>
#include <stdlib.h>

#include <induction.h>

#include "handler.h"

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Notification Manager Module");
}

int module_sanity_check (i_construct *construct)
{

  /* 
   * This is where any pre-spawning sanity checking is done.
   * Most important is to verify that the configuration passed
   * to the function will result in a module that will spawn
   * and actually work
   *
   * return 0 on 'all ok'
   * return -1 on 'failure'
   *
   */
  
  return 0;
}

int module_init (i_resource *self)
{
  /*
   * This is where initialisation of the module occurs.
   * A crucial step here is for the module to register itself
   * with the core. Otherwise the core will attempt to respawn 
   * the module.
   *
   * return 0 on 'all ok'
   * return -1 on 'failure'
   *
   */

  int num;
  i_msgproc_handler *hdlr;
  
  i_printf (0, "module_init entered.");
  i_status_set (self, STATUS_NORMAL, "init", "Resource Initialising");

  num = i_log_enable (self, NULL);
  if (num != 0)
  { i_printf (1, "module_init warning, failed to enable i_log"); }

  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_NOTIFICATION, l_notification_handler, NULL);
  if (!hdlr)
  { i_printf (0, "module_init failed to add message handler for MSG_NOFICIATION"); return -1; }
  
  return 0;
}

int module_entry (i_resource *self)
{
  /*
   * This is the main() equivalent for a lithium module.
   * Run the module from this function.
   *
   * return 0 on 'successful run'
   * return -1 on 'failed run'
   *
   */

  int num;
  
  i_printf (0, "module_entry entered.");

  i_status_clear (self, "init");
  i_status_set (self, STATUS_NORMAL, "Running", "Resource Running");      
  
  return 0;
}

int module_shutdown (i_resource *self)
{
  i_printf (0, "module_shutdown entered.");
  
  return 0;
}
