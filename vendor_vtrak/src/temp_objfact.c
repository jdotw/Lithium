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

#include "temp.h"

/* Object Factory Fabrication */

int v_temp_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_temp_item *temp;

  /* Object Configuration */
  char *desc_index_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Temperature Sensor %s", desc_index_str);
  free (desc_index_str);

  /* Adjust index_oidstr */
  asprintf (&index_oidstr, "%i.%i.%s",
    (int) pdu->variables->name[pdu->variables->name_length-3],
    (int) pdu->variables->name[pdu->variables->name_length-2],
    index_oidstr);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_temp_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create temp item struct */
  temp = v_temp_item_create ();
  if (!temp)
  { i_printf (1, "v_temp_objfact_fab failed to create temp item for object %s", obj->name_str); return -1; }
  temp->obj = obj;
  obj->itemptr = temp;
  temp->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  temp->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational Status", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.13.1.3", index_oidstr, RECMETHOD_NONE, 0);
  temp->temperature = l_snmp_metric_create (self, obj, "temperature", "Temperature", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.13.1.2", index_oidstr, RECMETHOD_RRD, 0);
  temp->temperature->record_defaultflag = 1;

  /* Enqueue the temp item */
  num = i_list_enqueue (cnt->item_list, temp);
  if (num != 0)
  { i_printf (1, "v_temp_objfact_fab failed to enqueue temp for object %s", obj->name_str); v_temp_item_free (temp); return -1; }

  /* Free our custom indexoid */
  free (index_oidstr);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_temp_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_temp_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the temp and remove
   * it from the item_list
   */

  int num;
  v_temp_item *temp = obj->itemptr;

  if (!temp) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, temp);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
