#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/value.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/str.h>

#include "snmp.h"
#include "record.h"
#include "xsnmp_raidstatus.h"

/* Xsnmp RAID Status */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */

/* Variable Fetching */

i_container* l_xsnmp_raidstatus_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int l_xsnmp_raidstatus_enable (i_resource *self)
{
  int num;
  i_triggerset *tset;
  l_xsnmp_raidstatus_item *raid;
  i_object *obj;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_xsnmp_raidstatus_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsnmp_raidstatus", "RAID Status");
  if (!static_cnt)
  { i_printf (1, "l_xsnmp_raidstatus_enable failed to create container"); return -1; }
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_xsnmp_raidstatus_enable failed to register container"); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_xsnmp_raidstatus_enable failed to raid and apply container refresh config"); return -1; }

  /* 
   * Trigger sets 
   */

  tset = i_triggerset_create ("general_status", "General Status", "general_status");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  tset = i_triggerset_create ("battery_status", "Battery Status", "battery_status");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  /* 
   * Items and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_xsnmp_raidstatus_enable failed to create item_list"); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create object */
  obj = i_object_create ("master", "Master");
  if (!obj)
  { i_printf (1, "l_xsnmp_raidstatus_enable failed to create object"); return -1; }
  obj->cnt = static_cnt;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create item */
  raid = (l_xsnmp_raidstatus_item *) malloc(sizeof(l_xsnmp_raidstatus_item));
  memset (raid, 0, sizeof(l_xsnmp_raidstatus_item));
  raid->obj = obj;
  obj->itemptr = raid;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Set refresh params */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /*
   * Metrics
   */

  raid->general_status = l_snmp_metric_create (self, obj, "general_status", "General Status", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.6.1.1.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (raid->general_status, 1, "Good");
  i_metric_enumstr_add (raid->general_status, 2, "Issued Found");
  i_metric_enumstr_add (raid->general_status, 0, "Unknown");
  raid->general_status_message = l_snmp_metric_create (self, obj, "general_status_message", "General Status Message", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.1.2.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  raid->battery_status = l_snmp_metric_create (self, obj, "battery_status", "Battery Status", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.6.1.3.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (raid->battery_status, 1, "Charged");
  i_metric_enumstr_add (raid->battery_status, 2, "Conditioning");
  i_metric_enumstr_add (raid->battery_status, 3, "Charging");
  i_metric_enumstr_add (raid->battery_status, 4, "Not Charging");
  i_metric_enumstr_add (raid->battery_status, 0, "Unknown");
  raid->battery_status_message = l_snmp_metric_create (self, obj, "battery_status_message", "Battery Status Message", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.1.4.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  raid->hw_version = l_snmp_metric_create (self, obj, "hw_version", "Hardware Version", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.1.5.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  raid->fw_version = l_snmp_metric_create (self, obj, "fw_version", "Firmware Version", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.1.6.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  
  raid->write_cache = l_snmp_metric_create (self, obj, "write_cache", "Write Cache", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.6.1.7.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_metric_enumstr_add (raid->write_cache, 1, "Enabled");
  i_metric_enumstr_add (raid->write_cache, 2, "Disabled");
  i_metric_enumstr_add (raid->write_cache, 0, "Unknown");

  /* Load/Apply refresh configuration for the object */
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &defrefconfig);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);
  
  /* Apply all triggersets */
  num = i_triggerset_evalapprules_allsets (self, obj);
  if (num != 0)
  { i_printf (1, "l_xsnmp_raidstatus_enable warning, failed to apply all triggersets for master object"); }

  /* Enqueue the raid item */
  i_list_enqueue (static_cnt->item_list, raid);

  return 0;
}


