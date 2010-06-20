#include <stdlib.h>

#include <induction.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <induction/hierarchy.h>
#include <induction/list.h>

#include <lithium/snmp.h>

#include "input.h"

/* 
 * Printer input - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_input_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_input_item *input;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_input_objform;
  obj->histform_func = v_input_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_input_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create input item struct */
  input = v_input_item_create ();
  if (!input)
  { i_printf (1, "v_input_objfact_fab failed to create input item for object %s", obj->name_str); return -1; }
  input->obj = obj;
  obj->itemptr = input;
  input->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Temp */
  input->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_INTEGER, ".1.3.6.1.2.1.43.8.2.1.2.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (input->type, 1, "Other");
  i_metric_enumstr_add (input->type, 2, "Unknown");
  i_metric_enumstr_add (input->type, 3, "Sheet Feed Auto Removable Tray");
  i_metric_enumstr_add (input->type, 4, "Sheet Feed Auto Non-Removable Tray");
  i_metric_enumstr_add (input->type, 5, "Manual Feed Tray");
  i_metric_enumstr_add (input->type, 6, "Continuous Roll");
  i_metric_enumstr_add (input->type, 7, "Continuour Fan Fold");

  /* Units */
  input->units = l_snmp_metric_create (self, obj, "units", "Units", METRIC_INTEGER, ".1.3.6.1.2.1.43.8.2.1.8.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (input->units, 3, "/1000\"");
  i_metric_enumstr_add (input->units, 4, "um");
  i_metric_enumstr_add (input->units, 8, "sheets");
  i_metric_enumstr_add (input->units, 16, "ft");
  i_metric_enumstr_add (input->units, 17, "m");

  /* Max Capacity */
  input->max_capacity = l_snmp_metric_create (self, obj, "max_capacity", "Maximum Capacity", METRIC_INTEGER, ".1.3.6.1.2.1.43.8.2.1.9.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (input->max_capacity, -1, "No restriction");
  i_metric_enumstr_add (input->max_capacity, -2, "Unknown");

  /* Current Level */
  input->current_level = l_snmp_metric_create (self, obj, "current_level", "Current Level", METRIC_INTEGER, ".1.3.6.1.2.1.43.8.2.1.10.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (input->current_level, -1, "No restriction");
  i_metric_enumstr_add (input->current_level, -2, "Unknown");
  i_metric_enumstr_add (input->current_level, -3, "At least 1");
  i_entity_refreshcb_add (ENTITY(input->current_level), v_input_level_refcb, input);

  /* Level Percent */
  input->remaining_pc = i_metric_acpcent_create (self, obj, "remaining_pc", "Supply Remaining", RECMETHOD_RRD, input->current_level, input->max_capacity, 0);
  input->remaining_pc->record_defaultflag = 1;
  input->remaining_pc->unit_str = strdup ("%");

  /* Media */
  input->media = l_snmp_metric_create (self, obj, "media", "Media", METRIC_STRING, ".1.3.6.1.2.1.43.8.2.1.12.1", index_oidstr, RECMETHOD_NONE, 0);
  
  /* Enqueue the input item */
  num = i_list_enqueue (cnt->item_list, input);
  if (num != 0)
  { i_printf (1, "v_input_objfact_fab failed to enqueue input for object %s", obj->name_str); v_input_item_free (input); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_input_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_input_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the input and remove
   * it from the item_list
   */

  int num;
  v_input_item *input = obj->itemptr;

  if (!input) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, input);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
