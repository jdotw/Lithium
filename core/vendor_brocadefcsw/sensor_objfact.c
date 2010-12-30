#include <stdlib.h>

#include "induction.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/hashtable.h"
#include "induction/list.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/str.h"
#include "induction/hierarchy.h"

#include "device/snmp.h"
#include "device/record.h"

#include "sensor.h"
#include "slot.h"

/* 
 * Brocade Fibre Channel Ports (Not Slot-specific) - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_sensor_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu(pdu);

  /* Load/Apply Refresh config */
  int num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_sensor_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create sensor item struct */
  v_sensor_item *sensor = v_sensor_item_create ();
  if (!sensor)
  { i_printf (1, "v_sensor_objfact_fab failed to create sensor item for object %s", obj->name_str); return -1; }
  sensor->obj = obj;
  obj->itemptr = sensor;

  /* 
   * Sensor Metrics
   */

  /* Type */ 
  sensor->type = l_snmp_metric_create (self, obj, "type", "Type", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.1.22.1.2", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (sensor->type, 1, "Temperature");
  i_metric_enumstr_add (sensor->type, 2, "Fan");
  i_metric_enumstr_add (sensor->type, 3, "Power Supply");

  /* Status */ 
  sensor->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.1588.2.1.1.1.1.22.1.3", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (sensor->status, 1, "Unknown");
  i_metric_enumstr_add (sensor->status, 2, "Faulty");
  i_metric_enumstr_add (sensor->status, 3, "Below Minimum");
  i_metric_enumstr_add (sensor->status, 4, "Normal");
  i_metric_enumstr_add (sensor->status, 5, "Above Maximum");
  i_metric_enumstr_add (sensor->status, 6, "Absent");
  
  /* Value */ 
  sensor->value = l_snmp_metric_create (self, obj, "value", "Value", METRIC_GAUGE, ".1.3.6.1.4.1.1588.2.1.1.1.1.22.1.4", index_oidstr, RECMETHOD_RRD, 0);
  sensor->value->record_defaultflag = 1;
  i_entity_refreshcb_add (ENTITY(sensor->type), v_sensor_type_refcb, sensor->value);
  i_entity_refreshcb_add (ENTITY(sensor->status), v_sensor_status_refcb, sensor->value);

  /* Info (String) */ 
  sensor->info = l_snmp_metric_create (self, obj, "info", "Info", METRIC_STRING, ".1.3.6.1.4.1.1588.2.1.1.1.1.22.1.5", index_oidstr, RECMETHOD_NONE, 0);

  /* Evaluate all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);

  /* Enqueue the avail item */
  i_list_enqueue (cnt->item_list, sensor);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_sensor_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_sensor_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_sensor_item *sensor = obj->itemptr;

  if (!sensor) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, sensor);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
