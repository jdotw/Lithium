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
#include <induction/xsanregistry.h>
#include <lithium/avail.h>
#include <lithium/snmp.h>

#include "osx_server.h"
#include "xsan.h"
#include "xsanvol.h"
#include "xsaninfo.h"
#include "xsanvisdisk.h"
#include "plist.h"
#include "sysinfo.h"
#include "cpu.h"
#include "network.h"
#include "volumes.h"
#include "services.h"
#include "data.h"
#include "snmp.h"

static i_object* static_avail_obj = NULL;
i_object* v_osx_availobj ()
{ return static_avail_obj; }

static i_object* static_ipmi_avail_obj = NULL;
i_object* v_osx_ipmi_availobj ()
{ return static_ipmi_avail_obj; }

char* vendor_name ()
{ return "xsan_client"; }
char *vendor_desc ()
{ return "Xsan Client (Mac OS X Client)"; }

int v_xserve_extras ()
{ return 0; }

int v_xserve_intel_extras ()
{ return 0; }

int v_xsan_extras ()
{ return 1; }

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
  
  i_printf (0, "Lithium Xsan Client Device Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "xsan_client vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "xsan_client vendor module_entry entered.");

  if (self->hierarchy->dev->protocol == 1)
  {
    /* 
     * Xsan Client Servermgrd selected as protocol 
     */

    /* Disable SNMP */
    num = l_snmp_disable (self);

    /* Add Availability Object */
    static_avail_obj = l_avail_object_add (self, "data", "Server Data Retrieval");
    if (!static_avail_obj)
    { i_printf (1, "xsan_client vendor module_entry  warning, failed to add availability object"); }

    num = v_plist_enable (self);
    if (num != 0)
    { i_printf (1, "xsan_client vendor module_entry failed to enable v_plist sub-system"); return -1; }

    num = v_data_enable (self);
    if (num != 0)
    { i_printf (1, "xsan_client vendor module_entry failed to enable to data retrieval sub-system"); return -1; }

    num = v_xsan_enable (self);
    if (num != 0)
    { i_printf (1, "v_xsan_enable failed to enable the xsan sub-system"); return -1; }
  }
  else
  {
    v_snmp_enable (self);
  }

  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "xsan_client vendor module_shutdown entered.");
  
  return 0;
}
