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

#include "physical.h"

int v_physical_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_physical_item *physical = obj->itemptr;

  /* 
   * Wireless physicals 
   */

  asprintf (&str, "Physical Interface '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (physical->desc, NULL);
  i_form_string_add (reqdata->form_out, "desc", "Full Name", str);
  free (str);

  str = i_metric_valstr (physical->state, NULL);
  i_form_string_add (reqdata->form_out, "state", "State", str);
  free (str);

  str = i_metric_valstr (physical->speed, NULL);
  i_form_string_add (reqdata->form_out, "speed", "Speed", str);
  free (str);

  str = i_metric_valstr (physical->duplex, NULL);
  i_form_string_add (reqdata->form_out, "duplex", "Duplex", str);
  free (str);

  str = i_metric_valstr (physical->tx_rate, NULL);
  i_form_string_add (reqdata->form_out, "transmit_fps", "Transmit Throughput", str);
  free (str);

  str = i_metric_valstr (physical->rx_rate, NULL);
  i_form_string_add (reqdata->form_out, "receive_fps", "Receive Throughput", str);
  free (str);

  str = i_metric_valstr (physical->tx_eps, NULL);
  i_form_string_add (reqdata->form_out, "tx_eps", "Transmit Errors", str);
  free (str);

  str = i_metric_valstr (physical->rx_eps, NULL);
  i_form_string_add (reqdata->form_out, "rx_eps", "Receive Errors", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_physical_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_physical_item *physical = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, physical->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, physical->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, physical->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, physical->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



