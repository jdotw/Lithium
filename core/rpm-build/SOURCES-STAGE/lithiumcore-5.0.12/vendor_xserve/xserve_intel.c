#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/hashtable.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/hierarchy.h"
#include "induction/postgresql.h"
#include "induction/interface.h"
#include "induction/inventory.h"
#include "induction/ip.h"
#include "induction/construct.h"
#include "device/avail.h"
#include "device/snmp.h"

#include "plist.h"
#include "sysinfo.h"
#include "chassis.h"
#include "cpu.h"
#include "ram.h"
#include "mainboard.h"
#include "psu.h"
#include "pci.h"
#include "network.h"
#include "volumes.h"
#include "drives.h"
#include "services.h"
#include "fans.h"
#include "data.h"
#include "snmp.h"

static i_object* static_avail_obj = NULL;
i_object* v_osx_availobj ()
{ return static_avail_obj; }

static i_object* static_ipmi_avail_obj = NULL;
i_object* v_osx_ipmi_availobj ()
{ return static_ipmi_avail_obj; }

char* vendor_name ()
{ return "xserve_intel"; }
char *vendor_desc ()
{ return "Xserve Intel Xeon"; }

int v_xserve_extras ()
{ return 1; }

int v_xserve_intel_extras ()
{ return 1; }

int v_xsan_extras ()
{ return 0; }

