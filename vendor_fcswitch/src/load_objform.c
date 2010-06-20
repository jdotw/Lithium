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

#include "load.h"

int v_load_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_load_item *load = obj->itemptr;

  /* 
   * Load averages
   */

  i_form_frame_start (reqdata->form_out, obj->name_str, "Load Average Current Data");

  str = i_metric_valstr (load->one_min, NULL);
  i_form_string_add (reqdata->form_out, "one_min", "One Minute Average", str);
  free (str);

  str = i_metric_valstr (load->five_min, NULL);
  i_form_string_add (reqdata->form_out, "five_min", "Five Minute Average", str);
  free (str);

  str = i_metric_valstr (load->fifteen_min, NULL);
  i_form_string_add (reqdata->form_out, "fifteen_min", "Fifteen Minute Average", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_load_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_load_item *load = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, load->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, load->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, load->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, load->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);


  return 1;
}



