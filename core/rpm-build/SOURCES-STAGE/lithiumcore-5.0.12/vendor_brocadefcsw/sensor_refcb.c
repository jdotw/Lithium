#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/adminstate.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "sensor.h"

/* Sensor Refresh Callbacks */

int v_sensor_status_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when a refresh of the status metric is complete
   *
   * Check status and disable other metrics if it's absent(6)
   *   * Always return 0 to keep callback alive
   */

  i_metric_value *val;
  i_metric *met = (i_metric *) ent;
  v_sensor_item *sensor = met->obj->itemptr;
  i_metric *child_met;
  i_metric *value_met = (i_metric *) passdata;

  val = i_metric_curval (met);
  if (!val) return 0;

  if (val->integer == 6 && sensor->disabled_metrics == 0)  
  {
    /* Status of the sensor is Absent
     * Disable all other metric in this object
     */
    for (i_list_move_head(met->obj->met_list); (child_met=i_list_restore(met->obj->met_list))!=NULL; i_list_move_next(met->obj->
met_list))
    {
      if (child_met != met)
      { i_adminstate_change (self, ENTITY(child_met), ENTADMIN_DISABLED); }
    }
    
    if (value_met->hidden == 0) value_met->hidden = 1;

    sensor->disabled_metrics = 1;
  }
  else if (val->integer != 6 && sensor->disabled_metrics == 1)
  {
    /* Sensor is present, enable all other metrics */
    for (i_list_move_head(met->obj->met_list); (child_met=i_list_restore(met->obj->met_list))!=NULL; i_list_move_next(met->obj->
met_list))
    {
      if (child_met != met)
      { i_adminstate_change (self, ENTITY(child_met), ENTADMIN_ENABLED); }
    }

    if (value_met->hidden == 1) value_met->hidden = 0;

    sensor->disabled_metrics = 0;
  }

  return 0;
}

int v_sensor_type_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when a refresh of the type metric is complete
   *
   * Depending on the type, set the units of the value 
   *
   */

  i_metric_value *val;
  i_metric *value_met = (i_metric *) passdata;
  i_metric *met = (i_metric *) ent;

  val = i_metric_curval (met);
  if (!val) return 0;

  switch (val->integer)
  {
    case 1:
      /* Temp -- degC */
      if (!value_met->unit_str) value_met->unit_str = strdup ("deg.C");
      break;
    case 2:
      /* Fan -- RPM */
      if (!value_met->unit_str) value_met->unit_str = strdup ("RPM");
      break;
    case 3:
      /* PSU -- Hide, no value will be shown */
      if (value_met->hidden == 0) value_met->hidden = 1;
      break;
  }

  return 0;
}


