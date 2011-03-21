#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/str.h>

#include "record.h"
#include "navtree.h"
#include "mformprio.h"
#include "nwtput.h"

/* Network Throughput (nwtput) Sub-System
 *
 * This sub-system provides aggregate bit per
 * second and packet per second throughput
 * statistics. An API is provided for other
 * sub-systems to specify input/output
 * bit per second and packet per second metrics
 * 
 */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */
static i_object *static_bpsobj = NULL;                /* Aggregate bits/sec object */
static i_object *static_ppsobj = NULL;                /* Aggregate pkts/sec object */

/* Variable Fetching */

i_container* l_nwtput_cnt ()
{ return static_cnt; }

i_object* l_nwtput_bpsobj ()
{ return static_bpsobj; }

i_object* l_nwtput_ppsobj ()
{ return static_ppsobj; }

/* Sub-System Enable / Disable */

int l_nwtput_enable (i_resource *self)
{
  int num;
  l_nwtput_item *bpsitem;
  l_nwtput_item *ppsitem;
  static i_entity_refresh_config refconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_nwtput_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("nwtput", "Network Throughput");
  if (!static_cnt)
  { i_printf (1, "l_nwtput_enable failed to create container"); l_nwtput_disable (self); return -1; }
  static_cnt->mainform_func = l_nwtput_cntform;
  static_cnt->sumform_func = l_nwtput_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_nwtput_enable failed to register container"); l_nwtput_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &refconfig);
  if (num != 0)
  { i_printf (1, "l_nwtput_enable failed to load and apply container refresh config"); l_nwtput_disable (self); return -1; }

  /*
   * Item List etc 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_nwtput_enable failed to create item_list"); l_nwtput_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /*
   * Master (Aggregate) Objects (bps/pps)
   */

  /* Create bps object */
  static_bpsobj = i_object_create ("bps", "Bits per Second");
  if (!static_bpsobj)
  { i_printf (1, "l_nwtput_enable failed to create bps object"); l_nwtput_disable (self); return -1; }
  static_bpsobj->cnt = static_cnt;
  static_bpsobj->mainform_func = l_nwtput_objform;
  static_bpsobj->histform_func = l_nwtput_objform_hist;
  static_bpsobj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create pps object */
  static_ppsobj = i_object_create ("pps", "Packets per Second");
  if (!static_ppsobj)
  { i_printf (1, "l_nwtput_enable failed to create pps object"); l_nwtput_disable (self); return -1; }
  static_ppsobj->cnt = static_cnt;
  static_ppsobj->mainform_func = l_nwtput_objform;
  static_ppsobj->histform_func = l_nwtput_objform_hist;
  static_ppsobj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create bps item */
  bpsitem = l_nwtput_item_create ();
  bpsitem->obj = static_bpsobj;
  static_bpsobj->itemptr = bpsitem;

  /* Create pps item */
  ppsitem = l_nwtput_item_create ();
  ppsitem->obj = static_ppsobj;
  static_ppsobj->itemptr = ppsitem;

  /* Register objects */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_bpsobj));
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_ppsobj));

  /* Load/Apply refresh config */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;        
  
  /* Load/Apply refresh configuration for objects */
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_bpsobj), &refconfig);
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_ppsobj), &refconfig);

  /* Metrics */
  bpsitem->input = i_metric_acsum_create (self, static_bpsobj, "input", "Input Bits per Second", METRIC_FLOAT, RECMETHOD_RRD, NULL, NULL, ACSUM_REFMETHOD_PARENT);
  bpsitem->input->valstr_func = i_string_rate_metric;
  bpsitem->input->unit_str = strdup ("bits/s");
  bpsitem->input->record_defaultflag = 1;
  bpsitem->input->record_priority = 2;
    
  bpsitem->output = i_metric_acsum_create (self, static_bpsobj, "output", "Output Bits per Second", METRIC_FLOAT, RECMETHOD_RRD, NULL, NULL, ACSUM_REFMETHOD_PARENT);
  bpsitem->output->valstr_func = i_string_rate_metric;
  bpsitem->output->unit_str = strdup ("bits/s");
  bpsitem->output->record_defaultflag = 1;
  bpsitem->output->record_priority = 2;
  
  ppsitem->input = i_metric_acsum_create (self, static_ppsobj, "input", "Input Packets per Second", METRIC_FLOAT, RECMETHOD_RRD, NULL, NULL, ACSUM_REFMETHOD_PARENT);
  ppsitem->input->valstr_func = i_string_rate_metric;
  ppsitem->input->unit_str = strdup ("pkts/s");
  ppsitem->input->record_defaultflag = 1;
  ppsitem->input->record_priority = 2;
  
  ppsitem->output = i_metric_acsum_create (self, static_ppsobj, "output", "Output Packets per Second", METRIC_FLOAT, RECMETHOD_RRD, NULL, NULL, ACSUM_REFMETHOD_PARENT);
  ppsitem->output->valstr_func = i_string_rate_metric;
  ppsitem->output->unit_str = strdup ("pkts/s");
  ppsitem->output->record_defaultflag = 1;
  ppsitem->output->record_priority = 2;

  /*
   * Combined/Custom Graphs
   */
  bpsitem->tput_cg = i_metric_cgraph_create (static_bpsobj, "tput_cg", "Bits/sec");
  asprintf (&bpsitem->tput_cg->title_str, "Aggregate '%s' Throughput", static_bpsobj->desc_str);
  asprintf (&bpsitem->tput_cg->render_str, "\"CDEF:kbps_in_min=met_%s_input_min,1000,/\" \"CDEF:kbps_in_avg=met_%s_input_avg,1000,/\" \"CDEF:kbps_in_max=met_%s_input_max,1000,/\" \"CDEF:kbps_out_min=met_%s_output_min,1000,/\" \"CDEF:kbps_out_avg=met_%s_output_avg,1000,/\" \"CDEF:kbps_out_max=met_%s_output_max,1000,/\" \"LINE1:met_%s_input_min#000E73:Min.\" \"LINE1:met_%s_input_avg#001EFF:Avg.\" \"LINE1:met_%s_input_max#00B4FF: Max. Input\" \"GPRINT:kbps_in_min:MIN:   Min %%.2lf Kbits/s\" \"GPRINT:kbps_in_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_in_max:MAX: Max %%.2lf Kbits/s\\n\" \"LINE1:met_%s_output_min#006B00:Min.\" \"LINE1:met_%s_output_avg#009B00:Avg.\" \"LINE1:met_%s_output_max#00ED00: Max. Output\" \"GPRINT:kbps_out_min:MIN:  Min %%.2lf Kbits/s\" \"GPRINT:kbps_out_avg:AVERAGE: Avg %%.2lf Kbits/s\" \"GPRINT:kbps_out_max:MAX: Max %%.2lf Kbits/s\\n\"",
    static_bpsobj->name_str, static_bpsobj->name_str, static_bpsobj->name_str,
    static_bpsobj->name_str, static_bpsobj->name_str, static_bpsobj->name_str,
    static_bpsobj->name_str, static_bpsobj->name_str, static_bpsobj->name_str,
    static_bpsobj->name_str, static_bpsobj->name_str, static_bpsobj->name_str);
  i_list_enqueue (bpsitem->tput_cg->met_list, bpsitem->input);
  i_list_enqueue (bpsitem->tput_cg->met_list, bpsitem->output);

  ppsitem->tput_cg = i_metric_cgraph_create (static_ppsobj, "tput_cg", "Pkts/sec");
  asprintf (&ppsitem->tput_cg->title_str, "Aggregate '%s' Throughput", static_ppsobj->desc_str);
  asprintf (&ppsitem->tput_cg->render_str, "\"CDEF:kpps_in_min=met_%s_input_min,1000,/\" \"CDEF:kpps_in_avg=met_%s_input_avg,1000,/\" \"CDEF:kpps_in_max=met_%s_input_max,1000,/\" \"CDEF:kpps_out_min=met_%s_output_min,1000,/\" \"CDEF:kpps_out_avg=met_%s_output_avg,1000,/\" \"CDEF:kpps_out_max=met_%s_output_max,1000,/\" \"LINE1:met_%s_input_min#000E73:Min.\" \"LINE1:met_%s_input_avg#001EFF:Avg.\" \"LINE1:met_%s_input_max#00B4FF: Max. Input\" \"GPRINT:kpps_in_min:MIN:   Min %%.2lf Kpkts/s\" \"GPRINT:kpps_in_avg:AVERAGE: Avg %%.2lf Kpkts/s\" \"GPRINT:kpps_in_max:MAX: Max %%.2lf Kpkts/s\\n\" \"LINE1:met_%s_output_min#006B00:Min.\" \"LINE1:met_%s_output_avg#009B00:Avg.\" \"LINE1:met_%s_output_max#00ED00: Max. Output\" \"GPRINT:kpps_out_min:MIN:  Min %%.2lf Kpkts/s\" \"GPRINT:kpps_out_avg:AVERAGE: Avg %%.2lf Kpkts/s\" \"GPRINT:kpps_out_max:MAX: Max %%.2lf Kpkts/s\\n\"",
    static_ppsobj->name_str, static_ppsobj->name_str, static_ppsobj->name_str,
    static_ppsobj->name_str, static_ppsobj->name_str, static_ppsobj->name_str,
    static_ppsobj->name_str, static_ppsobj->name_str, static_ppsobj->name_str,
    static_ppsobj->name_str, static_ppsobj->name_str, static_ppsobj->name_str);
  i_list_enqueue (ppsitem->tput_cg->met_list, ppsitem->input);
  i_list_enqueue (ppsitem->tput_cg->met_list, ppsitem->output);


  /* Enqueue the items */
  i_list_enqueue (static_cnt->item_list, bpsitem);
  i_list_enqueue (static_cnt->item_list, ppsitem);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_bpsobj);
  i_triggerset_evalapprules_allsets (self, static_ppsobj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_bpsobj);
  l_record_eval_recrules_obj (self, static_ppsobj);
  
  return 0;  
}

int l_nwtput_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_nwtput_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Remove bps object */
  if (static_bpsobj)
  { 
    if (static_bpsobj->itemptr) { l_nwtput_item_free ((l_nwtput_item *) static_bpsobj->itemptr); static_bpsobj->itemptr = NULL; }
    i_entity_deregister (self, ENTITY(static_bpsobj));
    i_entity_free (ENTITY(static_bpsobj));
    static_bpsobj = NULL; 
  }

  /* Remove pps object */
  if (static_ppsobj)
  { 
    if (static_ppsobj->itemptr) { l_nwtput_item_free ((l_nwtput_item *) static_ppsobj->itemptr); static_ppsobj->itemptr = NULL; }
    i_entity_deregister (self, ENTITY(static_ppsobj));
    i_entity_free (ENTITY(static_ppsobj));
    static_ppsobj = NULL; 
  }

  /* Deregister/Free container */
  if (static_cnt) 
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}


