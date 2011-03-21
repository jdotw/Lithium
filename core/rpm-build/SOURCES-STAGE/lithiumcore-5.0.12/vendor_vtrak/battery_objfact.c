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

/* Object Factory Fabrication */

int v_battery_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_battery_item *battery;

  /* Object Configuration */
  char *desc_index_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Battery %s", desc_index_str);
  free (desc_index_str);

  /* Adjust index_oidstr */
  asprintf (&index_oidstr, "%i.%i.%s",
    (int) pdu->variables->name[pdu->variables->name_length-3],
    (int) pdu->variables->name[pdu->variables->name_length-2],
    index_oidstr);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_battery_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create battery item struct */
  battery = v_battery_item_create ();
  if (!battery)
  { i_printf (1, "v_battery_objfact_fab failed to create battery item for object %s", obj->name_str); return -1; }
  battery->obj = obj;
  obj->itemptr = battery;
  battery->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  battery->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational Status", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.15.1.14", index_oidstr, RECMETHOD_NONE, 0);

  /* Info */
  battery->manufactured = l_snmp_metric_create (self, obj, "manufactured", "Manufactured", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.15.1.3", index_oidstr, RECMETHOD_NONE, 0);
  battery->chemistry = l_snmp_metric_create (self, obj, "chemistry", "Chemistry", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.15.1.6", index_oidstr, RECMETHOD_NONE, 0);
  battery->celltype = l_snmp_metric_create (self, obj, "celltype", "Cell Type", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.15.1.15", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (battery->celltype, 1, "One Cell");
  i_metric_enumstr_add (battery->celltype, 2, "Two Cell");
  i_metric_enumstr_add (battery->celltype, 4, "Four Cell");
  i_metric_enumstr_add (battery->celltype, 255, "Unknown");

  /* Temps */
  battery->temp = l_snmp_metric_create (self, obj, "temp", "Temperature", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.7", index_oidstr, RECMETHOD_RRD, 0);
  battery->temp->record_defaultflag = 1;
  battery->temp_charge_max = l_snmp_metric_create (self, obj, "temp_charge_max", "Max. Charge Temp", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.8", index_oidstr, RECMETHOD_NONE, 0);
  battery->temp_discharge_max = l_snmp_metric_create (self, obj, "temp_discharge_max", "Max. Discharge Temp", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.9", index_oidstr, RECMETHOD_NONE, 0);

  /* Cycles */
  battery->cyclecount = l_snmp_metric_create (self, obj, "cyclecount", "Cycle Count", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.10", index_oidstr, RECMETHOD_NONE, 0);
  battery->remaining = l_snmp_metric_create (self, obj, "remaining", "Remaining Capacity", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.11", index_oidstr, RECMETHOD_RRD, 0);
  battery->remaining->unit_str = strdup ("%");
  battery->remaining->record_defaultflag = 1;
  battery->holdtime = l_snmp_metric_create (self, obj, "holdtime", "Hold Time", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.16", index_oidstr, RECMETHOD_NONE, 0);
  battery->remaining->unit_str = strdup ("hr");
  battery->remaining->record_defaultflag = 1;

  /* Voltage/Current */
  battery->voltage = l_snmp_metric_create (self, obj, "voltage", "Voltage", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.12", index_oidstr, RECMETHOD_RRD, 0);
  battery->voltage->unit_str = strdup ("mV");
  battery->voltage->record_defaultflag = 1;
  battery->current = l_snmp_metric_create (self, obj, "current", "Current", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.15.1.13", index_oidstr, RECMETHOD_RRD, 0);
  battery->current->unit_str = strdup ("mA");
  battery->current->record_defaultflag = 1;

  /* Enqueue the battery item */
  num = i_list_enqueue (cnt->item_list, battery);
  if (num != 0)
  { i_printf (1, "v_battery_objfact_fab failed to enqueue battery for object %s", obj->name_str); v_battery_item_free (battery); return -1; }

  /* Free our custom indexoid */
  free (index_oidstr);

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
