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

#include "state.h"

/* Unit State */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Pointer retrieval */

int v_state_enabled ()
{ return static_enabled; }

i_container* v_state_cnt ()
{ return static_cnt; }

i_object* v_state_obj ()
{ return static_obj; }

/* Enable / Disable */

int v_state_enable (i_resource *self)
{
  int num;
  v_state_item *state;
  static i_entity_refresh_config defrefconfig;
  
  if (static_enabled == 1)
  { i_printf (1, "v_state_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("state", "State");
  if (!static_cnt)
  { i_printf (1, "v_state_enable failed to create container"); v_state_disable (self); return -1; }
  static_cnt->mainform_func = v_state_cntform;
  static_cnt->sumform_func = v_state_sumform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register container */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_state_enable failed to register container"); v_state_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_state_enable failed to load and apply container refresh config"); v_state_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_state_enable failed to create item_list"); v_state_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_state_enable failed to create master object"); v_state_disable (self); return -1; }
  static_obj->mainform_func = v_state_objform;
  static_obj->navtree_expand = NAVTREE_EXP_ALWAYS;

  /* Create state item */
  state = v_state_item_create ();
  if (!state)
  { i_printf (1, "v_state_enable failed to create static_state struct"); v_state_disable (self); return -1; }
  state->obj = static_obj;
  static_obj->itemptr = state;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, state);
  if (num != 0)
  { i_printf (1, "v_state_enable failed to enqueue item into static_cnt->item_list"); v_state_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  state->system = l_snmp_metric_create (self, static_obj, "system", "System State", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.1.0", NULL, RECMETHOD_RRD, 0);
  state->system->record_defaultflag = 1;
  i_metric_enumstr_add (state->system, 1, "On");
  i_metric_enumstr_add (state->system, 2, "Off");

  state->humidifier = l_snmp_metric_create (self, static_obj, "humidifier", "Humidifier", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.4.0", NULL, RECMETHOD_RRD, 0);
  state->humidifier->record_defaultflag = 1;
  i_metric_enumstr_add (state->humidifier, 1, "On");
  i_metric_enumstr_add (state->humidifier, 2, "Off");

  state->dehumidifier = l_snmp_metric_create (self, static_obj, "dehumidifier", "Dehumidifier", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.5.0", NULL, RECMETHOD_RRD, 0);
  state->dehumidifier->record_defaultflag = 1;
  i_metric_enumstr_add (state->dehumidifier, 1, "On");
  i_metric_enumstr_add (state->dehumidifier, 2, "Off");

  state->systemfan = l_snmp_metric_create (self, static_obj, "systemfan", "System Fan", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.7.0", NULL, RECMETHOD_RRD, 0);
  state->systemfan->record_defaultflag = 1;
  i_metric_enumstr_add (state->systemfan, 1, "On");
  i_metric_enumstr_add (state->systemfan, 2, "Off");

  state->systemalarm = l_snmp_metric_create (self, static_obj, "systemalarm", "System Alarm", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.8.0", NULL, RECMETHOD_RRD, 0);
  state->systemalarm->record_defaultflag = 1;
  i_metric_enumstr_add (state->systemalarm, 1, "On");
  i_metric_enumstr_add (state->systemalarm, 2, "Off");

  state->cooling_pc = l_snmp_metric_create (self, static_obj, "cooling_pc", "Cooling Capacity Used", METRIC_GAUGE, "enterprises.476.1.42.3.4.3.9.0", NULL, RECMETHOD_RRD, 0);
  state->cooling_pc->record_defaultflag = 1;

  state->heating_pc = l_snmp_metric_create (self, static_obj, "heating_pc", "Heating Capacity Used", METRIC_GAUGE, "enterprises.476.1.42.3.4.3.10.0", NULL, RECMETHOD_RRD, 0);
  state->heating_pc->record_defaultflag = 1;

  state->audible_alarm = l_snmp_metric_create (self, static_obj, "audible_alarm", "Audible Alarm", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.11.0", NULL, RECMETHOD_RRD, 0);
  state->audible_alarm->record_defaultflag = 1;
  i_metric_enumstr_add (state->audible_alarm, 1, "On");
  i_metric_enumstr_add (state->audible_alarm, 2, "Off");

  state->cooling_unit_1 = l_snmp_metric_create (self, static_obj, "cooling_unit_1", "Cooling Unit 1", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.12.2.0", NULL, RECMETHOD_RRD, 0);
  state->cooling_unit_1->record_defaultflag = 1;
  i_metric_enumstr_add (state->cooling_unit_1, 1, "On");
  i_metric_enumstr_add (state->cooling_unit_1, 2, "Off");

  state->cooling_unit_2 = l_snmp_metric_create (self, static_obj, "cooling_unit_2", "Cooling Unit 2", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.12.3.0", NULL, RECMETHOD_RRD, 0);
  state->cooling_unit_2->record_defaultflag = 1;
  i_metric_enumstr_add (state->cooling_unit_2, 1, "On");
  i_metric_enumstr_add (state->cooling_unit_2, 2, "Off");

  state->heating_unit_1 = l_snmp_metric_create (self, static_obj, "heating_unit_1", "Heating Unit 1", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.13.2.0", NULL, RECMETHOD_RRD, 0);
  state->heating_unit_1->record_defaultflag = 1;
  i_metric_enumstr_add (state->heating_unit_1, 1, "On");
  i_metric_enumstr_add (state->heating_unit_1, 2, "Off");

  state->heating_unit_2 = l_snmp_metric_create (self, static_obj, "heating_unit_2", "Heating Unit 2", METRIC_INTEGER, "enterprises.476.1.42.3.4.3.13.3.0", NULL, RECMETHOD_RRD, 0);
  state->heating_unit_2->record_defaultflag = 1;
  i_metric_enumstr_add (state->heating_unit_2, 1, "On");
  i_metric_enumstr_add (state->heating_unit_2, 2, "Off");

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  /*
   * Combined/Custom Graphs
   */
  state->state_cg = i_metric_cgraph_create (static_obj, "state_cg", "%");
  asprintf (&state->state_cg->title_str, "Output Capacity");
  asprintf (&state->state_cg->render_str, " \"LINE1:met_%s_cooling_min#000E73:Min.\" \"LINE1:met_%s_cooling_avg#001EFF:Avg.\" \"LINE1:met_%s_cooling_max#00B4FF: Max. Cooling Output\" \"GPRINT:met_%s_cooling_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_cooling_avg:AVERAGE: Avg %%.2lf %%%%\" \"GPRINT:met_%s_cooling_max:MAX: Max %%.2lf %%%%\\n\" \"LINE1:met_%s_heating_min#6B0000:Min.\" \"LINE1:met_%s_heating_avg#9B0000:Avg.\" \"LINE1:met_%s_heating_max#ED0000:Max. Heating Output\" \"GPRINT:met_%s_heating_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_heating_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_heating_max:MAX: Max. %%.2lf %%%%\\n\"",
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, static_obj->name_str, static_obj->name_str,
    static_obj->name_str, static_obj->name_str, static_obj->name_str);
  i_list_enqueue (state->state_cg->met_list, state->cooling_pc);
  i_list_enqueue (state->state_cg->met_list, state->heating_pc);


  
  return 0;
}

int v_state_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "v_state_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_obj)
  { 
    v_state_item *item = static_obj->itemptr;
    if (item) { v_state_item_free (item); }
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

