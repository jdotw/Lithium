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

#include "system.h"

/* 
 * system Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_system_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_system_item *system;

  /* Object setup */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_system_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create system item struct */
  system = v_system_item_create ();
  system->obj = obj;
  obj->itemptr = system;
  system->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  system->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.10.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (system->status, 0, "Single-Boot");
  i_metric_enumstr_add (system->status, 1, "Dual-Boot");
  i_metric_enumstr_add (system->status, 2, "Normal");
  i_metric_enumstr_add (system->status, 3, "Degraded");
  i_metric_enumstr_add (system->status, 4, "Failover");
  i_metric_enumstr_add (system->status, 5, "Failback");
  i_metric_enumstr_add (system->status, 6, "Failed");

  /* Enclosure */
  system->enclosure = l_snmp_metric_create (self, obj, "enclosure", "Enclosure", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.10.1.1.4", index_oidstr, RECMETHOD_NONE, 0);

  /* Serial */
  system->serial = l_snmp_metric_create (self, obj, "serial", "Serial", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.10.1.1.5", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the system item */
  num = i_list_enqueue (cnt->item_list, system);
  if (num != 0)
  { i_printf (1, "v_system_objfact_fab failed to enqueue system for object %s", obj->name_str); v_system_item_free (system); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_system_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_system_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the system and remove
   * it from the item_list
   */

  int num;
  v_system_item *system = obj->itemptr;

  if (!system) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, system);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
