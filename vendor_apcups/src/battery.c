#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/record.h>
#include <lithium/snmp.h>

#include "battery.h"

/* UPS Battery Sub-System */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_battery_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_battery_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  static_cnt = i_container_create ("apcbattery", "Battery");
  if (!static_cnt)
  { i_printf (1, "v_battery_enable failed to create container"); v_battery_disable (self); return -1; }
  //static_cnt->mainform_func = v_battery_cntform;
  //static_cnt->sumform_func = v_battery_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_battery_enable failed to register container"); v_battery_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_battery_enable failed to load and apply container refresh config"); v_battery_disable (self); return -1; }

  /*
   * Trigger Sets 
   */

  /* Capacity */
  tset = i_triggerset_create ("capacity", "Capacity", "capacity");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_GAUGE, TRGTYPE_RANGE, 20, NULL, 50, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_GAUGE, TRGTYPE_LT, 20, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("temp", "Temperature", "temp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_GAUGE, TRGTYPE_RANGE, 30, NULL, 40, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_GAUGE, TRGTYPE_GT, 40, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  tset = i_triggerset_create ("status", "Status", "status");
  i_triggerset_addtrg (self, tset, "low", "Low", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_battery_enable failed to create item_list"); v_battery_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_battery_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_battery_enable failed to create master object"); v_battery_disable (self); return -1; }
  //static_obj->mainform_func = v_battery_objform;

  /* Create battery item */
  v_battery_item *battery = v_battery_item_create ();
  if (!battery)
  { i_printf (1, "v_battery_enable failed to create battery item struct"); v_battery_disable (self); return -1; }
  battery->obj = static_obj;
  static_obj->itemptr = battery;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, battery);
  if (num != 0)
  { i_printf (1, "v_battery_enable failed to enqueue item into static_cnt->item_list"); v_battery_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Basic */
  battery->status = l_snmp_metric_create (self, static_obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.318.1.1.1.2.1.1", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (battery->status, 1, "Unknown");
  i_metric_enumstr_add (battery->status, 2, "Battery Normal");
  i_metric_enumstr_add (battery->status, 3, "Battery Low");
  battery->timeonbatt = l_snmp_metric_create (self, static_obj, "timeonbatt", "Time on Battery", METRIC_INTERVAL, ".1.3.6.1.4.1.318.1.1.1.2.1.2", "0", RECMETHOD_NONE, 0);
  battery->lastreplaced = l_snmp_metric_create (self, static_obj, "lastreplaced", "Last Replaced", METRIC_STRING, ".1.3.6.1.4.1.318.1.1.1.2.1.3", "0", RECMETHOD_NONE, 0);

  /* Advanced */
  battery->capacity = l_snmp_metric_create (self, static_obj, "capacity", "Capacity", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.2.2.1", "0", RECMETHOD_RRD, 0);
  battery->capacity->record_defaultflag = 1;
  battery->capacity->unit_str = strdup ("%");
  battery->temp = l_snmp_metric_create (self, static_obj, "temp", "Temperature", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.2.2.2", "0", RECMETHOD_RRD, 0);
  battery->temp->unit_str = strdup ("dec.C");
  battery->temp->record_defaultflag = 1;
  battery->runtime_remaining = l_snmp_metric_create (self, static_obj, "runtime_remaining", "Runtime Remaining", METRIC_INTERVAL, ".1.3.6.1.4.1.318.1.1.1.2.2.3", "0", RECMETHOD_NONE, 0);
  battery->replace_indicator = l_snmp_metric_create (self, static_obj, "replace_indicator", "Replacement Indicator", METRIC_INTEGER, ".1.3.6.1.4.1.318.1.1.1.2.2.4", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (battery->replace_indicator, 1, "No Battery Needs Replacing");
  i_metric_enumstr_add (battery->replace_indicator, 2, "Battery Needs Replacing");
  battery->battpack_count = l_snmp_metric_create (self, static_obj, "battpack_count", "Battery Pack Count", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.2.2.5", "0", RECMETHOD_NONE, 0);
  battery->battpack_badcount = l_snmp_metric_create (self, static_obj, "battpack_badcount", "Battery Pack Bad Count", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.2.2.6", "0", RECMETHOD_NONE, 0);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_battery_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

