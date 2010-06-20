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

#include "supplies.h"

/* 
 * Printer Supplies - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_supplies_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_supplies_item *supplies;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
//  obj->mainform_func = v_supplies_objform;
//  obj->histform_func = v_supplies_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_supplies_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create supplies item struct */
  supplies = v_supplies_item_create ();
  if (!supplies)
  { i_printf (1, "v_supplies_objfact_fab failed to create supplies item for object %s", obj->name_str); return -1; }
  supplies->obj = obj;
  obj->itemptr = supplies;
  supplies->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Class */
  supplies->class = l_snmp_metric_create (self, obj, "class", "Class", METRIC_INTEGER, ".1.3.6.1.2.1.43.11.1.1.4.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (supplies->class, 1, "Other");
  i_metric_enumstr_add (supplies->class, 3, "Supply That Is Consumed");
  i_metric_enumstr_add (supplies->class, 4, "Receptacle That Is Filled");
  i_entity_refreshcb_add (ENTITY(supplies->class), v_supplies_class_refcb, supplies);

  /* Type */
  supplies->type = l_snmp_metric_create (self, obj, "type", "Supply Type", METRIC_INTEGER, ".1.3.6.1.2.1.43.11.1.1.5.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (supplies->type, 1, "Other");
  i_metric_enumstr_add (supplies->type, 2, "Unknown");
  i_metric_enumstr_add (supplies->type, 3, "Toner");
  i_metric_enumstr_add (supplies->type, 4, "Waste Toner");
  i_metric_enumstr_add (supplies->type, 5, "Ink");
  i_metric_enumstr_add (supplies->type, 6, "Ink Cartridge");
  i_metric_enumstr_add (supplies->type, 7, "Ink Ribbon");
  i_metric_enumstr_add (supplies->type, 8, "Waste Ink");
  i_metric_enumstr_add (supplies->type, 9, "Photo Conductor");
  i_metric_enumstr_add (supplies->type, 10, "Developer");
  i_metric_enumstr_add (supplies->type, 11, "Fuser Oil");
  i_metric_enumstr_add (supplies->type, 12, "Solid Wax");
  i_metric_enumstr_add (supplies->type, 13, "Ribbon Wax");
  i_metric_enumstr_add (supplies->type, 14, "Waste Wax");
  i_metric_enumstr_add (supplies->type, 15, "Fuser");
  i_metric_enumstr_add (supplies->type, 16, "Corona Wire");
  i_metric_enumstr_add (supplies->type, 17, "Fuser Oil Wick");
  i_metric_enumstr_add (supplies->type, 18, "Cleaner Unit");
  i_metric_enumstr_add (supplies->type, 19, "Fuser Cleaning Pad");
  i_metric_enumstr_add (supplies->type, 20, "Transfer Unit");
  i_metric_enumstr_add (supplies->type, 21, "Toner Cartridge");
  i_metric_enumstr_add (supplies->type, 22, "Fuser Oiler");
  i_metric_enumstr_add (supplies->type, 23, "Water");
  i_metric_enumstr_add (supplies->type, 24, "Waste Water");
  i_metric_enumstr_add (supplies->type, 25, "Glue Water Additive");
  i_metric_enumstr_add (supplies->type, 26, "Waste Paper");
  i_metric_enumstr_add (supplies->type, 27, "Binding Supply");
  i_metric_enumstr_add (supplies->type, 28, "Banding Supply");
  i_metric_enumstr_add (supplies->type, 29, "Stitching Wire");
  i_metric_enumstr_add (supplies->type, 30, "Shrink Wrap");
  i_metric_enumstr_add (supplies->type, 31, "Paper Wrap");
  i_metric_enumstr_add (supplies->type, 32, "Staples");
  i_metric_enumstr_add (supplies->type, 33, "Inserts");
  i_metric_enumstr_add (supplies->type, 34, "Covers");

  /* Units */
  supplies->units = l_snmp_metric_create (self, obj, "units", "Units", METRIC_INTEGER, ".1.3.6.1.2.1.43.11.1.1.7.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (supplies->units, 3, "/1000\"");
  i_metric_enumstr_add (supplies->units, 4, "um");
  i_metric_enumstr_add (supplies->units, 7, "impressions");
  i_metric_enumstr_add (supplies->units, 8, "sheets");
  i_metric_enumstr_add (supplies->units, 11, "hours");
  i_metric_enumstr_add (supplies->units, 12, "/1000oz");
  i_metric_enumstr_add (supplies->units, 13, "/10g");
  i_metric_enumstr_add (supplies->units, 14, "/100flOz");
  i_metric_enumstr_add (supplies->units, 15, "/10mm");
  i_metric_enumstr_add (supplies->units, 16, "ft");
  i_metric_enumstr_add (supplies->units, 17, "m");
  i_metric_enumstr_add (supplies->units, 18, "items");

  /* Max Capacity */
  supplies->max_capacity = l_snmp_metric_create (self, obj, "max_capacity", "Maximum Capacity", METRIC_INTEGER, ".1.3.6.1.2.1.43.11.1.1.8.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (supplies->max_capacity, -1, "No restruction");
  i_metric_enumstr_add (supplies->max_capacity, -2, "Unknown");

  /* Current Level */
  supplies->current_level = l_snmp_metric_create (self, obj, "current_level", "Current Level", METRIC_INTEGER, ".1.3.6.1.2.1.43.11.1.1.9.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (supplies->current_level, -1, "No restruction");
  i_metric_enumstr_add (supplies->current_level, -2, "Unknown");
  i_metric_enumstr_add (supplies->current_level, -3, "Some available");
  i_entity_refreshcb_add (ENTITY(supplies->current_level), v_supplies_level_refcb, supplies);

  /* Enqueue the supplies item */
  num = i_list_enqueue (cnt->item_list, supplies);
  if (num != 0)
  { i_printf (1, "v_supplies_objfact_fab failed to enqueue supplies for object %s", obj->name_str); v_supplies_item_free (supplies); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_supplies_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_supplies_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the supplies and remove
   * it from the item_list
   */

  int num;
  v_supplies_item *supplies = obj->itemptr;

  if (!supplies) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, supplies);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
