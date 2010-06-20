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

#include "power.h"

int v_power_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_power_item *power = obj->itemptr;

  /* 
   * powers 
   */

  asprintf (&str, "power: '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (power->ent_type, NULL);
  i_form_string_add (reqdata->form_out, "ent_type", "Entity Type", str);
  free (str);

  str = i_metric_valstr (power->hw_ver, NULL);
  i_form_string_add (reqdata->form_out, "hw_ver", "Hardware Version", str);
  free (str);

  str = i_metric_valstr (power->sw_ver, NULL);
  i_form_string_add (reqdata->form_out, "sw_ver", "Software Version", str);
  free (str);

  str = i_metric_valstr (power->state, NULL);
  i_form_string_add (reqdata->form_out, "state", "Current State", str);
  free (str);

  str = i_metric_valstr (power->fault, NULL);
  i_form_string_add (reqdata->form_out, "fault", "Current Fault", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_power_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
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



