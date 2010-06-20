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
#include <induction/hierarchy.h>
#include <induction/postgresql.h>
#include <induction/interface.h>
#include <induction/inventory.h>
#include <induction/ip.h>
#include <induction/construct.h>
#include <lithium/snmp.h>
#include <lithium/snmp_sysinfo.h>
#include <lithium/snmp_iface.h>
#include <lithium/snmp_ipaddr.h>
#include <lithium/snmp_route.h>
#include <lithium/icmp.h>
#include <lithium/nwtput.h>

#include "fan.h"
#include "sensor.h"
#include "fantray.h"
#include "expmodule.h"
#include "power.h"

char* vendor_name ()
{ return "3com"; }
char *vendor_desc ()
{ return "3Com Switch"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium 3Com Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "3com vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "3com vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to initialise l_snmp_objfact"); return -1; }      

  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable SNMP System Information sub-system"); return -1; }

  num = v_fan_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable the fan sub-system"); return -1; }
  
  num = v_sensor_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable the sensor sub-system"); return -1; }
  
  num = v_power_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable the power sub-system"); return -1; }
  
  num = v_fantray_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable the fantray sub-system"); return -1; }
  
  num = v_expmodule_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable the expmodule sub-system"); return -1; }
  
  num = l_nwtput_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable NW Throughput sub-system"); return -1; }
  
  num = l_snmp_iface_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable SNMP Interface List sub-system"); return -1; }

  num = l_snmp_ipaddr_enable (self);
  if (num != 0)
  { i_printf (1, "3com vendor module_entry failed to enable SNMP Interface List sub-system"); return -1; }

  num = l_icmp_enable (self, 0);
  if (num != 0)
  { i_printf (1, "generic vendor module_entry failed to enable l_icmp"); }


  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "3com vendor module_shutdown entered.");
  
  return 0;
}
