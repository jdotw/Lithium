#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"
#include "device/record.h"

#include "input.h"

/* UPS Input Sub-System */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_input_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_input_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  static_cnt = i_container_create ("mgeinput", "Input");
  if (!static_cnt)
  { i_printf (1, "v_input_enable failed to create container"); v_input_disable (self); return -1; }
  //static_cnt->mainform_func = v_input_cntform;
  //static_cnt->sumform_func = v_input_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_input_enable failed to register container"); v_input_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_input_enable failed to load and apply container refresh config"); v_input_disable (self); return -1; }

  /*
   * Trigger Sets 
   */

  /* Voltage Trigger set */
  tset = i_triggerset_create ("linefail_cause", "Utility Power Failure", "linefail_cause");
  i_triggerset_addtrg (self, tset, "outoftolvolt", "Voltage out of Range", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "outoftolfreq", "Frequency out of Range", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "utilityoff", "No Voltage", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_input_enable failed to create item_list"); v_input_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_input_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_input_enable failed to create master object"); v_input_disable (self); return -1; }
  //static_obj->mainform_func = v_input_objform;

  /* Create input item */
  v_input_item *input = v_input_item_create ();
  if (!input)
  { i_printf (1, "v_input_enable failed to create input item struct"); v_input_disable (self); return -1; }
  input->obj = static_obj;
  static_obj->itemptr = input;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, input);
  if (num != 0)
  { i_printf (1, "v_input_enable failed to enqueue item into static_cnt->item_list"); v_input_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Basic */
  input->phase = l_snmp_metric_create (self, static_obj, "phase", "Phase", METRIC_INTEGER, ".1.3.6.1.4.1.705.1.6.1", "0", RECMETHOD_NONE, 0);

  input->status = l_snmp_metric_create (self, static_obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.705.1.6.3", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (input->status, 2, "OK");
  i_metric_enumstr_add (input->status, 1, "FAILED");

  input->linefail_cause = l_snmp_metric_create (self, static_obj, "linefail_cause", "Failure Cause", METRIC_INTEGER, ".1.3.6.1.4.1.705.1.6.4", "0", RECMETHOD_NONE, 0);
  i_metric_enumstr_add (input->linefail_cause, 1, "None");
  i_metric_enumstr_add (input->linefail_cause, 2, "Voltage out of range");
  i_metric_enumstr_add (input->linefail_cause, 3, "Frequency out of range");
  i_metric_enumstr_add (input->linefail_cause, 4, "Power Failure");

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_input_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

