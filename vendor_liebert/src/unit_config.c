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

#include "unit_config.h"

/* Unit Config */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int v_config_enabled ()
{ return static_enabled; }

i_container* v_config_cnt ()
{ return static_cnt; }

i_object* v_config_obj ()
{ return static_obj; }

/* Enable / Disable */

int v_config_enable (i_resource *self)
{
  int num;
  v_config_item *config;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "v_config_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("config", "Configuration");
  if (!static_cnt)
  { i_printf (1, "v_config_enable failed to create container"); v_config_disable (self); return -1; }
  static_cnt->mainform_func = v_config_cntform;
  static_cnt->sumform_func = v_config_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_config_enable failed to register container"); v_config_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_config_enable failed to load and apply container refresh config"); v_config_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_config_enable failed to create item_list"); v_config_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_config_enable failed to create master object"); v_config_disable (self); return -1; }
  static_obj->mainform_func = v_config_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create config item */
  config = v_config_item_create ();
  if (!config)
  { i_printf (1, "v_config_enable failed to create static_config struct"); v_config_disable (self); return -1; }
  config->obj = static_obj;
  static_obj->itemptr = config;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, config);
  if (num != 0)
  { i_printf (1, "v_config_enable failed to enqueue item into static_cnt->item_list"); v_config_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  config->restart_delay = l_snmp_metric_create (self, static_obj, "restart_delay", "Restart Delay", METRIC_GAUGE, "enterprises.476.1.42.3.4.4.4.0", NULL, RECMETHOD_NONE, 0);

  config->remote_shutdown = l_snmp_metric_create (self, static_obj, "remote_shutdown", "Remote Shutdown", METRIC_INTEGER, "enterprises.476.1.42.3.4.4.7.0", NULL, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (config->remote_shutdown, 1, "Disabled");
  i_metric_enumstr_add (config->remote_shutdown, 2, "Enabled");

  config->cooling_maint = l_snmp_metric_create (self, static_obj, "cooling_maint", "Cooling System Maintenance Interval", METRIC_GAUGE, "enterprises.476.1.42.3.4.4.8.0", NULL, RECMETHOD_NONE, 0);
  config->cooling_maint->unit_str = strdup ("hours");

  config->humidifier_maint = l_snmp_metric_create (self, static_obj, "humidifier_maint", "Humidifier System Maintenance Interval", METRIC_GAUGE, "enterprises.476.1.42.3.4.4.9.0", NULL, RECMETHOD_NONE, 0);
  config->humidifier_maint->unit_str = strdup ("hours");

  config->filter_maint = l_snmp_metric_create (self, static_obj, "filter_maint", "Filter Maintenance Interval", METRIC_GAUGE, "enterprises.476.1.42.3.4.4.10.0", NULL, RECMETHOD_NONE, 0);
  config->filter_maint->unit_str = strdup ("hours");

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);
  
  return 0;
}

int v_config_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_config_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    v_config_item *item = static_obj->itemptr;
    if (item) { v_config_item_free (item); }
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

