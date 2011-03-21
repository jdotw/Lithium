#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/record.h"
#include "device/snmp.h"

#include "envmon.h"

/* NetBotz Environmental Monitoring Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static i_object *static_masterobj = NULL;             /* The 'master' snmp_users obj */

/* Variable Fetching */

i_container* v_envmon_cnt ()
{ return static_cnt; }

i_object* v_envmon_masterobj ()
{ return static_masterobj; }

/* Sub-System Enable / Disable */

int v_envmon_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config refconfig;
  i_object *obj;
  v_envmon_item *env;
  i_triggerset *tset;

  if (static_enabled == 1)
  { i_printf (1, "v_envmon_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("envmon", "Environment");
  if (!static_cnt)
  { i_printf (1, "v_envmon_enable failed to create container"); v_envmon_disable (self); return -1; }
  static_cnt->mainform_func = v_envmon_cntform;
  static_cnt->sumform_func = v_envmon_cntform;
  static_cnt->prio = 2000;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_envmon_enable failed to register container"); v_envmon_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &refconfig);
  if (num != 0)
  { i_printf (1, "v_envmon_enable failed to load and apply container refresh config"); v_envmon_disable (self); return -1; }

  /* Create trigger set */
  tset = i_triggerset_create ("temp", "Temperature", "temp");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 28, NULL, 34, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 34, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /*
   * Item List and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_envmon_enable failed to create item_list"); v_envmon_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create object */
  obj = i_object_create ("master", "Conditions");
  if (!obj)
  { i_printf (1, "v_envmon_enable failed to create object"); return -1; }
  obj->cnt = v_envmon_cnt ();
  obj->mainform_func = v_envmon_objform;
  static_masterobj = obj;

  /* Create item */
  env = v_envmon_item_create ();
  if (!env)
  { i_printf (1, "v_envmon_enable failed to create item"); i_entity_free (obj); return -1; }
  env->obj = obj;
  obj->itemptr = env;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));
  /* Setup refresh configuration default
   * to default to a REFMETHOD_PARENT 
   * refresh method. This default is applied
   * to all metrics and to the object
   * itself
   */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* SNMP Metrics */
  env->temp = l_snmp_metric_create (self, obj, "temp", "Temperature", METRIC_GAUGE, ".1.3.6.1.4.1.5528.100.4.1.1.1.8", "1095346743", RECMETHOD_RRD, 0);
  env->temp->unit_str = strdup ("deg.C");
  env->temp->record_defaultflag = 1;
  env->humidity = l_snmp_metric_create (self, obj, "humidity", "Humidity", METRIC_GAUGE, ".1.3.6.1.4.1.5528.100.4.1.2.1.8", "1094232622", RECMETHOD_RRD, 0);
  env->humidity->unit_str = strdup ("%");
  env->humidity->record_defaultflag = 1;
  env->dewpoint = l_snmp_metric_create (self, obj, "dewpoint", "Dew Point", METRIC_GAUGE, ".1.3.6.1.4.1.5528.100.4.1.3.1.8", "2634294963", RECMETHOD_RRD, 0);
  env->dewpoint->unit_str = strdup ("deg.C");
  env->dewpoint->record_defaultflag = 1;
  env->audio = l_snmp_metric_create (self, obj, "audio", "Noise Level", METRIC_GAUGE, ".1.3.6.1.4.1.5528.100.4.1.4.1.8", "1092397166", RECMETHOD_RRD, 0);
  env->audio->record_defaultflag = 1;
  env->airflow = l_snmp_metric_create (self, obj, "airflow", "Air Flow", METRIC_GAUGE, ".1.3.6.1.4.1.5528.100.4.1.5.1.8", "1092459120", RECMETHOD_RRD, 0);
  env->airflow->unit_str = strdup ("m/min");
  env->airflow->record_defaultflag = 1;
  env->door = l_snmp_metric_create (self, obj, "door", "Door Sensor", METRIC_INTEGER, ".1.3.6.1.4.1.5528.100.4.2.2.1.2", "432254604", RECMETHOD_RRD, 0);
  env->door->record_defaultflag = 1;
  i_metric_enumstr_add (env->door, -1, "Sensor Disconnected");
  i_metric_enumstr_add (env->door, 0, "Door Open");
  i_metric_enumstr_add (env->door, 1, "Door Closed");
  env->motion = l_snmp_metric_create (self, obj, "motion", "Motion", METRIC_INTEGER, ".1.3.6.1.4.1.5528.100.4.2.3.1.2", "434081418", RECMETHOD_RRD, 0);
  env->motion->record_defaultflag = 1;
  i_metric_enumstr_add (env->motion, 0, "No Motion");
  i_metric_enumstr_add (env->motion, 1, "Motion Detected");

  env->campic_small = i_metric_create ("campic_small", "Camera Picture (Small)", METRIC_INTEGER);
  i_metric_enumstr_add (env->campic_small, 0, "Non-current");
  i_metric_enumstr_add (env->campic_small, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(env->campic_small));
  i_entity_refresh_config_apply (self, ENTITY(env->campic_small), &refconfig);
  env->campic_small->refresh_func = v_envmon_campic_refresh;
  env->campic_small->mainform_func = v_envmon_campicform;

  env->campic_large = i_metric_create ("campic_large", "Camera Picture (Large)", METRIC_INTEGER);
  i_metric_enumstr_add (env->campic_large, 0, "Non-current");
  i_metric_enumstr_add (env->campic_large, 1, "Current");
  i_entity_register (self, ENTITY(obj), ENTITY(env->campic_large));
  i_entity_refresh_config_apply (self, ENTITY(env->campic_large), &refconfig);
  env->campic_large->refresh_func = v_envmon_campic_refresh;
  env->campic_large->mainform_func = v_envmon_campicform;

  /* Load/Apply refresh configuration for the object */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);
  if (num != 0)
  {
    i_printf (1, "v_envmon_enable failed to load and apply object refresh config for object master object");
    return -1;
  }

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);
  
  /* Enqueue the snmp_users item */
  i_list_enqueue (static_cnt->item_list, env);

  return 0;  
}

int v_envmon_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "v_envmon_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Deregister/Free container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}


