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
#include "device/snmp.h"
#include "device/record.h"

#include "osx_server.h"
#include "data.h"
#include "mainboard.h"

/* Xserve Maim Board Sub-System */

static i_container *static_cnt = NULL;
static v_mainboard_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_mainboard_cnt ()
{ return static_cnt; }

v_mainboard_item* v_mainboard_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_mainboard_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsimainboard", "Main Board");
  if (!static_cnt)
  { i_printf (1, "v_mainboard_enable failed to create container"); v_mainboard_disable (self); return -1; }
  static_cnt->mainform_func = v_mainboard_cntform;
  static_cnt->sumform_func = v_mainboard_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;
  
  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_mainboard_enable failed to register container"); v_mainboard_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_mainboard_enable failed to load and apply container refresh config"); v_mainboard_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_mainboard_enable failed to create item_list"); v_mainboard_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_mainboard_objform;
  obj->histform_func = v_mainboard_objform_hist;

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* 
   * Trigger sets 
   */

  i_triggerset *tset;

  tset = i_triggerset_create ("main_3_3v", "Main 3.3v", "main_3_3v");
  i_triggerset_addtrg (self, tset, "overvolt", "Over Volt", VALTYPE_FLOAT, TRGTYPE_GT, 3.465, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "undervolt", "Under Volt", VALTYPE_FLOAT, TRGTYPE_LT, 3.087, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("main_12v", "Main 12v", "main_12v");
  i_triggerset_addtrg (self, tset, "overvolt", "Over Volt", VALTYPE_FLOAT, TRGTYPE_GT, 12.6, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "undervolt", "Under Volt", VALTYPE_FLOAT, TRGTYPE_LT, 11.34, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("nbridge_temp", "North Bridge Heatsink Temp", "nbridge_temp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 85.0, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /*
   * Create Metrics 
   */ 
  
  /* Create item */
  static_item = v_mainboard_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;
  i_list_enqueue (static_cnt->item_list, static_item);
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  static_item->fbdimm_vrm_12v = i_metric_create ("fbdimm_vrm_12v", "FBDIMM VRM Input", METRIC_FLOAT);
  static_item->fbdimm_vrm_12v->unit_str = strdup ("v");
  static_item->fbdimm_vrm_12v->record_method = RECMETHOD_RRD;
  static_item->fbdimm_vrm_12v->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->fbdimm_vrm_12v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->fbdimm_vrm_12v), &refconfig);

  static_item->main_12v = i_metric_create ("main_12v", "Main 12v", METRIC_FLOAT);
  static_item->main_12v->unit_str = strdup ("v");
  static_item->main_12v->record_method = RECMETHOD_RRD;
  static_item->main_12v->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->main_12v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->main_12v), &refconfig);
  
  static_item->standby = i_metric_create ("standby", "Standby", METRIC_FLOAT);
  static_item->standby->unit_str = strdup ("v");
  static_item->standby->record_method = RECMETHOD_RRD;
  static_item->standby->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->standby));
  i_entity_refresh_config_apply (self, ENTITY(static_item->standby), &refconfig);

  static_item->main_3_3v = i_metric_create ("main_3_3v", "Main 3.3v", METRIC_FLOAT);
  static_item->main_3_3v->unit_str = strdup ("v");
  static_item->main_3_3v->record_method = RECMETHOD_RRD;
  static_item->main_3_3v->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->main_3_3v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->main_3_3v), &refconfig);

  static_item->ns_bridge_power = i_metric_create ("ns_bridge_power", "North and South Bridge Power", METRIC_FLOAT);
  static_item->ns_bridge_power->unit_str = strdup ("watts");
  static_item->ns_bridge_power->record_method = RECMETHOD_RRD;
  static_item->ns_bridge_power->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->ns_bridge_power));
  i_entity_refresh_config_apply (self, ENTITY(static_item->ns_bridge_power), &refconfig);

  static_item->nbridge_temp = i_metric_create ("nbridge_temp", "North Bridge Heatsink Temp", METRIC_FLOAT);
  static_item->nbridge_temp->unit_str = strdup ("deg.C");
  static_item->nbridge_temp->record_method = RECMETHOD_RRD;
  static_item->nbridge_temp->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->nbridge_temp));
  i_entity_refresh_config_apply (self, ENTITY(static_item->nbridge_temp), &refconfig);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return 0;
}

int v_mainboard_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

