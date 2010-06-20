#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "output.h"

/* UPS Output Sub-System */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_output_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_output_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  static_cnt = i_container_create ("apcoutput", "Output");
  if (!static_cnt)
  { i_printf (1, "v_output_enable failed to create container"); v_output_disable (self); return -1; }
  //static_cnt->mainform_func = v_output_cntform;
  //static_cnt->sumform_func = v_output_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_output_enable failed to register container"); v_output_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_output_enable failed to load and apply container refresh config"); v_output_disable (self); return -1; }

  /*
   * Trigger Sets 
   */

  /* Load */

  tset = i_triggerset_create ("status", "Status", "status");
  i_triggerset_addtrg (self, tset, "onbattery", "On Battery", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "onsmartboost", "On Smart Boost", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "off", "Off", VALTYPE_INTEGER, TRGTYPE_EQUAL, 7, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "rebooting", "Rebooting", VALTYPE_INTEGER, TRGTYPE_EQUAL, 8, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "switchbypass", "Switched Bypass", VALTYPE_INTEGER, TRGTYPE_EQUAL, 9, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "hairwarefailure", "Hardware Failure Bypass", VALTYPE_INTEGER, TRGTYPE_EQUAL, 10, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "onsmarttrim", "On Smart Trim", VALTYPE_INTEGER, TRGTYPE_EQUAL, 12, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("voltage", "Voltage", "voltage");
  i_triggerset_addtrg (self, tset, "critical", "Critical", VALTYPE_GAUGE, TRGTYPE_LT, 1, NULL, 85, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  tset = i_triggerset_create ("load", "Load", "load");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_GAUGE, TRGTYPE_RANGE, 60, NULL, 75, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_GAUGE, TRGTYPE_RANGE, 75, NULL, 90, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_GAUGE, TRGTYPE_GT, 90, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_output_enable failed to create item_list"); v_output_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_output_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_output_enable failed to create master object"); v_output_disable (self); return -1; }
  //static_obj->mainform_func = v_output_objform;

  /* Create output item */
  v_output_item *output = v_output_item_create ();
  if (!output)
  { i_printf (1, "v_output_enable failed to create output item struct"); v_output_disable (self); return -1; }
  output->obj = static_obj;
  static_obj->itemptr = output;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, output);
  if (num != 0)
  { i_printf (1, "v_output_enable failed to enqueue item into static_cnt->item_list"); v_output_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Basic */
  output->status = l_snmp_metric_create (self, static_obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.318.1.1.1.4.1.1", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (output->status, 1, "Unknown");
  i_metric_enumstr_add (output->status, 2, "On Line");
  i_metric_enumstr_add (output->status, 3, "On Battery");
  i_metric_enumstr_add (output->status, 4, "On Smart Boost");
  i_metric_enumstr_add (output->status, 5, "Timed Sleeping");
  i_metric_enumstr_add (output->status, 6, "Software Bypass");
  i_metric_enumstr_add (output->status, 7, "Off");
  i_metric_enumstr_add (output->status, 8, "Rebooting");
  i_metric_enumstr_add (output->status, 9, "Switched Bypass");
  i_metric_enumstr_add (output->status, 10, "Hardware Failure Bypass");
  i_metric_enumstr_add (output->status, 11, "Sleeping Until Power Return");
  i_metric_enumstr_add (output->status, 12, "On Smart Trim");
  output->phase = l_snmp_metric_create (self, static_obj, "phase", "Phase", METRIC_INTEGER, ".1.3.6.1.4.1.318.1.1.1.4.1.2", "0", RECMETHOD_NONE, 0);

  /* Advanced */
  output->voltage = l_snmp_metric_create (self, static_obj, "voltage", "Voltage", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.4.2.1", "0", RECMETHOD_RRD, 0);
  output->voltage->unit_str = strdup ("V");
  output->voltage->record_defaultflag = 1;
  output->frequency = l_snmp_metric_create (self, static_obj, "frequency", "Frequency", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.4.2.2", "0", RECMETHOD_RRD, 0);
  output->frequency->unit_str = strdup ("Hz");
  output->frequency->record_defaultflag = 1;
  output->load = l_snmp_metric_create (self, static_obj, "load", "Load", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.4.2.3", "0", RECMETHOD_RRD, 0);
  output->load->unit_str = strdup ("%");
  output->load->record_defaultflag = 1;
  output->current = l_snmp_metric_create (self, static_obj, "current", "Current", METRIC_GAUGE, ".1.3.6.1.4.1.318.1.1.1.4.2.4", "0", RECMETHOD_RRD, 0);
  output->current->unit_str = strdup ("Amp");
  output->current->record_defaultflag = 1;
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_output_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

