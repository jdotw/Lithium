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

#include "sensor.h"

/* 
 * 3Com sensors - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_sensor_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_sensor_item *sensor;
 
  /* Make sure this object is a sensor */
  if (pdu->variables->name[pdu->variables->name_length-2] != 3) return -1;
  
  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_sensor_objform;
  obj->histform_func = v_sensor_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_sensor_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create sensor item struct */
  sensor = v_sensor_item_create ();
  if (!sensor)
  { i_printf (1, "v_sensor_objfact_fab failed to create sensor item for object %s", obj->name_str); return -1; }
  sensor->obj = obj;
  obj->itemptr = sensor;
  sensor->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */
  /* sensor Type */
  sensor->ent_type = l_snmp_metric_create (self, obj, "ent_type", "Sensor Type", METRIC_INTEGER, "enterprises.43.43.1.1.4.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (sensor->ent_type,1,"Not Present");
  i_metric_enumstr_add (sensor->ent_type,2,"Temperature Sensor");
    
  /* Hardware Version */
  sensor->hw_ver = l_snmp_metric_create (self, obj, "hw_ver", "Hardware Version", METRIC_STRING, "enterprises.43.43.1.1.5.1", index_oidstr, RECMETHOD_NONE, 0);

  /* Software Version */
  sensor->sw_ver = l_snmp_metric_create (self, obj, "sw_ver", "Software Version", METRIC_STRING, "enterprises.43.43.1.1.6.1", index_oidstr, RECMETHOD_NONE, 0);

  /* sensor State */
  sensor->state = l_snmp_metric_create (self, obj, "state", "Current State", METRIC_INTEGER, "enterprises.43.43.1.1.8.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (sensor->state,1,"Unknown");
  i_metric_enumstr_add (sensor->state,2,"Initialisingn");
  i_metric_enumstr_add (sensor->state,3,"Idle");
  i_metric_enumstr_add (sensor->state,4,"Operational");
  i_metric_enumstr_add (sensor->state,5,"Failure");
  i_metric_enumstr_add (sensor->state,6,"Not Present");

  /* Current Fault */
  sensor->fault = l_snmp_metric_create (self, obj, "fault", "Fault", METRIC_INTEGER, "enterprises.43.43.1.1.9.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (sensor->fault,0,"No Error");
  i_metric_enumstr_add (sensor->fault,1,"Temperature Warm");
  i_metric_enumstr_add (sensor->fault,2,"Temoerature Critical");
  i_metric_enumstr_add (sensor->fault,3,"Temperature Too High");
  
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
  /* FIX needs to free the mempool and remove
   * it from the item_list
   */

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