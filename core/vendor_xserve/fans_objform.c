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

#include "osx_server.h"
#include "fans.h"

int v_fans_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_fans_item *fan = obj->itemptr;

  /* 
   * Fans resource
   */

  asprintf (&str, "Blower '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  if (v_xserve_intel_extras())
  {
    str = i_metric_valstr (fan->in_rpm, NULL);
    i_form_string_add (reqdata->form_out, "rpm_in", "Inlet RPM", str);
    free (str);
    
    str = i_metric_valstr (fan->out_rpm, NULL);
    i_form_string_add (reqdata->form_out, "rpm_out", "Outlet RPM", str);
    free (str);
  }
  else
  {
    str = i_metric_valstr (fan->rpm, NULL);
    i_form_string_add (reqdata->form_out, "rpm", "RPM", str);
    free (str);
  }

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_fans_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_fans_item *fan = obj->itemptr;

  /*
   * Historic Data 
   */

  if (v_xserve_intel_extras())
  {
    /* Day */
    i_form_metgraph_add (self, reqdata->form_out, fan->in_rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
    /* Week */
    i_form_metgraph_add (self, reqdata->form_out, fan->in_rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
    /* Month */
    i_form_metgraph_add (self, reqdata->form_out, fan->in_rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
    /* Year */
    i_form_metgraph_add (self, reqdata->form_out, fan->in_rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  }
  else
  {
    /* Day */
    i_form_metgraph_add (self, reqdata->form_out, fan->rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
    /* Week */
    i_form_metgraph_add (self, reqdata->form_out, fan->rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
    /* Month */
    i_form_metgraph_add (self, reqdata->form_out, fan->rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
    /* Year */
    i_form_metgraph_add (self, reqdata->form_out, fan->rpm, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  }

  return 1;
}




