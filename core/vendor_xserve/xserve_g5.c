#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>

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
#include <induction/metric.h>
#include <induction/hierarchy.h>
#include <induction/postgresql.h>
#include <induction/interface.h>
#include <induction/inventory.h>
#include <induction/ip.h>
#include <induction/construct.h>
#include "device/avail.h"
#include "device/snmp.h"

#include "plist.h"
#include "sysinfo.h"
#include "cpu.h"
#include "ram.h"
#include "network.h"
#include "volumes.h"
#include "drives.h"
#include "services.h"
#include "fans.h"
#include "enclosure.h"
#include "power.h"
#include "data.h"
#include "snmp.h"

static i_object* static_avail_obj = NULL;
i_object* v_osx_availobj ()
{ return static_avail_obj; }

static i_object* static_ipmi_avail_obj = NULL;
i_object* v_osx_ipmi_availobj ()
{ return static_ipmi_avail_obj; }

char* vendor_name ()
{ return "xserve_ppc"; }
char *vendor_desc ()
{ return "Xserve G5"; }

int v_xserve_extras ()
{ return 1; }

int v_xserve_intel_extras ()
{ return 0; }

int v_xsan_extras ()
{ return 0; }

static int static_power_state = 1;
int v_powerstate ()
{ return static_power_state; }
void v_powerstate_set (int value)
{ static_power_state = value; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Xserve PPC Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "xserve_ppc vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  i_entity_refresh_config defrefconfig;
  
  i_printf (0, "xserve_ppc vendor module_entry entered.");

  if (self->hierarchy->dev->limited == 1)
  { i_printf (0, "xserve_intel vendor module not supported with a limited license"); return 0; }

  if (self->hierarchy->dev->protocol == 1)
  {
    /* 
     * Servermgrd Selected as Monitoring Protocol
     */
    
    /* Disable SNMP */
    num = l_snmp_disable (self);

    /* Add Availability Object */
    static_avail_obj = l_avail_object_add (self, "data", "Server Data Retrieval");
    if (!static_avail_obj)
    { i_printf (1, "xserve_ppc vendor module_entry  warning, failed to add availability object"); }

    num = v_plist_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable v_plist sub-system"); return -1; }

    num = v_data_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable to data retrieval sub-system"); return -1; }

    num = v_sysinfo_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the sysinfo sub-system"); return -1; }

    num = v_cpu_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the cpu sub-system"); return -1; }

    num = v_ram_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the ram sub-system"); return -1; }

    num = v_fans_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the fans sub-system"); return -1; }

    num = v_enclosure_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the enclosure sub-system"); return -1; }

    num = v_power_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the power sub-system"); return -1; }

    num = v_drives_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the drives sub-system"); return -1; } 

    num = v_volumes_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the volumes sub-system"); return -1; }

    num = v_network_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the network sub-system"); return -1; }

    num = v_services_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the services sub-system"); return -1; }

    /* Create Data Metrics */
    v_data_item *dataitem = v_data_static_item();
    
    /* Sysinfo metric */
    dataitem->xserve = i_metric_create ("xserve", "Xserve", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->xserve, 0, "Invalid");
    i_metric_enumstr_add (dataitem->xserve, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xserve));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->xserve), &defrefconfig);
    dataitem->xserve->refresh_func = v_data_xserve_state_refresh;
  }
  else
  {
    /*
     * SNMP Selected as monitoring protocol
     */

    v_snmp_enable (self);
  }
  
  /* Standard refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "xserve vendor module_shutdown entered.");
  
  return 0;
}
