#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include <lithium/snmp.h>

#include "array.h"

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_array_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_array_item *array;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_array_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create array item struct */
  array = v_array_item_create ();
  if (!array)
  { i_printf (1, "v_array_objfact_fab failed to create array item for object %s", obj->name_str); return -1; }
  array->obj = obj;
  obj->itemptr = array;
  array->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Array Info */
  array->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.4.1.18", index_oidstr, RECMETHOD_NONE, 0);

  /* State */
  array->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational State", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.4.1.3", index_oidstr, RECMETHOD_NONE, 0);
  array->condition = l_snmp_metric_create (self, obj, "condition", "Condition", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.4.1.4", index_oidstr, RECMETHOD_NONE, 0);
  array->operation = l_snmp_metric_create (self, obj, "operation", "Operation", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.4.1.5", index_oidstr, RECMETHOD_NONE, 0);

  /* Configuration */
  array->mediapatrol = l_snmp_metric_create (self, obj, "mediapatrol", "Media Patrol", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.4.1.10", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (array->mediapatrol, 1, "Enabled");
  i_metric_enumstr_add (array->mediapatrol, 0, "Disabled");
  array->pdmenabled = l_snmp_metric_create (self, obj, "pdmenabled", "PDM", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.4.1.11", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (array->pdmenabled, 1, "Enabled");
  i_metric_enumstr_add (array->pdmenabled, 0, "Disabled");
  array->phydrvcount = l_snmp_metric_create (self, obj, "phydrvcount", "Physical Drive Count", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.2.4.1.12", index_oidstr, RECMETHOD_NONE, 0);
  array->logdrvcount = l_snmp_metric_create (self, obj, "logdrvcount", "Logical Drive Count", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.2.4.1.13", index_oidstr, RECMETHOD_NONE, 0);
  array->dedspares = l_snmp_metric_create (self, obj, "dedspares", "Dedicated Spares", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.2.4.1.14", index_oidstr, RECMETHOD_NONE, 0);

  /* Drives */
  array->phydrvs = l_snmp_metric_create (self, obj, "phydrvs", "Physical Drives", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.4.1.15", index_oidstr, RECMETHOD_NONE, 0);
  array->logdrvs = l_snmp_metric_create (self, obj, "logdrvs", "Logical Drives", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.4.1.16", index_oidstr, RECMETHOD_NONE, 0);
  array->sparedrvs = l_snmp_metric_create (self, obj, "sparedrvs", "Spare Drives", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.4.1.17", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the array item */
  num = i_list_enqueue (cnt->item_list, array);
  if (num != 0)
  { i_printf (1, "v_array_objfact_fab failed to enqueue array for object %s", obj->name_str); v_array_item_free (array); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_array_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_array_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the array and remove
   * it from the item_list
   */

  int num;
  v_array_item *array = obj->itemptr;

  if (!array) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, array);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
