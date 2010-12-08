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
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/str.h>
#include <lithium/record.h>

#include "xsan.h"
#include "data.h"
#include "xsaninfo.h"

/* Xsan Info  */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

i_container* v_xsaninfo_cnt ()
{ return static_cnt; }

i_object* v_xsaninfo_obj ()
{ return static_obj; }

/* Enable */

int v_xsaninfo_enable (i_resource *self)
{
  int num;
  v_xsaninfo_item *xsaninfo;
  static i_entity_refresh_config defrefconfig;
  
  /* Create/Config Container */
  static_cnt = i_container_create ("xsaninfo", "Xsan");
  if (!static_cnt)
  { i_printf (1, "v_xsaninfo_enable failed to create container"); return -1; }
//  static_cnt->mainform_func = v_xsaninfo_cntform;
//  static_cnt->sumform_func = v_xsaninfo_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_xsaninfo_enable failed to register container"); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_xsaninfo_enable failed to load and apply container refresh config"); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_xsaninfo_enable failed to create item_list"); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* 
   * Triggers
   */

  i_triggerset *tset = i_triggerset_create ("cpu_usage", "CPU Usage", "cpu_usage");
  tset->default_duration = 600;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 85, NULL, 95, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_GT, 95, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Master Object
   */

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_xsaninfo_enable failed to create master object"); return -1; }
//  static_obj->mainform_func = v_xsaninfo_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create xsaninfo item */
  xsaninfo = v_xsaninfo_item_create ();
  if (!xsaninfo)
  { i_printf (1, "v_xsaninfo_enable failed to create static_xsaninfo struct"); return -1; }
  xsaninfo->obj = static_obj;
  static_obj->itemptr = xsaninfo;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, xsaninfo);
  if (num != 0)
  { i_printf (1, "v_xsaninfo_enable failed to enqueue item into static_cnt->item_list"); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Standard refresh config */
  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  
  xsaninfo->cpu_usage = i_metric_create ("cpu_usage", "CPU Usage", METRIC_FLOAT);
  xsaninfo->cpu_usage->unit_str = strdup ("%");
  xsaninfo->cpu_usage->min_val = i_metric_value_create ();
  xsaninfo->cpu_usage->min_val->flt = 0;
  xsaninfo->cpu_usage->max_val = i_metric_value_create ();
  xsaninfo->cpu_usage->max_val->flt = 100;
  xsaninfo->cpu_usage->record_method = RECMETHOD_RRD;
  xsaninfo->cpu_usage->record_defaultflag = 1;
  i_entity_register (self, ENTITY(static_obj), ENTITY(xsaninfo->cpu_usage));
  i_entity_refresh_config_apply (self, ENTITY(xsaninfo->cpu_usage), &refconfig);

  xsaninfo->network_tput = i_metric_create ("network_tput", "Network Throughput", METRIC_FLOAT);
  xsaninfo->network_tput->unit_str = strdup ("B/sec");
  xsaninfo->network_tput->valstr_func = i_string_rate_metric;
  xsaninfo->network_tput->record_method = RECMETHOD_RRD;
  xsaninfo->network_tput->record_defaultflag = 1;
  i_entity_register (self, ENTITY(static_obj), ENTITY(xsaninfo->network_tput));
  i_entity_refresh_config_apply (self, ENTITY(xsaninfo->network_tput), &refconfig);

  xsaninfo->fibre_tput = i_metric_create ("fibre_tput", "Fibre Throughput", METRIC_FLOAT);
  xsaninfo->fibre_tput->unit_str = strdup ("B/sec");
  xsaninfo->fibre_tput->valstr_func = i_string_rate_metric;
  xsaninfo->fibre_tput->record_method = RECMETHOD_RRD;
  xsaninfo->fibre_tput->record_defaultflag = 1;
  i_entity_register (self, ENTITY(static_obj), ENTITY(xsaninfo->fibre_tput));
  i_entity_refresh_config_apply (self, ENTITY(xsaninfo->fibre_tput), &refconfig);
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  /*
   * Create Data Metrics 
   */

  v_data_item *dataitem = v_data_static_item();

  /* Standard refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Propertes */
  dataitem->xsan_localproperties = i_metric_create ("xsan_localproperties", "Xsan Local Properties", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->xsan_cpu_history, 0, "Invalid");
  i_metric_enumstr_add (dataitem->xsan_cpu_history, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan_localproperties));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan_localproperties), &defrefconfig);
  dataitem->xsan_localproperties->refresh_func = v_data_xsan_localproperties_refresh;

  return 0;
}

int v_xsaninfo_enable_history (i_resource *self)
{
  v_data_item *dataitem = v_data_static_item();
  
  /* Standard refresh config */
  i_entity_refresh_config defrefconfig;
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  i_printf (1, "v_xsaninfo_enable_history enabling history version %i", v_xsan_version());

  if (v_xsan_version() == 1)
  {    
    /* CPU History */
    dataitem->xsan_cpu_history = i_metric_create ("xsan_cpu_history", "Xsan CPU History", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->xsan_cpu_history, 0, "Invalid");
    i_metric_enumstr_add (dataitem->xsan_cpu_history, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan_cpu_history));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan_cpu_history), &defrefconfig);
    dataitem->xsan_cpu_history->refresh_func = v_data_xsan_cpu_history_refresh;
  
    /* Network History */
    dataitem->xsan_network_history = i_metric_create ("xsan_network_history", "Xsan Network History", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->xsan_network_history, 0, "Invalid");
    i_metric_enumstr_add (dataitem->xsan_network_history, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan_network_history));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan_network_history), &defrefconfig);
    dataitem->xsan_network_history->refresh_func = v_data_xsan_network_history_refresh;
  
    /* Fibre History */
    dataitem->xsan_fibre_history = i_metric_create ("xsan_fibre_history", "Xsan Fibre History", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->xsan_fibre_history, 0, "Invalid");
    i_metric_enumstr_add (dataitem->xsan_fibre_history, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan_fibre_history));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan_fibre_history), &defrefconfig);
    dataitem->xsan_fibre_history->refresh_func = v_data_xsan_fibre_history_refresh;
  }
  else if (v_xsan_version() == 2)
  {
    /* Network History */
    dataitem->xsan2_history = i_metric_create ("xsan2_history", "Xsan History", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->xsan2_history, 0, "Invalid");
    i_metric_enumstr_add (dataitem->xsan2_history, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan2_history));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan2_history), &defrefconfig);
    dataitem->xsan2_history->refresh_func = v_data_xsan2_history_refresh;
  }
  
  return 0;
}


