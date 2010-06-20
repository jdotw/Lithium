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
#include <induction/container.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/postgresql.h>
#include <induction/interface.h>
#include <induction/inventory.h>
#include <induction/ip.h>
#include <induction/construct.h>
#include <induction/navtree.h>
#include <lithium/snmp.h>
#include <lithium/snmp_sysinfo.h>
#include <lithium/snmp_storage.h>
#include <lithium/snmp_iface.h>
#include <lithium/snmp_ipaddr.h>
#include <lithium/avail.h>
#include <lithium/icmp.h>
#include <lithium/nwtput.h>

#include "temp.h"
#include "humid.h"
#include "state.h"
#include "opstats.h"
#include "unit_config.h"

char* vendor_name ()
{ return "liebert"; }
char *vendor_desc ()
{ return "Liebert Air Conditioner"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Liebert Aircon Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "liebert vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  i_container *cnt;
  
  i_printf (0, "liebert vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed to initialise l_snmp_objfact"); return -1; }      

  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed to enable SNMP System Information sub-system"); return -1; }
  cnt = l_snmp_sysinfo_cnt ();
  cnt->prio = 90;

  num = v_temp_enable (self);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed to enable temperature sub-system"); return -1; }

  num = v_humid_enable (self);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed ot enable humidity sub-system"); return -1; }
  
  num = v_state_enable (self);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed ot enable state sub-system"); return -1; }
  
  num = v_config_enable (self);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed ot enable config sub-system"); return -1; }
  
  num = v_opstats_enable (self);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed ot enable opstats sub-system"); return -1; }
  
  num = l_snmp_iface_enable (self);
  if (num != 0)
  { i_printf (1, "liebert vendor module_entry failed to enable SNMP Interface List sub-system"); return -1; }
  cnt = l_snmp_iface_cnt ();
  cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  num = l_snmp_ipaddr_enable (self);
  if (num != 0)
  { i_printf (1, "cisco vendor module_entry failed to enable SNMP Interface List sub-system"); return -1; }

  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "cisco vendor module_shutdown entered.");
  
  return 0;
}
