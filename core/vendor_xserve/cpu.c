#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "induction/str.h"
#include "device/snmp.h"
#include "device/record.h"

#include "osx_server.h"
#include "data.h"
#include "cpu.h"

/* OS X / Xserve CPU Info */

static i_container *static_cnt = NULL;
static v_cpu_item *static_master_cpu = NULL;

/* Variable Retrieval */

i_container* v_cpu_cnt ()
{ return static_cnt; }

v_cpu_item* v_cpu_master ()
{ return static_master_cpu; }

v_cpu_item* v_cpu_x (int x)
{
  if (!static_cnt) return NULL;

  i_list_move_head (static_cnt->item_list);
  if (!v_xserve_extras())
  { i_list_move_next (static_cnt->item_list); }       /* Move past master object */
  int i;
  for (i=0; i < x; i++)
  { i_list_move_next (static_cnt->item_list); }
  return i_list_restore (static_cnt->item_list);
}

/* Enable / Disable */

int v_cpu_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  if (v_xserve_intel_extras())
  { static_cnt = i_container_create ("xsicpu", "CPU"); }
  else if (v_xserve_extras())
  { static_cnt = i_container_create ("xscpu", "CPU"); }
  else
  { static_cnt = i_container_create ("xcpu", "CPU"); }
  if (!static_cnt)
  { i_printf (1, "v_cpu_enable failed to create container"); v_cpu_disable (self); return -1; }
  static_cnt->mainform_func = v_cpu_cntform;
  static_cnt->sumform_func = v_cpu_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_cpu_enable failed to register container"); v_cpu_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_cpu_enable failed to load and apply container refresh config"); v_cpu_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  /* CPU Usage */
  if (v_xserve_intel_extras())
  {
    tset = i_triggerset_create ("usage_1", "Core A Usage", "usage_1");
    tset->default_duration = 600;
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 85, NULL, 95, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_GT, 95, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
    i_triggerset_assign (self, static_cnt, tset);
    
    tset = i_triggerset_create ("usage_2", "Core B Usage", "usage_2");
    tset->default_duration = 600;
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 85, NULL, 95, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_GT, 95, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
    i_triggerset_assign (self, static_cnt, tset);

//    tset = i_triggerset_create ("v_12v", "12v VRM Input", "v_12v");
//    i_triggerset_addtrg (self, tset, "overvolt", "Over Voltage", VALTYPE_FLOAT, TRGTYPE_GT, 12.6, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//    i_triggerset_addtrg (self, tset, "undervolt", "Under Voltage", VALTYPE_FLOAT, TRGTYPE_LT, 11.34, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//    i_triggerset_assign (self, static_cnt, tset);
                
//    tset = i_triggerset_create ("power_vcore", "Vcore", "power_vcore");
//    i_triggerset_addtrg (self, tset, "overvolt", "Over Voltage", VALTYPE_FLOAT, TRGTYPE_GT, 1.368, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//    i_triggerset_addtrg (self, tset, "undervolt", "Under Voltage", VALTYPE_FLOAT, TRGTYPE_LT, 1.000, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
//    i_triggerset_assign (self, static_cnt, tset);
                
    tset = i_triggerset_create ("temp_heatsink", "Heatsink Temp", "temp_heatsink");
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 75.00, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_assign (self, static_cnt, tset);
  }
  else
  {
    tset = i_triggerset_create ("usage", "Usage", "usage");
    tset->default_duration = 600;
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 85, NULL, 95, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_GT, 95, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
    i_triggerset_assign (self, static_cnt, tset);
  }

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_cpu_enable failed to create item_list"); v_cpu_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master CPU item */
  if (!v_xserve_extras())
  { static_master_cpu = v_cpu_create (self); }

  /*
   * Create Data Metrics 
   */
  v_data_item *dataitem = v_data_static_item();

  /* Standard refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Sysinfo metric */
  dataitem->hardware = i_metric_create ("hardware", "System Info", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->hardware, 0, "Invalid");
  i_metric_enumstr_add (dataitem->hardware, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->hardware));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->hardware), &defrefconfig);
  dataitem->hardware->refresh_func = v_data_info_hardware_refresh;

  return 0;
}

