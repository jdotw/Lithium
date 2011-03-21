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
#include "induction/str.h"
#include "induction/hierarchy.h"
#include "induction/list.h"

#include "device/snmp.h"

#include "unit.h"
#include "sensor.h"

/* 
 * Fibre Alliance Sensors - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_sensor_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_sensor_item *sensor;
  v_unit_item *unit = passdata;

  /* Object Configuration */
  char *sensor_str = l_snmp_get_string_from_pdu (pdu);
  asprintf(&obj->desc_str, "%s %s", unit->obj->desc_str, sensor_str);
  free (sensor_str);
  obj->prio = 20;

  /* Load/Apply Refresh config */
  i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);

  /* Create sensor item struct */
  sensor = v_sensor_item_create ();
  sensor->obj = obj;
  obj->itemptr = sensor;

  /* 
   * Metric Creation 
   */

  sensor->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.8.1.4"), index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (sensor->status, 1, "Unknown");
  i_metric_enumstr_add (sensor->status, 2, "Other");
  i_metric_enumstr_add (sensor->status, 3, "OK");
  i_metric_enumstr_add (sensor->status, 4, "Warning");
  i_metric_enumstr_add (sensor->status, 5, "Failed");

  sensor->message = l_snmp_metric_create (self, obj, "message", "Message", METRIC_STRING, v_unit_oid_glue(unit->oid_suffix, ".1.3.6.1.3.94.1.8.1.6"), index_oidstr, RECMETHOD_NONE, 0);
  sensor->message->summary_flag = 1;
  
  /* Enqueue the sensor item */
  num = i_list_enqueue (cnt->item_list, sensor);
  if (num != 0)
  { i_printf (1, "v_sensor_objfact_fab failed to enqueue sensor for object %s", obj->name_str); v_sensor_item_free (sensor); return -1; }

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
