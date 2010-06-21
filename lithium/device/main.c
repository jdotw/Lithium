#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/log.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/opstate.h>
#include <induction/vendor.h>
#include <induction/navtree.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/postgresql.h>

#include "navtree.h"
#include "mformprio.h"
#include "avail.h"
#include "snmp.h"
#include "modb.h"
#include "vendor.h"
#include "refresh.h"
#include "service.h"
#include "icmp.h"
#include "device_xml.h"

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Device Module");
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
  
  /* Configure device entity */
  self->hierarchy->dev->navtree_func = l_navtree_func;    
  self->hierarchy->dev->xml_func = l_device_xml;    

  /* Configure navtree */
  i_navtree_defaultfunc_set (l_navtree_func);
  
  /* 
   * Sub-System Initialisation 
   */

  num = i_log_enable (self, NULL);
  if (num != 0)
  { i_printf (1, "module_init failed to enable logging sub-system"); return -1; }

  num = i_pg_async_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable async postgres sub-system"); return -1; }

  /* Check marking */
  if (self->hierarchy->dev->mark == ENTSTATE_OUTOFSERVICE) return 0;

  /* 
   * Monitoring sub-systems
   */

  num = l_refresh_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "module_init failed to initialise device refresh sub-system"); return -1; }

  num = l_avail_enable (self);
  if (num != 0)
  { i_printf (1, "module_init warning, failed to enable availability recording"); }
    
  num = l_snmp_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable SNMP sub-system"); return -1; }
  
  num = l_service_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable service sub-system"); return -1; }
  
  /* Check protocol */
  if (self->hierarchy->dev->protocol == 0 && strcmp(self->hierarchy->dev->vendor_str, "xraid") != 0)
  {
    /* SNMP, Detect Xsnmp */
    l_snmp_xsnmp_detect (self);
  }
  else
  {
    /* 
     * Non-SNMP, proceed to enabling ModB and Vendor
     */

    /* Module Builder init */
    num = l_modb_init (self);
    if (num != 0)
    { i_printf (1, "module_init failed to initialise module builder sub-systen (unrecoverable)"); return -1; }
  
    /* Vendor initialisation */
    num = l_vendor_init (self);
    if (num != 0)
    { i_printf (1, "module_init failed to initialise vendor module sub-system (unrecoverable)"); return -1; }
  }

  /* ICMP */
  if (self->hierarchy->dev->icmp)
  {
    l_icmp_enable (self, 0);
  }

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

  char *resaddr_str;

  resaddr_str = i_resource_address_struct_to_string (RES_ADDR(self));
  i_printf (0, "module_entry device %s at %s for customer %s online at %s", self->hierarchy->dev->desc_str, self->hierarchy->site->desc_str, self->hierarchy->cust->desc_str, resaddr_str);
  free (resaddr_str);

  sleep (10);

  /* Ready */
  i_resource_send_ready (self);               

  /* Set initial state */
  if (self->hierarchy->dev->mark != 0)
  { i_opstate_change (self, ENTITY(self->hierarchy->dev), self->hierarchy->dev->mark); }
            
  return 0;
}

int module_shutdown (i_resource *self)
{
  i_printf (0, "module_shutdown entered.");
  
  return 0;
}
