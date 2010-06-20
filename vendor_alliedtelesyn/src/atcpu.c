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
#include <induction/value.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "atcpu.h"

/* Allied Telesyn CPU Utilisation */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int v_atcpu_enabled ()
{ return static_enabled; }

i_container* v_atcpu_cnt ()
{ return static_cnt; }

i_object* v_atcpu_obj ()
{ return static_obj; }

/* Enable / Disable */

int v_atcpu_enable (i_resource *self)
{
  int num;
  v_atcpu_item *cpu;
  i_triggerset *tset;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "v_atcpu_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("atcpu", "CPU Utilisation");
  if (!static_cnt)
  { i_printf (1, "v_atcpu_enable failed to create container"); v_atcpu_disable (self); return -1; }
  static_cnt->mainform_func = v_atcpu_cntform;
  static_cnt->sumform_func = v_atcpu_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_atcpu_enable failed to register container"); v_atcpu_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_atcpu_enable failed to load and apply container refresh config"); v_atcpu_disable (self); return -1; }

  /* 
   * Trigger sets
   */

  tset = i_triggerset_create ("onemin_avg", "One Minute Average", "onemin_avg");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("fivemin_avg", "Five Minute Average", "fivemin_avg");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 70, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);  
    
  /*
   * Master object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_atcpu_enable failed to create item_list"); v_atcpu_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_atcpu_enable failed to create master object"); v_atcpu_disable (self); return -1; }
  static_obj->mainform_func = v_atcpu_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create cpu item */
  cpu = v_atcpu_item_create ();
  if (!cpu)
  { i_printf (1, "v_atcpu_enable failed to create static_cpu struct"); v_atcpu_disable (self); return -1; }
  cpu->obj = static_obj;
  static_obj->itemptr = cpu;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, cpu);
  if (num != 0)
  { i_printf (1, "v_atcpu_enable failed to enqueue item into static_cnt->item_list"); v_atcpu_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */
  

  cpu->onesec_avg= l_snmp_metric_create (self, static_obj, "onesec_avg", "One Second Average", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.3.5", "0", RECMETHOD_RRD, 0);
  cpu->onesec_avg->unit_str = strdup ("%");
  cpu->onesec_avg->record_defaultflag = 1;

  cpu->tensec_avg= l_snmp_metric_create (self, static_obj, "tensec_avg", "Ten Second Average", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.3.4", "0", RECMETHOD_RRD, 0);
  cpu->tensec_avg->unit_str = strdup ("%");
  cpu->tensec_avg->record_defaultflag = 1;

  cpu->onemin_avg= l_snmp_metric_create (self, static_obj, "onemin_avg", "One Minute Average", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.3.3", "0", RECMETHOD_RRD, 0);
  cpu->onemin_avg->unit_str = strdup ("%");
  cpu->onemin_avg->record_defaultflag = 1;

  cpu->fivemin_avg= l_snmp_metric_create (self, static_obj, "fivemin_avg", "Five Minute Average", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.3.7", "0", RECMETHOD_RRD, 0);
  cpu->fivemin_avg->unit_str = strdup ("%");
  cpu->fivemin_avg->record_defaultflag = 1;

  cpu->fivemin_max = l_snmp_metric_create (self, static_obj, "fivemin_max", "Five Minute Maximum", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.3.3.6", "0", RECMETHOD_RRD, 0);
  cpu->fivemin_max->unit_str = strdup ("%");
  cpu->fivemin_max->record_defaultflag = 1;

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);
  
  return 0;
}

int v_atcpu_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_atcpu_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    v_atcpu_item *item = static_obj->itemptr;
    if (item) { v_atcpu_item_free (item); }
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

