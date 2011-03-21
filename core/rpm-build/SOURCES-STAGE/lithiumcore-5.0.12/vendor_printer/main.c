#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/hashtable.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/postgresql.h"
#include "induction/interface.h"
#include "induction/construct.h"
#include "device/snmp.h"
#include "device/snmp_sysinfo.h"
#include "device/snmp_iface.h"
#include "device/snmp_ipaddr.h"
#include "device/snmp_swrun.h"
#include "device/snmp_storage.h"
#include "device/snmp_hrcpu.h"
#include "device/snmp_users.h"
#include "device/icmp.h"
#include "device/nwtput.h"

#include "supplies.h"
#include "alert.h"
#include "input.h"

char* vendor_name ()
{ return "printer"; }
char *vendor_desc ()
{ return "Printer with SNMP Support"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Printer Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "printer vendor module_init entered.");
  
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "printer vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to initialise l_snmp_objfact"); return -1; }
  
  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to enable l_snmp_sysinfo"); }

  num = v_supplies_enable (self);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to enable v_supplies"); }
  
  num = v_input_enable (self);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to enable v_input"); }
  
  num = v_alert_enable (self);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to enable v_alert"); }
  
  num = l_snmp_storage_enable (self);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to enable l_snmp_storage"); }
  
  num = l_snmp_iface_enable (self);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to enable l_snmp_iface"); }

  num = l_snmp_ipaddr_enable (self);
  if (num != 0)
  { i_printf (1, "printer vendor module_entry failed to enable SNMP Interface List sub-system"); return -1; }
  
  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "printer vendor module_shutdown entered.");
  
  return 0;
}
