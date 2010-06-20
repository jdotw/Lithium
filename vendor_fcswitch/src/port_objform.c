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

#include "port.h"

int v_port_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_port_item *port = obj->itemptr;

  /* 
   * Wireless ports 
   */

  asprintf (&str, "Physical Interface '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (port->wwn, NULL);
  i_form_string_add (reqdata->form_out, "desc", "WWN", str);
  free (str);

  str = i_metric_valstr (port->hwstate, NULL);
  i_form_string_add (reqdata->form_out, "hwstate", "Hardware State", str);
  free (str);

  str = i_metric_valstr (port->speed, NULL);
  i_form_string_add (reqdata->form_out, "speed", "Speed", str);
  free (str);

  str = i_metric_valstr (port->bps_in, NULL);
  i_form_string_add (reqdata->form_out, "bps_in", "Input Throughput", str);
  free (str);

  str = i_metric_valstr (port->bps_out, NULL);
  i_form_string_add (reqdata->form_out, "bps_out", "Output Throughput", str);
  free (str);

  str = i_metric_valstr (port->eps, NULL);
  i_form_string_add (reqdata->form_out, "eps", "Error Rate", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_port_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_port_item *port = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, port->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, port->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, port->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, port->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



