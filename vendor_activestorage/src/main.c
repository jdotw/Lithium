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

#include "system.h"
#include "ctrl.h"
#include "fibreport.h"
#include "drive.h"
#include "raidset.h"
#include "fans.h"
#include "temp.h"
#include "battery.h"
#include "psu.h"
#include "voltage.h"
#include "volume.h"

char* vendor_name ()
{ return "activestorage"; }
char *vendor_desc ()
{ return "Active Storage"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Active Storage Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "activestorage vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "activestorage vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "activestorage vendor module_entry failed to initialise l_snmp_objfact"); return -1; }      

  v_system_enable (self);
  v_ctrl_enable (self);
  v_drive_enable (self);
  v_raidset_enable (self);
  v_volume_enable (self);
  v_battery_enable (self);
  v_fibreport_enable (self);
  v_temp_enable (self);
  v_fans_enable (self);
  v_psu_enable (self);
  v_voltage_enable (self);

  num = l_snmp_ipaddr_enable (self);
  if (num != 0)
  { i_printf (1, "activestorage vendor module_entry failed to enable SNMP Interface List sub-system"); return -1; }

  num = l_icmp_enable (self, 0);
  if (num != 0)
  { i_printf (1, "generic vendor module_entry failed to enable l_icmp"); }

  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "activestorage vendor module_shutdown entered.");
  
  return 0;
}
