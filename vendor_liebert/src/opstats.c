#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "opstats.h"

/* Operation Statistics (run times) */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int v_opstats_enabled ()
{ return static_enabled; }

i_container* v_opstats_cnt ()
{ return static_cnt; }

i_object* v_opstats_obj ()
{ return static_obj; }

/* Enable / Disable */

int v_opstats_enable (i_resource *self)
{
  int num;
  v_opstats_item *opstats;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "v_opstats_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("opstats", "Component Run Times");
  if (!static_cnt)
  { i_printf (1, "v_opstats_enable failed to create container"); v_opstats_disable (self); return -1; }
  static_cnt->mainform_func = v_opstats_cntform;
  static_cnt->sumform_func = v_opstats_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_opstats_enable failed to register container"); v_opstats_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_opstats_enable failed to load and apply container refresh config"); v_opstats_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_opstats_enable failed to create item_list"); v_opstats_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_opstats_enable failed to create master object"); v_opstats_disable (self); return -1; }
  static_obj->mainform_func = v_opstats_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create opstats item */
  opstats = v_opstats_item_create ();
  if (!opstats)
  { i_printf (1, "v_opstats_enable failed to create static_opstats struct"); v_opstats_disable (self); return -1; }
  opstats->obj = static_obj;
  static_obj->itemptr = opstats;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, opstats);
  if (num != 0)
  { i_printf (1, "v_opstats_enable failed to enqueue item into static_cnt->item_list"); v_opstats_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  opstats->comp1_rt = l_snmp_metric_create (self, static_obj, "comp1_rt", "Compressor 1", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.1.0", NULL, RECMETHOD_NONE, 0);
  opstats->comp1_rt->unit_str = strdup ("hours");

  opstats->comp2_rt = l_snmp_metric_create (self, static_obj, "comp2_rt", "Compressor 2", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.2.0", NULL, RECMETHOD_NONE, 0);
  opstats->comp2_rt->unit_str = strdup ("hours");

  opstats->fan_rt = l_snmp_metric_create (self, static_obj, "fan_rt", "Fan", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.3.0", NULL, RECMETHOD_NONE, 0);
  opstats->fan_rt->unit_str = strdup ("hours");

  opstats->humidifier_rt = l_snmp_metric_create (self, static_obj, "humidifier_rt", "Humidifier", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.4.0", NULL, RECMETHOD_NONE, 0);
  opstats->humidifier_rt->unit_str = strdup ("hours");

  opstats->reheat1_rt = l_snmp_metric_create (self, static_obj, "reheat1_rt", "Reheating Element 1", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.7.0", NULL, RECMETHOD_NONE, 0);
  opstats->reheat1_rt->unit_str = strdup ("hours");

  opstats->reheat2_rt = l_snmp_metric_create (self, static_obj, "reheat2_rt", "Reheating Element 2", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.8.0", NULL, RECMETHOD_NONE, 0);
  opstats->reheat2_rt->unit_str = strdup ("hours");

  opstats->cooling_rt = l_snmp_metric_create (self, static_obj, "cooling_rt", "Cooling Mode", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.10.0", NULL, RECMETHOD_NONE, 0);
  opstats->cooling_rt->unit_str = strdup ("hours");

  opstats->heating_rt = l_snmp_metric_create (self, static_obj, "heating_rt", "Heating Mode", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.11.0", NULL, RECMETHOD_NONE, 0);
  opstats->heating_rt->unit_str = strdup ("hours");

  opstats->humidifying_rt = l_snmp_metric_create (self, static_obj, "humidifying_rt", "Humidifying Mode", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.12.0", NULL, RECMETHOD_NONE, 0);
  opstats->humidifying_rt->unit_str = strdup ("hours");

  opstats->dehumidifying_rt = l_snmp_metric_create (self, static_obj, "dehumidifying_rt", "Dehumidifying Mode", METRIC_GAUGE, "enterprises.476.1.42.3.4.6.13.0", NULL, RECMETHOD_NONE, 0);
  opstats->dehumidifying_rt->unit_str = strdup ("hours");

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);
  
  return 0;
}

int v_opstats_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_opstats_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    v_opstats_item *item = static_obj->itemptr;
    if (item) { v_opstats_item_free (item); }
    i_entity_deregister (self, ENTITY(static_obj));
    i_entity_free (ENTITY(static_obj));
    static_obj = NULL;
  }

  if (static_cnt)
  {
    i_entity_deregister (self, ENTITY(static_cnt));
    i_entity_free (ENTITY(static_cnt));
    static_cnt = NULL;
  }

  return 0;
}

