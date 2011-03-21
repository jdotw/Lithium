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
#include "network.h"

int v_network_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_network_item *network = obj->itemptr;

  /* 
   * CPU resource
   */

  asprintf (&str, "Network '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (network->fullname, NULL);
  i_form_string_add (reqdata->form_out, "fullname", "Full Name", str);
  free (str);

  str = i_metric_valstr (network->ip, NULL);
  i_form_string_add (reqdata->form_out, "ip", "IP Address", str);
  free (str);

  str = i_metric_valstr (network->subnetmask, NULL);
  i_form_string_add (reqdata->form_out, "subnetmask", "Subnet Mask", str);
  free (str);

  if (!v_xserve_intel_extras())
  {
    str = i_metric_valstr (network->broadcast, NULL);
    i_form_string_add (reqdata->form_out, "broadcast", "Broadcast", str);
    free (str);

    str = i_metric_valstr (network->router, NULL);
    i_form_string_add (reqdata->form_out, "router", "Router", str);
    free (str);
  }

  str = i_metric_valstr (network->bps_in, NULL);
  i_form_string_add (reqdata->form_out, "bps_in", "Input Bit Rate", str);
  free (str);

  str = i_metric_valstr (network->bps_out, NULL);
  i_form_string_add (reqdata->form_out, "bps_out", "Output Bit Rate", str);
  free (str);

  if (v_xserve_extras())
  {
    str = i_metric_valstr (network->pps_in, NULL);
    i_form_string_add (reqdata->form_out, "pps_in", "Input Packet Rate", str);
    free (str);

    str = i_metric_valstr (network->pps_out, NULL);
    i_form_string_add (reqdata->form_out, "pps_out", "Output Packet Rate", str);
    free (str);

    str = i_metric_valstr (network->mac, NULL);
    i_form_string_add (reqdata->form_out, "mac", "MAC", str);
    free (str);

    if (!v_xserve_intel_extras())
    {
      str = i_metric_valstr (network->kind, NULL);
      i_form_string_add (reqdata->form_out, "kind", "Type", str);
      free (str);
    }

    str = i_metric_valstr (network->duplex, NULL);
    i_form_string_add (reqdata->form_out, "duplex", "Duplex", str);
    free (str);

    str = i_metric_valstr (network->speed, NULL);
    i_form_string_add (reqdata->form_out, "speed", "Speed", str);
    free (str);

    str = i_metric_valstr (network->linkstate, NULL);
    i_form_string_add (reqdata->form_out, "linkstate", "Link State", str);
    free (str);
  }

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_network_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_network_item *network = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, network->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, network->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, network->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, network->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  
  return 1;
}




