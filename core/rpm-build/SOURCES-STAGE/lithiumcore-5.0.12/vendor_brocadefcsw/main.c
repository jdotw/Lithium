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
#include "device/procpro.h"
#include "device/snmp_storage.h"
#include "device/snmp_hrcpu.h"
#include "device/snmp_users.h"
#include "device/icmp.h"
#include "device/snmp_nscpu.h"
#include "device/snmp_nsload.h"
#include "device/snmp_nsdiskio.h"
#include "device/snmp_nsram.h"
#include "device/snmp_nsnwtput.h"
#include "device/snmp_nstcpconn.h"

#include "port.h"
#include "sensor.h"

char* vendor_name ()
{ return "brocadefcsw"; }
char *vendor_desc ()
{ return "Brocade Fibre Channel Switch"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Brocade FC Switch Device Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "brocadefcsw vendor module_init entered.");
  
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "brocadefcsw vendor module_entry entered.");

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  if (num != 0)
  { i_printf (1, "brocadefcsw vendor module_entry failed to initialise l_snmp_objfact"); return -1; }
  
  num = l_snmp_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "brocadefcsw vendor module_entry failed to enable l_snmp_sysinfo"); }

  num = v_sensor_enable (self);
  if (num != 0)
  { i_printf (1, "brocadefcsw vendor module_entry failed to enable v_sensor"); }

  num = v_port_enable (self);
  if (num != 0)
  { i_printf (1, "brocadefcsw vendor module_entry failed to enable v_port"); }

  num = l_icmp_enable (self, 0);
  if (num != 0)
  { i_printf (1, "brocadefcsw vendor module_entry failed to enable l_icmp"); }
        
  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "brocadefcsw vendor module_shutdown entered.");
  
  return 0;
}
