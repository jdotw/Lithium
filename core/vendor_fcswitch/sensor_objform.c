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
#include "induction/form.h"
#include "induction/str.h"

#include "sensor.h"

int v_sensor_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_sensor_item *sensor = obj->itemptr;

  /* 
   * Wireless sensors 
   */

  asprintf (&str, "Physical Interface '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (sensor->status, NULL);
  i_form_string_add (reqdata->form_out, "status", "Status", str);
  free (str);

  str = i_metric_valstr (sensor->info, NULL);
  i_form_string_add (reqdata->form_out, "info", "Info", str);
  free (str);

  str = i_metric_valstr (sensor->message, NULL);
  i_form_string_add (reqdata->form_out, "message", "Message", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}


