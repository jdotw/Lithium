#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/object.h>
#include <induction/hierarchy.h>
#include <induction/postgresql.h>
#include <induction/interface.h>
#include <induction/inventory.h>
#include <induction/ip.h>
#include <induction/construct.h>
#include <lithium/snmp.h>
#include <lithium/snmp_sysinfo.h>
#include <lithium/snmp_ipaddr.h>
#include <lithium/snmp_route.h>
#include <lithium/icmp.h>
#include <lithium/nwtput.h>

#include "client.h"
#include "clientcount.h"
#include "physical.h"
#include "sysinfo_refcb.h"

static int  static_nseries_extras = 0;
int v_nseries_extras ()
{ return static_nseries_extras; }
void v_nseries_extras_set (int value)
{ static_nseries_extras = value; }

char* vendor_name ()
{ return "airport"; }
char *vendor_desc ()
{ return "Airport Base Station"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Airport Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "airport vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "airport vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "airport vendor module_entry failed to initialise l_snmp_objfact"); return -1; }      

  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "airport vendor module_entry failed to enable SNMP System Information sub-system"); return -1; }
  i_object *sysinfo_obj = l_snmp_sysinfo_obj ();
  i_entity_refreshcb_add (ENTITY(sysinfo_obj), v_sysinfo_refcb, NULL);
  
  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "cisco vendor module_shutdown entered.");
  
  return 0;
}
