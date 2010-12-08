#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"

#include "device/snmp.h"

#include "battery.h"

/* 
 * battery Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_battery_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_battery_item *battery;

  /* Object setup */
  char *str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Battery %s", str);
  free (str);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_battery_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create battery item struct */
  battery = v_battery_item_create ();
  battery->obj = obj;
  obj->itemptr = battery;
  battery->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Present */
  battery->present = l_snmp_metric_create (self, obj, "present", "Present", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.5.1.1.2", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (battery->present, 0, "No");
  i_metric_enumstr_add (battery->present, 1, "Yes");

  /* Charge */
  battery->charge = l_snmp_metric_create (self, obj, "charge", "Charge", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.5.1.1.3", index_oidstr, RECMETHOD_RRD, 0);
  battery->charge->unit_str = strdup ("%");
  battery->charge->record_defaultflag = 1;

  /* Voltage */
  battery->voltage = l_snmp_metric_create (self, obj, "voltage", "Voltage", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.5.1.1.4", index_oidstr, RECMETHOD_RRD, 0);
  battery->voltage->unit_str = strdup ("mV");
  battery->voltage->record_defaultflag = 1;

  /* Status */
  battery->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.5.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (battery->status, 0, "Unknown");
  i_metric_enumstr_add (battery->status, 16, "Fully Discharged");
  i_metric_enumstr_add (battery->status, 32, "Fully Charged");
  i_metric_enumstr_add (battery->status, 64, "Discharging");
  i_metric_enumstr_add (battery->status, 128, "Initialized");

  /* Enqueue the battery item */
  num = i_list_enqueue (cnt->item_list, battery);
  if (num != 0)
  { i_printf (1, "v_battery_objfact_fab failed to enqueue battery for object %s", obj->name_str); v_battery_item_free (battery); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_battery_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_battery_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the battery and remove
   * it from the item_list
   */

  int num;
  v_battery_item *battery = obj->itemptr;

  if (!battery) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, battery);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
