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

#include "status.h"

int v_status_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_status_item *status = obj->itemptr;

  /* 
   * CPU resource
   */

  asprintf (&str, "Network '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  /* AMbient Temp */

  str = i_metric_valstr (status->ambient_temp, NULL);
  i_form_string_add (reqdata->form_out, "ambient_temp", "Ambient Temperature", str);
  free (str);

  /* UPS */

  str = i_metric_valstr (status->batterylow, NULL);
  i_form_string_add (reqdata->form_out, "batterylow", "UPS Battery Low", str);
  free (str);

  str = i_metric_valstr (status->batteryinuse, NULL);
  i_form_string_add (reqdata->form_out, "batteryinuse", "UPS Battery In Use", str);
  free (str);

  str = i_metric_valstr (status->linepowerdown, NULL);
  i_form_string_add (reqdata->form_out, "linepowerdown", "UPS Line Power Down", str);
  free (str);

  /* General Monitors */

  str = i_metric_valstr (status->drivetemp, NULL);
  i_form_string_add (reqdata->form_out, "drivetemp", "Drive Temperature", str);
  free (str);

  str = i_metric_valstr (status->blowertemp, NULL);
  i_form_string_add (reqdata->form_out, "blowertemp", "Blower Temperature", str);
  free (str);

  str = i_metric_valstr (status->raidtemp, NULL);
  i_form_string_add (reqdata->form_out, "raidtemp", "RAID Controller Temp", str);
  free (str);

  str = i_metric_valstr (status->blowerspeed, NULL);
  i_form_string_add (reqdata->form_out, "blowerspeed", "Blower Speed", str);
  free (str);

  str = i_metric_valstr (status->emutemp, NULL);
  i_form_string_add (reqdata->form_out, "emutemp", "EMU Temp", str);
  free (str);

  str = i_metric_valstr (status->ambientstate, NULL);
  i_form_string_add (reqdata->form_out, "ambientstate", "Ambient Temp", str);
  free (str);

  str = i_metric_valstr (status->smart, NULL);
  i_form_string_add (reqdata->form_out, "smart", "SMART Status", str);
  free (str);

  str = i_metric_valstr (status->blower, NULL);
  i_form_string_add (reqdata->form_out, "blower", "Blower", str);
  free (str);

  str = i_metric_valstr (status->raidcomms, NULL);
  i_form_string_add (reqdata->form_out, "raidcomms", "RAID Comms", str);
  free (str);

  str = i_metric_valstr (status->battery, NULL);
  i_form_string_add (reqdata->form_out, "battery", "Battery", str);
  free (str);

  str = i_metric_valstr (status->serviceid, NULL);
  i_form_string_add (reqdata->form_out, "serviceid", "Service ID", str);
  free (str);

  str = i_metric_valstr (status->buzzer, NULL);
  i_form_string_add (reqdata->form_out, "buzzer", "Buzzer", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_status_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_status_item *status = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, status->ambient_temp, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, status->ambient_temp, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, status->ambient_temp, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, status->ambient_temp, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  
  return 1;
}




