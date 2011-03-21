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
#include "psu.h"

int v_psu_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_psu_item *psu = obj->itemptr;

  /* 
   * CPU resource
   */

  asprintf (&str, "'%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (psu->exhaust_temp, NULL);
  i_form_string_add (reqdata->form_out, "exhaust_temp", "Exhaust Temperature", str);
  free (str);

  str = i_metric_valstr (psu->v_12v, NULL);
  i_form_string_add (reqdata->form_out, "v_12v", "12v Supply", str);
  free (str);

  str = i_metric_valstr (psu->v_5v_standby, NULL);
  i_form_string_add (reqdata->form_out, "v_5v_standby", "5v Standby Supply", str);
  free (str);

  str = i_metric_valstr (psu->power, NULL);
  i_form_string_add (reqdata->form_out, "power", "Power", str);
  free (str);

  str = i_metric_valstr (psu->fan_in_rpm, NULL);
  i_form_string_add (reqdata->form_out, "fan_in_rpm", "Fan Inlet RPM", str);
  free (str);

  str = i_metric_valstr (psu->fan_out_rpm, NULL);
  i_form_string_add (reqdata->form_out, "fan_out_rpm", "Fan Outlet RPM", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_psu_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_psu_item *psu = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, psu->power, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, psu->power, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, psu->power, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, psu->power, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}