v_cpu_item* v_cpu_create (i_resource *self)
{
  char *name_str;
  char *desc_str;
  v_cpu_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  if (!static_cnt) return NULL;

  /* Set name/desc */
  if (!v_xserve_extras())
  {
    if (static_cnt->item_list->size == 0)
    {
      asprintf (&name_str, "master");
      asprintf (&desc_str, "Master");
    }
    else
    {
      asprintf (&name_str, "cpu_%lu", static_cnt->item_list->size);
      asprintf (&desc_str, "CPU %lu", static_cnt->item_list->size);
    }
  }
  else
  {
      asprintf (&name_str, "cpu_%lu", static_cnt->item_list->size+1);
      asprintf (&desc_str, "CPU %lu", static_cnt->item_list->size+1);
  }

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  obj->mainform_func = v_cpu_objform;
  obj->histform_func = v_cpu_objform_hist;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_cpu_item_create ();
  item->obj = obj;
  obj->itemptr = item;
  i_list_enqueue (static_cnt->item_list, item);

  /*
   * Create Metrics 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  if (!v_xserve_intel_extras())
  {
    item->usage = i_metric_create ("usage", "Utilization", METRIC_FLOAT);
    item->usage->unit_str = strdup ("%");
    item->usage->record_method = RECMETHOD_RRD;
    item->usage->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->usage));
    i_entity_refresh_config_apply (self, ENTITY(item->usage), &refconfig);
  }

  if (v_xserve_extras() && strcmp(obj->name_str, "master") != 0)
  {
    /* Xserve Specific */
    if (v_xserve_intel_extras ())
    {
      /* Intel Xserve */

      item->usage_1 = i_metric_create ("usage_1", "Utilization (Core 1)", METRIC_FLOAT);
      item->usage_1->unit_str = strdup ("%");
      item->usage_1->record_method = RECMETHOD_RRD;
      item->usage_1->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->usage_1));
      i_entity_refresh_config_apply (self, ENTITY(item->usage_1), &refconfig);
  
      item->usage_2 = i_metric_create ("usage_2", "Utilization (Core 2)", METRIC_FLOAT);
      item->usage_2->unit_str = strdup ("%");
      item->usage_2->record_method = RECMETHOD_RRD;
      item->usage_2->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->usage_2));
      i_entity_refresh_config_apply (self, ENTITY(item->usage_2), &refconfig);
  
      item->temp_heatsink = i_metric_create ("temp_heatsink", "Heatsink Temperature", METRIC_FLOAT);
      item->temp_heatsink->unit_str = strdup ("deg.C");
      item->temp_heatsink->record_method = RECMETHOD_RRD;
      item->temp_heatsink->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->temp_heatsink));
      i_entity_refresh_config_apply (self, ENTITY(item->temp_heatsink), &refconfig);

      item->v_12v = i_metric_create ("v_12v", "VRM 12v Input", METRIC_FLOAT);
      item->v_12v->unit_str = strdup ("volts");
      item->v_12v->record_method = RECMETHOD_RRD;
      item->v_12v->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->v_12v));
      i_entity_refresh_config_apply (self, ENTITY(item->v_12v), &refconfig);

    }
    else
    {
      /* PPC Xserve */
      
      item->temp_ambient = i_metric_create ("temp_ambient", "Ambient Temperature", METRIC_FLOAT);
      item->temp_ambient->unit_str = strdup ("deg.C");
      item->temp_ambient->record_method = RECMETHOD_RRD;
      item->temp_ambient->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->temp_ambient));
      i_entity_refresh_config_apply (self, ENTITY(item->temp_ambient), &refconfig);
      
      item->temp_internal = i_metric_create ("temp_internal", "Internal Temperature", METRIC_FLOAT);
      item->temp_internal->unit_str = strdup ("deg.C");
      item->temp_internal->record_method = RECMETHOD_RRD;
      item->temp_internal->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->temp_internal));
      i_entity_refresh_config_apply (self, ENTITY(item->temp_internal), &refconfig);

      item->current_core = i_metric_create ("current_core", "Core Current", METRIC_FLOAT);
      item->current_core->unit_str = strdup ("amps");
      item->current_core->record_method = RECMETHOD_RRD;
      item->current_core->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->current_core));
      i_entity_refresh_config_apply (self, ENTITY(item->current_core), &refconfig);

      item->current_12v = i_metric_create ("current_12v", "12v Current", METRIC_FLOAT);
      item->current_12v->unit_str = strdup ("amps");
      item->current_12v->record_method = RECMETHOD_RRD;
      item->current_12v->record_defaultflag = 1;
      i_entity_register (self, ENTITY(obj), ENTITY(item->current_12v));
      i_entity_refresh_config_apply (self, ENTITY(item->current_12v), &refconfig);

    }

    /* Common */
    
    item->temp_inlet = i_metric_create ("temp_inlet", "Inlet Temperature", METRIC_FLOAT);
    item->temp_inlet->unit_str = strdup ("deg.C");
    item->temp_inlet->record_method = RECMETHOD_RRD;
    item->temp_inlet->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->temp_inlet));
    i_entity_refresh_config_apply (self, ENTITY(item->temp_inlet), &refconfig);

    item->power_watts = i_metric_create ("power_watts", "Power Consumed", METRIC_FLOAT);
    item->power_watts->unit_str = strdup ("Watts");
    item->power_watts->record_method = RECMETHOD_RRD;
    item->power_watts->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->power_watts));
    i_entity_refresh_config_apply (self, ENTITY(item->power_watts), &refconfig);

    item->power_vcore = i_metric_create ("power_vcore", "Vcore", METRIC_FLOAT);
    item->power_vcore->unit_str = strdup ("volts");
    item->power_vcore->record_method = RECMETHOD_RRD;
    item->power_vcore->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->power_vcore));
    i_entity_refresh_config_apply (self, ENTITY(item->power_vcore), &refconfig);

  }

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_cpu_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

