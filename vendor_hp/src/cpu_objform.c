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

#include "cpu.h"

int v_cpu_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_cpu_item *cpu = obj->itemptr;

  /* 
   * CPU resource
   */

  asprintf (&str, "CPU '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (cpu->util_pc, NULL);
  i_form_string_add (reqdata->form_out, "util_pc", "Utilizaton", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_cpu_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Historic Data 
   */

  /* Day */
//  i_form_metcgraph_add (self, reqdata->form_out, cpu->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
//  i_form_metcgraph_add (self, reqdata->form_out, cpu->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
//  i_form_metcgraph_add (self, reqdata->form_out, cpu->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
//  i_form_metcgraph_add (self, reqdata->form_out, cpu->load_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



