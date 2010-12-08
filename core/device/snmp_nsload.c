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
#include "snmp_nsload.h"

/* load - Load Average Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */

/* Variable Fetching */

i_container* l_snmp_nsload_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int l_snmp_nsload_enable (i_resource *self)
{
  int num;
  i_triggerset *tset;
  l_snmp_nsload_item *load;
  i_object *obj;
  static i_entity_refresh_config defrefconfig;
  static i_entity_refresh_config refconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_nsload_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("load", "Load Averages");
  if (!static_cnt)
  { i_printf (1, "l_snmp_nsload_enable failed to create container"); l_snmp_nsload_disable (self); return -1; }
  static_cnt->mainform_func = l_snmp_nsload_cntform;
  static_cnt->sumform_func = l_snmp_nsload_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_nsload_enable failed to register container"); l_snmp_nsload_disable (self); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_nsload_enable failed to load and apply container refresh config"); l_snmp_nsload_disable (self); return -1; }

  /* 
   * Trigger sets 
   */

  tset = i_triggerset_create ("five_min", "Five Minute Load Average", "five_min");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 5, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  tset = i_triggerset_create ("fifteen_min", "Fifteen Minute Load Average", "fifteen_min");
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 10, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  /* 
   * Items and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_nsload_enable failed to create item_list"); l_snmp_nsload_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create object */
  obj = i_object_create ("master", "Master");
  if (!obj)
  { i_printf (1, "l_snmp_nsload_enable failed to create object"); l_snmp_nsload_disable (self); return -1; }
  obj->cnt = static_cnt;
  obj->mainform_func = l_snmp_nsload_objform;
  obj->histform_func = l_snmp_nsload_objform_hist;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create item */
  load = l_snmp_nsload_item_create ();
  if (!load)
  { i_printf (1, "l_snmp_nsload_enable failed to create item"); l_snmp_nsload_disable (self); return -1; }
  load->obj = obj;
  obj->itemptr = load;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Set refresh params */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* One Minute */
  load->one_min = i_metric_create ("one_min", "One Minute", METRIC_FLOAT);
  load->one_min->record_defaultflag = 1;
  load->one_min->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(load->one_min));
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_apply (self, ENTITY(load->one_min), &refconfig);
                
  load->one_min_raw = l_snmp_metric_create (self, obj, "one_min_raw", "One Minute Raw", METRIC_INTEGER, ".1.3.6.1.4.1.2021.10.1.5.1", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(load->one_min_raw), l_snmp_nsload_refcb, NULL);

  /* Five Minute */
  load->five_min = i_metric_create ("five_min", "Five Minute", METRIC_FLOAT);
  load->five_min->record_defaultflag = 1;
  load->five_min->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(load->five_min));
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_apply (self, ENTITY(load->five_min), &refconfig);

  load->five_min_raw = l_snmp_metric_create (self, obj, "five_min_raw", "Five Minute Raw", METRIC_INTEGER, ".1.3.6.1.4.1.2021.10.1.5.2", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  load->five_min_raw->record_defaultflag = 1;
  i_entity_refreshcb_add (ENTITY(load->five_min_raw), l_snmp_nsload_refcb, NULL);

  /* Fifteen Minute */
  load->fifteen_min = i_metric_create ("fifteen_min", "Fifteen Minute", METRIC_FLOAT);
  load->fifteen_min->record_defaultflag = 1;
  load->fifteen_min->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(load->fifteen_min));
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_apply (self, ENTITY(load->fifteen_min), &refconfig);

  load->fifteen_min_raw = l_snmp_metric_create (self, obj, "fifteen_min_raw", "Fifteen Minute Raw", METRIC_INTEGER, ".1.3.6.1.4.1.2021.10.1.5.3", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  load->fifteen_min_raw->record_defaultflag = 1;
  i_entity_refreshcb_add (ENTITY(load->fifteen_min_raw), l_snmp_nsload_refcb, NULL);

  /* 
   * Misc 
   */
  
  load->io_sent = l_snmp_metric_create (self, obj, "io_sent", "Block IO Sent", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.5.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  load->io_sent->unit_str = strdup ("blk/sec");
  load->io_sent->valstr_func = i_string_volume_metric;

  load->io_recv = l_snmp_metric_create (self, obj, "io_recv", "Block IO Received", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.6.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  load->io_recv->unit_str = strdup ("blk/sec");
  load->io_recv->valstr_func = i_string_volume_metric;

  load->interrupts = l_snmp_metric_create (self, obj, "interrupts", "Interrupts", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.7.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  load->interrupts->unit_str = strdup ("int/sec");
  load->interrupts->valstr_func = i_string_volume_metric;

  load->cswitches = l_snmp_metric_create (self, obj, "cswitches", "Context Switches", METRIC_GAUGE, ".1.3.6.1.4.1.2021.11.8.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  load->cswitches->unit_str = strdup ("sw/sec");
  load->cswitches->valstr_func = i_string_volume_metric;

  /* Load/Apply refresh configuration for the object */
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &defrefconfig);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);
  
  /* Apply all triggersets */
  num = i_triggerset_evalapprules_allsets (self, obj);
  if (num != 0)
  { i_printf (1, "l_snmp_nsload_enable warning, failed to apply all triggersets for master object"); }

  /*
   * Custom/Combined Graphs
   */
  load->load_cg = i_metric_cgraph_create (obj, "load_cg", "%");
  load->load_cg->title_str = strdup ("Load Averages");
  asprintf (&load->load_cg->render_str, "\"LINE1:met_%s_one_min_min#000E73:Min.\" \"LINE1:met_%s_one_min_avg#001EFF:Avg.\" \"LINE1:met_%s_one_min_max#00B4FF:Max. One Minute Load\" \"GPRINT:met_%s_one_min_min:MIN:        Min %%.2lf %%%%\" \"GPRINT:met_%s_one_min_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_one_min_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_five_min_min#006B00:Min.\" \"LINE1:met_%s_five_min_avg#009B00:Avg.\" \"LINE1:met_%s_five_min_max#00ED00:Max. Five Minute Load\" \"GPRINT:met_%s_five_min_min:MIN:       Min %%.2lf %%%%\" \"GPRINT:met_%s_five_min_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_five_min_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_fifteen_min_min#6B0000:Min.\" \"LINE1:met_%s_fifteen_min_avg#9B0000:Avg.\" \"LINE1:met_%s_fifteen_min_max#ED0000:Max. Fifteen Minute Load\" \"GPRINT:met_%s_fifteen_min_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_fifteen_min_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_fifteen_min_max:MAX: Max. %%.2lf %%%%\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (load->load_cg->met_list, load->one_min);
  i_list_enqueue (load->load_cg->met_list, load->five_min);
  i_list_enqueue (load->load_cg->met_list, load->fifteen_min);

  
  /* Enqueue the load item */
  i_list_enqueue (static_cnt->item_list, load);

  return 0;
}

int l_snmp_nsload_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_nsload_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

