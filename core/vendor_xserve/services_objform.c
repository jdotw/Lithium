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
#include <induction/str.h>
#include <induction/form.h>

#include "services.h"

int v_services_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_services_item *service = obj->itemptr;

  /* 
   * Service
   */

  asprintf (&str, "Service '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (service->state, NULL);
  i_form_string_add (reqdata->form_out, "state", "State", str);
  free (str);

  str = i_metric_valstr (service->start_time, NULL);
  i_form_string_add (reqdata->form_out, "start_time", "Start Time", str);
  free (str);

  if (service->volume)
  {
    str = i_metric_valstr (service->volume, NULL);
    i_form_string_add (reqdata->form_out, "volume", service->volume->desc_str, str);
    free (str);
  }

  if (service->rate)
  {
    str = i_metric_valstr (service->rate, NULL);
    i_form_string_add (reqdata->form_out, "rate", service->rate->desc_str, str);
    free (str);
  }

  if (service->throughput)
  {
    str = i_metric_valstr (service->throughput, NULL);
    i_form_string_add (reqdata->form_out, "throughput", "Throughput", str);
    free (str);
  }

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_services_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_services_item *service = obj->itemptr;

  /*
   * Historic Data 
   */

  if (service->volume)
  {
    /* Day */
    i_form_metgraph_add (self, reqdata->form_out, service->volume, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
    /* Week */
    i_form_metgraph_add (self, reqdata->form_out, service->volume, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
    /* Month */
    i_form_metgraph_add (self, reqdata->form_out, service->volume, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
    /* Year */
    i_form_metgraph_add (self, reqdata->form_out, service->volume, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  }

  return 1;
}




