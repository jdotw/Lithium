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
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "temp.h"

/* Temperature */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int v_temp_enabled ()
{ return static_enabled; }

i_container* v_temp_cnt ()
{ return static_cnt; }

i_object* v_temp_obj ()
{ return static_obj; }

/* Enable / Disable */

int v_temp_enable (i_resource *self)
{
  int num;
  v_temp_item *temp;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "v_temp_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("temp", "Temperature");
  if (!static_cnt)
  { i_printf (1, "v_temp_enable failed to create container"); v_temp_disable (self); return -1; }
  static_cnt->mainform_func = v_temp_cntform;
  static_cnt->sumform_func = v_temp_sumform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_temp_enable failed to register container"); v_temp_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_temp_enable failed to load and apply container refresh config"); v_temp_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_temp_enable failed to create item_list"); v_temp_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_temp_enable failed to create master object"); v_temp_disable (self); return -1; }
  static_obj->mainform_func = v_temp_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create temp item */
  temp = v_temp_item_create ();
  if (!temp)
  { i_printf (1, "v_temp_enable failed to create static_temp struct"); v_temp_disable (self); return -1; }
  temp->obj = static_obj;
  static_obj->itemptr = temp;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, temp);
  if (num != 0)
  { i_printf (1, "v_temp_enable failed to enqueue item into static_cnt->item_list"); v_temp_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  if (self->hierarchy->dev->profile_str && strstr(self->hierarchy->dev->profile_str, "fahr"))
  {
    /* Use fahrenheit */
    temp->control = l_snmp_metric_create (self, static_obj, "control", "Control Measurement", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.2.3.1.3.1", NULL, RECMETHOD_RRD, 0);
    temp->control->unit_str = strdup ("deg.F");
    temp->control->record_defaultflag = 1;
    temp->supply = l_snmp_metric_create (self, static_obj, "supply", "Supply Air", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.2.3.1.3.2", NULL, RECMETHOD_RRD, 0);
    temp->supply->unit_str = strdup ("deg.F");
    temp->supply->record_defaultflag = 1;
    temp->rtn= l_snmp_metric_create (self, static_obj, "return", "Return Air", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.2.3.1.3.3", NULL, RECMETHOD_RRD, 0);    
    temp->rtn->unit_str = strdup ("deg.F");
    temp->rtn->record_defaultflag = 1;

    temp->control_high_thresh = l_snmp_metric_create (self, static_obj, "control_high_thresh", "Control High Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.2.3.1.4.1", NULL, RECMETHOD_NONE, 0);
    temp->control_high_thresh->unit_str = strdup ("deg.F");
    temp->return_high_thresh = l_snmp_metric_create (self, static_obj, "return_high_thresh", "Return High Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.2.3.1.4.3", NULL, RECMETHOD_NONE, 0);
    temp->return_high_thresh->unit_str = strdup ("deg.F");

    temp->control_low_thresh = l_snmp_metric_create (self, static_obj, "control_low_thresh", "Control Low Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.2.3.1.5.1", NULL, RECMETHOD_NONE, 0);
    temp->control_low_thresh->unit_str = strdup ("deg.F");
    temp->return_low_thresh = l_snmp_metric_create (self, static_obj, "return_low_thresh", "Return Low Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.2.3.1.5.3", NULL, RECMETHOD_NONE, 0);
    temp->return_low_thresh->unit_str = strdup ("deg.F");

    i_triggerset *tset = i_triggerset_create ("control", "Control Measurement", "control");
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 75, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "critical", "Crtical", VALTYPE_INTEGER, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
    i_triggerset_assign (self, static_cnt, tset);
  }
  else
  {
    /* Default to celcius */
    temp->control = l_snmp_metric_create (self, static_obj, "control", "Control Measurement", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.3.3.1.3.1", NULL, RECMETHOD_RRD, 0);
    temp->control->unit_str = strdup ("deg.C");
    temp->control->record_defaultflag = 1;
    temp->supply = l_snmp_metric_create (self, static_obj, "supply", "Supply Air", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.3.3.1.3.2", NULL, RECMETHOD_RRD, 0);
    temp->supply->unit_str = strdup ("deg.C");
    temp->supply->record_defaultflag = 1;
    temp->rtn= l_snmp_metric_create (self, static_obj, "return", "Return Air", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.3.3.1.3.3", NULL, RECMETHOD_RRD, 0);    
    temp->rtn->unit_str = strdup ("deg.C");
    temp->rtn->record_defaultflag = 1;

    temp->control_high_thresh = l_snmp_metric_create (self, static_obj, "control_high_thresh", "Control High Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.3.3.1.4.1", NULL, RECMETHOD_NONE, 0);
    temp->control_high_thresh->unit_str = strdup ("deg.C");
    temp->return_high_thresh = l_snmp_metric_create (self, static_obj, "return_high_thresh", "Return High Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.3.3.1.4.3", NULL, RECMETHOD_NONE, 0);
    temp->return_high_thresh->unit_str = strdup ("deg.C");

    temp->control_low_thresh = l_snmp_metric_create (self, static_obj, "control_low_thresh", "Control Low Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.3.3.1.5.1", NULL, RECMETHOD_NONE, 0);
    temp->control_low_thresh->unit_str = strdup ("deg.C");
    temp->return_low_thresh = l_snmp_metric_create (self, static_obj, "return_low_thresh", "Return Low Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.1.3.3.1.5.3", NULL, RECMETHOD_NONE, 0);
    temp->return_low_thresh->unit_str = strdup ("deg.C");
    
    i_triggerset *tset = i_triggerset_create ("control", "Control Measurement", "control");
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 25, NULL, 29, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "critical", "Crtical", VALTYPE_INTEGER, TRGTYPE_GT, 29, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
    i_triggerset_assign (self, static_cnt, tset);
  }

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  /*
   * Combined/Custom Graphs
   */
  temp->temp_cg = i_metric_cgraph_create (static_obj, "temp_cg", "deg");
  asprintf (&temp->temp_cg->title_str, "Temperature");
  asprintf (&temp->temp_cg->render_str, " \"LINE1:met_%s_control_min#000E73:Min.\" \"LINE1:met_%s_control_avg#001EFF:Avg.\" \"LINE1:met_%s_control_max#00B4FF: Max. Control Temp\" \"GPRINT:met_%s_control_min:MIN:   Min %%.2lf %s\" \"GPRINT:met_%s_control_avg:AVERAGE: Avg %%.2lf %s\" \"GPRINT:met_%s_control_max:MAX: Max %%.2lf %s\\n\" \"LINE1:met_%s_supply_min#006B00:Min.\" \"LINE1:met_%s_supply_avg#009B00:Avg.\" \"LINE1:met_%s_supply_max#00ED00: Max. Supply Temp\" \"GPRINT:met_%s_supply_min:MIN:  Min %%.2lf %s/s\" \"GPRINT:met_%s_supply_avg:AVERAGE: Avg %%.2lf %s\" \"GPRINT:met_%s_supply_max:MAX: Max %%.2lf %s\\n\" \"LINE1:met_%s_return_min#6B0000:Min.\" \"LINE1:met_%s_return_avg#9B0000:Avg.\" \"LINE1:met_%s_return_max#ED0000:Max. Return Temp\" \"GPRINT:met_%s_return_min:MIN:   Min %%.2lf %s\" \"GPRINT:met_%s_return_avg:AVERAGE:Avg. %%.2lf %s\" \"GPRINT:met_%s_return_max:MAX: Max. %%.2lf %s\\n\"",
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, temp->control->unit_str, static_obj->name_str, temp->control->unit_str, static_obj->name_str, temp->control->unit_str,
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, temp->supply->unit_str, static_obj->name_str, temp->supply->unit_str, static_obj->name_str, temp->supply->unit_str,
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, temp->rtn->unit_str, static_obj->name_str, temp->rtn->unit_str, static_obj->name_str, temp->rtn->unit_str);
  i_list_enqueue (temp->temp_cg->met_list, temp->control);
  i_list_enqueue (temp->temp_cg->met_list, temp->supply);
  i_list_enqueue (temp->temp_cg->met_list, temp->rtn);
  
  return 0;
}

int v_temp_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_temp_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    v_temp_item *item = static_obj->itemptr;
    if (item) { v_temp_item_free (item); }
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

