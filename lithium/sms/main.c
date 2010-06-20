#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/construct.h>

#include "gsmdevice.h"
#include "handler.h"

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium SMS Transceiver");
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
  l_gsmdevice *device;
  
  i_printf (0, "module_init entered.");

  device = l_gsmdevice_create ();
  if (!device)
  { i_printf (1, "module_init failed to create device struct"); return -1; }

  num = l_gsmdevice_init (self, device);
  if (num != 0)
  { i_printf (1, "module_init failed to initialise GSM hardware"); return -1; }

  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_SMS_SEND, l_sms_send_handler, device);
  if (!hdlr)
  { i_printf (1, "module_init failed to add i_msgproc handler for MSG_SMS_SEND"); return -1; }
  
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

  i_printf (0, "module_entry entered.");

  return 0;
}

int module_shutdown (i_resource *self)
{
  i_printf (0, "module_shutdown entered.");
  
  return 0;
}
