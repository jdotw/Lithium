#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/hashtable.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/hierarchy.h"
#include "induction/postgresql.h"
#include "induction/interface.h"
#include "induction/inventory.h"
#include "induction/ip.h"
#include "induction/construct.h"
#include "device/snmp.h"
#include "device/snmp_sysinfo.h"
#include "device/snmp_iface.h"
#include "device/snmp_ipaddr.h"
#include "device/snmp_route.h"
#include "device/icmp.h"
#include "device/nwtput.h"

#include "upsinfo.h"
#include "battery.h"
#include "input.h"
#include "inputphase.h"
#include "output.h"
#include "outputphase.h"

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium MGE UPS Vendor Module");
}

char* vendor_name ()
{ return "mgeups"; }
char *vendor_desc ()
{ return "MGE UPS"; }

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "mgeups vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "mgeups vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to initialise l_snmp_objfact"); return -1; }      

  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable SNMP System Information sub-system"); return -1; }
  
  num = v_upsinfo_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable UPS Info sub-system"); return -1; }
  
  num = v_input_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable Input sub-system"); return -1; }
  
  num = v_inputphase_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable Input sub-system"); return -1; }
  
  num = v_battery_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable Battery sub-system"); return -1; }
  
  num = v_output_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable output sub-system"); return -1; }
  
  num = v_outputphase_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable outputphase sub-system"); return -1; }
  
  num = l_snmp_iface_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable SNMP Interface List sub-system"); return -1; }

  num = l_snmp_ipaddr_enable (self);
  if (num != 0)
  { i_printf (1, "mgeups vendor module_entry failed to enable SNMP IP Address List sub-system"); return -1; }

  num = l_icmp_enable (self, 0);
  if (num != 0)
  { i_printf (1, "generic vendor module_entry failed to enable l_icmp"); }

  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "mgeups vendor module_shutdown entered.");
  
  return 0;
}
