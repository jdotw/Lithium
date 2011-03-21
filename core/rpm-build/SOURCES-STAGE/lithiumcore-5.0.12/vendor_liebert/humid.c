#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/snmp.h"
#include "device/record.h"

#include "humid.h"

/* Humidity */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int v_humid_enabled ()
{ return static_enabled; }

i_container* v_humid_cnt ()
{ return static_cnt; }

i_object* v_humid_obj ()
{ return static_obj; }

/* Enable / Disable */

int v_humid_enable (i_resource *self)
{
  int num;
  v_humid_item *humid;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "v_humid_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("humid", "Humidity");
  if (!static_cnt)
  { i_printf (1, "v_humid_enable failed to create container"); v_humid_disable (self); return -1; }
  static_cnt->mainform_func = v_humid_cntform;
  static_cnt->sumform_func = v_humid_sumform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_humid_enable failed to register container"); v_humid_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_humid_enable failed to load and apply container refresh config"); v_humid_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_humid_enable failed to create item_list"); v_humid_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_humid_enable failed to create master object"); v_humid_disable (self); return -1; }
  static_obj->mainform_func = v_humid_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create humid item */
  humid = v_humid_item_create ();
  if (!humid)
  { i_printf (1, "v_humid_enable failed to create static_humid struct"); v_humid_disable (self); return -1; }
  humid->obj = static_obj;
  static_obj->itemptr = humid;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, humid);
  if (num != 0)
  { i_printf (1, "v_humid_enable failed to enqueue item into static_cnt->item_list"); v_humid_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  humid->control = l_snmp_metric_create (self, static_obj, "control", "Control Measurement", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.2.2.3.1.3.1", NULL, RECMETHOD_RRD, 0);
  humid->control->unit_str = strdup ("%");
  humid->control->record_defaultflag = 1;
  humid->supply = l_snmp_metric_create (self, static_obj, "supply", "Supply Air", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.2.2.3.1.3.2", NULL, RECMETHOD_RRD, 0);
  humid->supply->unit_str = strdup ("%");
  humid->supply->record_defaultflag = 1;
  humid->rtn= l_snmp_metric_create (self, static_obj, "return", "Return Air", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.2.2.3.1.3.3", NULL, RECMETHOD_RRD, 0);    
  humid->rtn->unit_str = strdup ("%");
  humid->rtn->record_defaultflag = 1;

  humid->control_high_thresh = l_snmp_metric_create (self, static_obj, "control_high_thresh", "Control High Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.2.2.3.1.4.1", NULL, RECMETHOD_NONE, 0);
  humid->control_high_thresh->unit_str = strdup ("%");
  humid->return_high_thresh = l_snmp_metric_create (self, static_obj, "return_high_thresh", "Return High Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.2.2.3.1.4.3", NULL, RECMETHOD_NONE, 0);
  humid->return_high_thresh->unit_str = strdup ("%");

  humid->control_low_thresh = l_snmp_metric_create (self, static_obj, "control_low_thresh", "Control Low Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.2.2.3.1.5.1", NULL, RECMETHOD_NONE, 0);
  humid->control_low_thresh->unit_str = strdup ("%");
  humid->return_low_thresh = l_snmp_metric_create (self, static_obj, "return_low_thresh", "Return Low Threshold", METRIC_INTEGER, ".iso.org.dod.internet.private.enterprises.476.1.42.3.4.2.2.3.1.5.3", NULL, RECMETHOD_NONE, 0);
  humid->return_low_thresh->unit_str = strdup ("%");

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  /*
   * Combined/Custom Graphs
   */
  humid->humid_cg = i_metric_cgraph_create (static_obj, "humid_cg", "% (Rel)");
  asprintf (&humid->humid_cg->title_str, "Humidity");
  asprintf (&humid->humid_cg->render_str, " \"LINE1:met_%s_control_min#000E73:Min.\" \"LINE1:met_%s_control_avg#001EFF:Avg.\" \"LINE1:met_%s_control_max#00B4FF: Max. Control Humidity\" \"GPRINT:met_%s_control_min:MIN:   Min %%.2lf %s\" \"GPRINT:met_%s_control_avg:AVERAGE: Avg %%.2lf %s\" \"GPRINT:met_%s_control_max:MAX: Max %%.2lf %s\\n\" \"LINE1:met_%s_supply_min#006B00:Min.\" \"LINE1:met_%s_supply_avg#009B00:Avg.\" \"LINE1:met_%s_supply_max#00ED00: Max. Supply Humidity\" \"GPRINT:met_%s_supply_min:MIN:  Min %%.2lf %s/s\" \"GPRINT:met_%s_supply_avg:AVERAGE: Avg %%.2lf %s\" \"GPRINT:met_%s_supply_max:MAX: Max %%.2lf %s\\n\" \"LINE1:met_%s_return_min#6B0000:Min.\" \"LINE1:met_%s_return_avg#9B0000:Avg.\" \"LINE1:met_%s_return_max#ED0000:Max. Return Humidity\" \"GPRINT:met_%s_return_min:MIN:   Min %%.2lf %s\" \"GPRINT:met_%s_return_avg:AVERAGE:Avg. %%.2lf %s\" \"GPRINT:met_%s_return_max:MAX: Max. %%.2lf %s\\n\"",
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, humid->control->unit_str, static_obj->name_str, humid->control->unit_str, static_obj->name_str, humid->control->unit_str,
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, humid->supply->unit_str, static_obj->name_str, humid->supply->unit_str, static_obj->name_str, humid->supply->unit_str,
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, humid->rtn->unit_str, static_obj->name_str, humid->rtn->unit_str, static_obj->name_str, humid->rtn->unit_str);
  i_list_enqueue (humid->humid_cg->met_list, humid->control);
  i_list_enqueue (humid->humid_cg->met_list, humid->supply);
  i_list_enqueue (humid->humid_cg->met_list, humid->rtn);

  return 0;
}

int v_humid_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_humid_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    v_humid_item *item = static_obj->itemptr;
    if (item) { v_humid_item_free (item); }
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

