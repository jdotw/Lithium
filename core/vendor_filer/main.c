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
#include "induction/container.h"
#include "induction/hierarchy.h"
#include "induction/postgresql.h"
#include "induction/interface.h"
#include "induction/construct.h"
#include "device/snmp.h"
#include "device/snmp_sysinfo.h"
#include "device/snmp_iface.h"
#include "device/snmp_ipaddr.h"

#include "cpu.h"
#include "filesys.h"
#include "raid.h"


char* vendor_name ()
{ return "filer"; }
char *vendor_desc ()
{ return "NetApp Filer Appliance"; }

void module_info ()
{
  i_printf (0, "Lithium Filer Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "filer vendor module_init entered.");
  
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "filer vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "filer vendor module_entry failed to initialise l_snmp_objfact"); return -1; }
  
  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "filer vendor module_entry failed to enable l_snmp_sysinfo"); }

  num = v_cpu_enable (self);
  if (num != 0)
  { i_printf (1, "filer vendor module_entry failed to enable v_cpu"); }
  
  num = v_filesys_enable (self);
  if (num != 0)
  { i_printf (1, "filer vendor module_entry failed to enable v_filesys"); }
  
  num = v_raid_enable (self);
  if (num != 0)
  { i_printf (1, "filer vendor module_entry failed to enable v_raid"); }
  
  num = l_snmp_iface_enable (self);
  if (num != 0)
  { i_printf (1, "filer vendor module_entry failed to enable l_snmp_iface"); }
  
  num = l_snmp_ipaddr_enable (self);
  if (num != 0)
  { i_printf (1, "filer vendor module_entry failed to enable l_snmp_ipaddr"); }
        
  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "filer vendor module_shutdown entered.");
  
  return 0;
}
