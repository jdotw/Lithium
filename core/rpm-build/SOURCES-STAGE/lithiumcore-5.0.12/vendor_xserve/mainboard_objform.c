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
#include "mainboard.h"

int v_mainboard_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_mainboard_item *mb = obj->itemptr;

  /* 
   * Fans resource
   */

  asprintf (&str, "Blower '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (mb->main_12v, NULL);
  i_form_string_add (reqdata->form_out, "main_12v", "Main 12v", str);
  free (str);

  str = i_metric_valstr (mb->main_3_3v, NULL);
  i_form_string_add (reqdata->form_out, "main_3_3v", "Main 3.3v", str);
  free (str);

  str = i_metric_valstr (mb->standby, NULL);
  i_form_string_add (reqdata->form_out, "standby", "Standby", str);
  free (str);

  str = i_metric_valstr (mb->fbdimm_vrm_12v, NULL);
  i_form_string_add (reqdata->form_out, "fbdimm_vrm_12v", "FBDIMM 12v VRM Input", str);
  free (str);

  str = i_metric_valstr (mb->ns_bridge_power, NULL);
  i_form_string_add (reqdata->form_out, "ns_bridge_power", "North and South Bridge Power", str);
  free (str);

  str = i_metric_valstr (mb->nbridge_temp, NULL);
  i_form_string_add (reqdata->form_out, "nbridge_power", "North Bridge Temperature", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_mainboard_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_mainboard_item *mb = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, mb->main_12v, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, mb->main_12v, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, mb->main_12v, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, mb->main_12v, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}




