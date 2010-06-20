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
#include <induction/hierarchy.h>
#include <induction/postgresql.h>
#include <induction/interface.h>
#include <induction/inventory.h>
#include <induction/ip.h>
#include <induction/construct.h>
#include <lithium/avail.h>
#include <lithium/snmp.h>

#include "plist.h"
#include "sysinfo.h"
#include "status.h"
#include "arrays.h"
#include "drives.h"
#include "setup.h"
#include "hostifaces.h"
#include "power.h"
#include "battery.h"
#include "blower.h"
#include "data.h"

static i_object* static_avail_obj = NULL;
i_object* v_xraid_availobj ()
{ return static_avail_obj; }

char* vendor_name ()
{ return "xraid"; }
char *vendor_desc ()
{ return "Xserve RAID"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Xraid Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "xraid vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "xraid vendor module_entry entered.");

  /* Disable SNMP */
  num = l_snmp_disable (self);

  /* Add Availability Object */
  static_avail_obj = l_avail_object_add (self, "data", "Data Retrieval");
  if (!static_avail_obj)
  { i_printf (1, "xraid vendor module_entry  warning, failed to add availability object"); }

  num = v_sysinfo_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the sysinfo sub-system"); return -1; }

  num = v_drives_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the drives sub-system"); return -1; }

  num = v_arrays_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the arrays sub-system"); return -1; }

  num = v_status_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the status sub-system"); return -1; }

  num = v_setup_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the status sub-system"); return -1; }

  num = v_hostifaces_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the hostifaces sub-system"); return -1; }

  num = v_blower_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the blower sub-system"); return -1; }

  num = v_power_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the power sub-system"); return -1; }

  num = v_battery_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable the battery sub-system"); return -1; }

  num = v_data_enable (self);
  if (num != 0)
  { i_printf (1, "xraid vendor module_entry failed to enable to data retrieval sub-system"); return -1; }

  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "xraid vendor module_shutdown entered.");
  
  return 0;
}
