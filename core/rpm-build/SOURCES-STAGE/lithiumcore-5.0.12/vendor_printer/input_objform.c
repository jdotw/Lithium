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

#include "input.h"

int v_input_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_input_item *input = obj->itemptr;

  /* 
   * Printer Input
   */

  asprintf (&str, "Printer Input '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (input->media, NULL);
  i_form_string_add (reqdata->form_out, "type", "Type", str);
  free (str);

  str = i_metric_valstr (input->media, NULL);
  i_form_string_add (reqdata->form_out, "media", "Media", str);
  free (str);

  str = i_metric_valstr (input->max_capacity, NULL);
  i_form_string_add (reqdata->form_out, "max_capacity", "Maximum Capacity", str);
  free (str);

  str = i_metric_valstr (input->current_level, NULL);
  i_form_string_add (reqdata->form_out, "current_level", "Current Level", str);
  free (str);

  str = i_metric_valstr (input->remaining_pc, NULL);
  i_form_string_add (reqdata->form_out, "remaining_pc", "Remaining", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_input_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_input_item *input = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, input->remaining_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, input->remaining_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, input->remaining_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, input->remaining_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



