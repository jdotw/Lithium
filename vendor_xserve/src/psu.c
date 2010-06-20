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
#include <induction/str.h>
#include <induction/name.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "osx_server.h"
#include "data.h"
#include "psu.h"

/* Intel Xserve PSU Info */

static i_container *static_cnt = NULL;

/* Variable Retrieval */

i_container* v_psu_cnt ()
{ return static_cnt; }

v_psu_item* v_psu_get (char *desc_str)
{
  v_psu_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->desc_str, desc_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_psu_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsipsu", "Power Supplies");
  if (!static_cnt)
  { i_printf (1, "v_psu_enable failed to create container"); v_psu_disable (self); return -1; }
  static_cnt->mainform_func = v_psu_cntform;
  static_cnt->sumform_func = v_psu_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_psu_enable failed to register container"); v_psu_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_psu_enable failed to load and apply container refresh config"); v_psu_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  tset = i_triggerset_create ("fan_in_rpm", "Inlet Fan RPM", "fan_in_rpm");
  i_triggerset_addtrg (self, tset, "overspeed", "Over Speed", VALTYPE_FLOAT, TRGTYPE_GT, 18000, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "underspeed", "Under Speed", VALTYPE_FLOAT, TRGTYPE_LT, 1000, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
                
  tset = i_triggerset_create ("fan_out_rpm", "Outlet Fan RPM", "fan_out_rpm");
  i_triggerset_addtrg (self, tset, "overspeed", "Over Speed", VALTYPE_FLOAT, TRGTYPE_GT, 18000, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "underspeed", "Under Speed", VALTYPE_FLOAT, TRGTYPE_LT, 1000, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
                
  tset = i_triggerset_create ("exhaust_temp", "Exhaust Temp", "exhaust_temp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 65, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
                
  tset = i_triggerset_create ("v_5v_standby", "5v Standby", "v_5v_standby");
  i_triggerset_addtrg (self, tset, "overvolt", "Over Voltage", VALTYPE_FLOAT, TRGTYPE_GT, 5.29, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "undervolt", "Under Voltage", VALTYPE_FLOAT, TRGTYPE_LT, 4.72, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
                
  tset = i_triggerset_create ("v_12v", "12v Feed", "v_12v");
  i_triggerset_addtrg (self, tset, "overvolt", "Over Voltage", VALTYPE_FLOAT, TRGTYPE_GT, 12.6, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "undervolt", "Under Voltage", VALTYPE_FLOAT, TRGTYPE_LT, 11.34, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
                

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_psu_enable failed to create item_list"); v_psu_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  return 0;
}

v_psu_item* v_psu_create (i_resource *self, char *name)
{
  char *name_str;
  char *desc_str;
  v_psu_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Set name/desc */
  name_str = strdup (name);
  i_name_parse (name_str);
  desc_str = strdup (name);

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  obj->mainform_func = v_psu_objform;
  obj->histform_func = v_psu_objform_hist;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_psu_item_create ();
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

  item->exhaust_temp = i_metric_create ("exhaust_temp", "Exhaust Temp", METRIC_FLOAT);
  item->exhaust_temp->unit_str = strdup ("deg.C");
  item->exhaust_temp->record_method = RECMETHOD_RRD;
  item->exhaust_temp->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(item->exhaust_temp));
  i_entity_refresh_config_apply (self, ENTITY(item->exhaust_temp), &refconfig);

  item->fan_in_rpm = i_metric_create ("fan_in_rpm", "Fan Input Speed", METRIC_FLOAT);
  item->fan_in_rpm->unit_str = strdup ("rpm");
  item->fan_in_rpm->record_method = RECMETHOD_RRD;
  item->fan_in_rpm->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(item->fan_in_rpm));
  i_entity_refresh_config_apply (self, ENTITY(item->fan_in_rpm), &refconfig);

  item->fan_out_rpm = i_metric_create ("fan_out_rpm", "Fan Output Speed", METRIC_FLOAT);
  item->fan_out_rpm->unit_str = strdup ("rpm");
  item->fan_out_rpm->record_method = RECMETHOD_RRD;
  item->fan_out_rpm->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(item->fan_out_rpm));
  i_entity_refresh_config_apply (self, ENTITY(item->fan_out_rpm), &refconfig);

  item->v_5v_standby = i_metric_create ("v_5v_standby", "5v Standby", METRIC_FLOAT);
  item->v_5v_standby->unit_str = strdup ("volts");
  item->v_5v_standby->record_method = RECMETHOD_RRD;
  item->v_5v_standby->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(item->v_5v_standby));
  i_entity_refresh_config_apply (self, ENTITY(item->v_5v_standby), &refconfig);

  item->v_12v = i_metric_create ("v_12v", "12v", METRIC_FLOAT);
  item->v_12v->unit_str = strdup ("volts");
  item->v_12v->record_method = RECMETHOD_RRD;
  item->v_12v->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(item->v_12v));
  i_entity_refresh_config_apply (self, ENTITY(item->v_12v), &refconfig);

  item->power = i_metric_create ("power", "Power", METRIC_FLOAT);
  item->power->unit_str = strdup ("watts");
  item->power->record_method = RECMETHOD_RRD;
  item->power->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(item->power));
  i_entity_refresh_config_apply (self, ENTITY(item->power), &refconfig);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_psu_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

