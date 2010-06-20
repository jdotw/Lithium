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

#include "voltage.h"

/* Object Factory Fabrication */

int v_voltage_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_voltage_item *voltage;

  /* Object Configuration */
  char *desc_index_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Voltage Sensor %s", desc_index_str);
  free (desc_index_str);

  /* Adjust index_oidstr */
  asprintf (&index_oidstr, "%i.%i.%s",
    (int) pdu->variables->name[pdu->variables->name_length-3],
    (int) pdu->variables->name[pdu->variables->name_length-2],
    index_oidstr);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_voltage_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create voltage item struct */
  voltage = v_voltage_item_create ();
  if (!voltage)
  { i_printf (1, "v_voltage_objfact_fab failed to create voltage item for object %s", obj->name_str); return -1; }
  voltage->obj = obj;
  obj->itemptr = voltage;
  voltage->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  voltage->voltage = l_snmp_metric_create (self, obj, "voltage", "Voltage", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.14.1.2", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the voltage item */
  num = i_list_enqueue (cnt->item_list, voltage);
  if (num != 0)
  { i_printf (1, "v_voltage_objfact_fab failed to enqueue voltage for object %s", obj->name_str); v_voltage_item_free (voltage); return -1; }

  /* Free our custom indexoid */
  free (index_oidstr);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_voltage_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_voltage_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the voltage and remove
   * it from the item_list
   */

  int num;
  v_voltage_item *voltage = obj->itemptr;

  if (!voltage) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, voltage);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