static int static_power_state = 1;
int v_powerstate ()
{ return static_power_state; }
void v_powerstate_set (int value)
{ 
  int old_state = static_power_state;
  static_power_state = value; 
  if (old_state == 0 && static_power_state == 1)
  {
    /* Device is now powered ON */

  }
  else
  {
    /* Device is now powered OFF */

  }
}

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Xserve Intel Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "xserve_intel vendor module_init entered.");

  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "xserve_intel vendor module_entry entered.");

  if (self->hierarchy->dev->limited == 1)
  { i_printf (0, "xserve_intel vendor module not supported with a limited license"); return 0; }

  if (self->hierarchy->dev->protocol == 1)
  {
    /*
     * Servermgrd selected as monitoring protocol
     */

    /* Disable SNMP */
    num = l_snmp_disable (self);

    /* Add Availability Object */
    static_avail_obj = l_avail_object_add (self, "data", "Server Data Retrieval");
    if (!static_avail_obj)
    { i_printf (1, "xserve vendor module_entry  warning, failed to add availability object"); }

    num = v_plist_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable v_plist sub-system"); return -1; }
  }

  if (self->hierarchy->dev->protocol == 1 || self->hierarchy->dev->lom == 1)
  {
    if (self->hierarchy->dev->lom == 1 && !l_snmp_xsnmp_enabled())
    {
      /* Add Availability Object */
      static_ipmi_avail_obj = l_avail_object_add (self, "ipmi", "IPMI");
      if (!static_ipmi_avail_obj)
      { i_printf (1, "xserve vendor module_entry  warning, failed to add IPMI availability object"); }
    }

    num = v_data_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable to data retrieval sub-system"); return -1; }
  }

  if (self->hierarchy->dev->lom == 1 && !l_snmp_xsnmp_enabled())
  {
    /*
     * Create Data Metrics 
     */

    v_data_ipmi_item *dataitem = v_data_static_ipmi_item();
  
    /* Standard refresh config */
    i_entity_refresh_config defrefconfig;
    memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
    defrefconfig.refresh_method = REFMETHOD_PARENT;
    defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

    /* Chassis Data metric */
    dataitem->chassis = i_metric_create ("chassis", "IPMI chassis", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->chassis, 0, "Invalid");
    i_metric_enumstr_add (dataitem->chassis, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->chassis));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->chassis), &defrefconfig);
    dataitem->chassis->refresh_func = v_data_ipmi_chassis_refresh;
  
    /* Serial Number Data metric */
    dataitem->serialnum = i_metric_create ("serialnum", "IPMI serialnum", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->serialnum, 0, "Invalid");
    i_metric_enumstr_add (dataitem->serialnum, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->serialnum));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->serialnum), &defrefconfig);
    dataitem->serialnum->refresh_func = v_data_ipmi_serial_refresh;
  
    /* SDR Data metric */
    dataitem->sdr = i_metric_create ("sdr", "IPMI SDR", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->sdr, 0, "Invalid");
    i_metric_enumstr_add (dataitem->sdr, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->sdr));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->sdr), &defrefconfig);
    dataitem->sdr->refresh_func = v_data_ipmi_sdr_refresh;
  
    /* Static drive data metric */
    dataitem->ipmi_drive_static = i_metric_create ("ipmi_drive_static", "IPMI Drive Static", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->ipmi_drive_static, 0, "Invalid");
    i_metric_enumstr_add (dataitem->ipmi_drive_static, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipmi_drive_static));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->ipmi_drive_static), &defrefconfig);
    dataitem->ipmi_drive_static->refresh_func = v_data_ipmi_drive_static_refresh;
  
    /* Dynamic drive metric */
    dataitem->ipmi_drive_dynamic = i_metric_create ("ipmi_drive_dynamic", "IPMI Drive dynamic", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->ipmi_drive_dynamic, 0, "Invalid");
    i_metric_enumstr_add (dataitem->ipmi_drive_dynamic, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipmi_drive_dynamic));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->ipmi_drive_dynamic), &defrefconfig);
    dataitem->ipmi_drive_dynamic->refresh_func = v_data_ipmi_drive_dynamic_refresh;
  
    if (self->hierarchy->dev->protocol == 1)
    {
      /* Static network static metric */
      dataitem->ipmi_network_static = i_metric_create ("ipmi_network_static", "IPMI network Static", METRIC_INTEGER);
      i_metric_enumstr_add (dataitem->ipmi_network_static, 0, "Invalid");
      i_metric_enumstr_add (dataitem->ipmi_network_static, 1, "Current");
      i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipmi_network_static));
      i_entity_refresh_config_apply (self, ENTITY(dataitem->ipmi_network_static), &defrefconfig);
      dataitem->ipmi_network_static->refresh_func = v_data_ipmi_network_static_refresh;

      /* Dynamic network dynamic metric */
      dataitem->ipmi_network_dynamic = i_metric_create ("ipmi_network_dynamic", "IPMI network dynamic", METRIC_INTEGER);
      i_metric_enumstr_add (dataitem->ipmi_network_dynamic, 0, "Invalid");
      i_metric_enumstr_add (dataitem->ipmi_network_dynamic, 1, "Current");
      i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipmi_network_dynamic));
      i_entity_refresh_config_apply (self, ENTITY(dataitem->ipmi_network_dynamic), &defrefconfig);
      dataitem->ipmi_network_dynamic->refresh_func = v_data_ipmi_network_dynamic_refresh;
    }
  
    /* Static RAM data metric */
    dataitem->ipmi_ram_static = i_metric_create ("ipmi_ram_static", "IPMI ram static", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->ipmi_ram_static, 0, "Invalid");
    i_metric_enumstr_add (dataitem->ipmi_ram_static, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipmi_ram_static));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->ipmi_ram_static), &defrefconfig);
    dataitem->ipmi_ram_static->refresh_func = v_data_ipmi_ram_static_refresh;

    /* Dynamic RAM dynamic metric */
    dataitem->ipmi_ram_dynamic = i_metric_create ("ipmi_ram_dynamic", "IPMI ram dynamic", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->ipmi_ram_dynamic, 0, "Invalid");
    i_metric_enumstr_add (dataitem->ipmi_ram_dynamic, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipmi_ram_dynamic));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->ipmi_ram_dynamic), &defrefconfig);
    dataitem->ipmi_ram_dynamic->refresh_func = v_data_ipmi_ram_dynamic_refresh;

    if (self->hierarchy->dev->protocol == 1)
    {
      /* CPU data metric */
      dataitem->ipmi_cpu = i_metric_create ("ipmi_cpu", "IPMI cpu", METRIC_INTEGER);
      i_metric_enumstr_add (dataitem->ipmi_cpu, 0, "Invalid");
      i_metric_enumstr_add (dataitem->ipmi_cpu, 1, "Current");
      i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->ipmi_cpu));
      i_entity_refresh_config_apply (self, ENTITY(dataitem->ipmi_cpu), &defrefconfig);
      dataitem->ipmi_cpu->refresh_func = v_data_ipmi_cpu_refresh;
    }
  }

  /* 
   * Enable sub-systems 
   */

  if (self->hierarchy->dev->lom == 1 && !l_snmp_xsnmp_enabled())
  {
    num = v_chassis_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the chassis sub-system"); return -1; }

    num = v_ram_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the ram sub-system"); return -1; }

    num = v_mainboard_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the mainboard sub-system"); return -1; }

    num = v_pci_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the pci sub-system"); return -1; }

    num = v_psu_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the psu sub-system"); return -1; }

    num = v_fans_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the fans sub-system"); return -1; }

    num = v_drives_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the drives sub-system"); return -1; }
  }

  if (self->hierarchy->dev->protocol == 1)
  {
    /*
     * Servermgrd Selected as Monitoring Protocol
     */

    num = v_sysinfo_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the sysinfo sub-system"); return -1; }

    num = v_cpu_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the cpu sub-system"); return -1; }

    num = v_volumes_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the volumes sub-system"); return -1; }

    num = v_network_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the network sub-system"); return -1; }

    num = v_services_enable (self);
    if (num != 0)
    { i_printf (1, "xserve vendor module_entry failed to enable the services sub-system"); return -1; }
  }
  else
  {
    /*
     * SNMP Selected as monitoring protocol 
     */
    v_snmp_enable (self);
  }


  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "xserve vendor module_shutdown entered.");
  
  return 0;
}
