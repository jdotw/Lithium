#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "induction/form.h"

#include "chassis.h"

int v_chassis_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_chassis_item *item = obj->itemptr;

  /* 
   * Enclosure Temp Metric
   */

  i_form_frame_start (reqdata->form_out, obj->name_str, "Xserve Chassis");

  str = i_metric_valstr (item->systempower, NULL);
  i_form_string_add (reqdata->form_out, "systempower", "System Power", str);
  if (str) free (str);

  str = i_metric_valstr (item->serial, NULL);
  i_form_string_add (reqdata->form_out, "serial", "Serial Number", str);
  if (str) free (str);

  str = i_metric_valstr (item->model, NULL);
  i_form_string_add (reqdata->form_out, "model", "Model", str);
  if (str) free (str);

  str = i_metric_valstr (item->powercontrolfault, NULL);
  i_form_string_add (reqdata->form_out, "powercontrolfault", "Power Control Fault", str);
  if (str) free (str);

  str = i_metric_valstr (item->mainspowerfault, NULL);
  i_form_string_add (reqdata->form_out, "mainspowerfault", "Mains Power Fault", str);
  if (str) free (str);

  str = i_metric_valstr (item->poweroverload, NULL);
  i_form_string_add (reqdata->form_out, "poweroverload", "Power Overload", str);
  if (str) free (str);

  str = i_metric_valstr (item->poweroncause, NULL);
  i_form_string_add (reqdata->form_out, "poweroncause", "Last Power-On Cause", str);
  if (str) free (str);

  str = i_metric_valstr (item->poweroffcause, NULL);
  i_form_string_add (reqdata->form_out, "poweroffcause", "Last Power-Off Cause", str);
  if (str) free (str);

//  str = i_metric_valstr (item->coolingfault, NULL);
//  i_form_string_add (reqdata->form_out, "coolingfault", "Cooling/Fan Fault", str);
//  if (str) free (str);

//  str = i_metric_valstr (item->drivefault, NULL);
//  i_form_string_add (reqdata->form_out, "drivefault", "Drive Fault", str);
//  if (str) free (str);

  str = i_metric_valstr (item->keylock, NULL);
  i_form_string_add (reqdata->form_out, "keylock", "Key Lock", str);
  if (str) free (str);

  str = i_metric_valstr (item->idlight, NULL);
  i_form_string_add (reqdata->form_out, "idlight", "System Identifier", str);
  if (str) free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}


