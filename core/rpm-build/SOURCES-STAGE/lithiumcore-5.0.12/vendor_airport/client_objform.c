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
#include "induction/form.h"
#include "induction/str.h"

#include "client.h"

int v_client_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_client_item *client = obj->itemptr;

  /* 
   * Wireless clients 
   */

  asprintf (&str, "Wireless Client '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (client->strength, NULL);
  i_form_string_add (reqdata->form_out, "strength", "Signal Strength", str);
  free (str);

  str = i_metric_valstr (client->noise, NULL);
  i_form_string_add (reqdata->form_out, "noise", "Noise Level", str);
  free (str);

  str = i_metric_valstr (client->rate, NULL);
  i_form_string_add (reqdata->form_out, "rate", "Data Rate", str);
  free (str);

  str = i_metric_valstr (client->tx_rate, NULL);
  i_form_string_add (reqdata->form_out, "transmit_fps", "Transmit Throughput", str);
  free (str);

  str = i_metric_valstr (client->rx_rate, NULL);
  i_form_string_add (reqdata->form_out, "receive_fps", "Receive Throughput", str);
  free (str);

  str = i_metric_valstr (client->tx_eps, NULL);
  i_form_string_add (reqdata->form_out, "tx_eps", "Transmit Errors", str);
  free (str);

  str = i_metric_valstr (client->rx_eps, NULL);
  i_form_string_add (reqdata->form_out, "rx_eps", "Receive Errors", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_client_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_client_item *client = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, client->strength, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, client->strength, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, client->strength, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, client->strength, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



