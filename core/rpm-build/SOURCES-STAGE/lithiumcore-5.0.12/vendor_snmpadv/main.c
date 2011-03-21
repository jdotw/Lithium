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
//#include <lithium/snmp_hrprocessor.h>
#include "device/snmp_iface.h"
#include "device/snmp_ipaddr.h"
#include "device/snmp_swrun.h"
#include "device/procpro.h"
#include "device/snmp_storage.h"
#include "device/snmp_hrcpu.h"
#include "device/snmp_users.h"
#include "device/icmp.h"
#include "device/nwtput.h"

char* vendor_name ()
{ return "snmpadv"; }
char *vendor_desc ()
{ return "Generic SNMP (Advanced)"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Advanced SNMP Device Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "snmpadv vendor module_init entered.");
  
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "snmpadv vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to initialise l_snmp_objfact"); return -1; }
  
  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_snmp_sysinfo"); }
  
  num = l_snmp_hrcpu_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_snmp_hrcpu"); }
  
  num = l_snmp_users_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_snmp_users"); }
  
  num = l_snmp_storage_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_snmp_storage"); }

  num = l_nwtput_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_nwtput"); }
  
  num = l_snmp_iface_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_snmp_iface"); }
  
  num = l_snmp_ipaddr_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_snmp_ipaddr"); }

  num = l_snmp_swrun_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_snmp_swrun"); }

  num = l_procpro_enable (self);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_procpro"); }

  /* Enable ICMP */
  num = l_icmp_enable (self, 0);
  if (num != 0)
  { i_printf (1, "snmpadv vendor module_entry failed to enable l_icmp"); }
        
  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "snmpadv vendor module_shutdown entered.");
  
  return 0;
}
