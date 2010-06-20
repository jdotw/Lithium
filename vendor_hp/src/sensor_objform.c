#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "sensor.h"

int v_sensor_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_sensor_item *sensor = obj->itemptr;

  /* 
   * Storage Resource
   */

  asprintf (&str, "Sensor '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (sensor->status, NULL);
  i_form_string_add (reqdata->form_out, "status", "Status", str);
  free (str);

  str = i_metric_valstr (sensor->warnings, NULL);
  i_form_string_add (reqdata->form_out, "warnings", "Warnings", str);
  free (str);

  str = i_metric_valstr (sensor->failures, NULL);
  i_form_string_add (reqdata->form_out, "failures", "Failures", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_sensor_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
//  i_object *obj = (i_object *) ent;
//  v_mempool_item *pool = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



