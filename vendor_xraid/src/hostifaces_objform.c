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

#include "hostifaces.h"

int v_hostifaces_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_hostifaces_item *hostiface = obj->itemptr;

  /* 
   * Host Interfaces
   */

  asprintf (&str, "Volume '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (hostiface->linkstate, NULL);
  i_form_string_add (reqdata->form_out, "linkstate", "Link State", str);
  free (str);

  str = i_metric_valstr (hostiface->hardloopid, NULL);
  i_form_string_add (reqdata->form_out, "hardloopid", "Hardware Loop ID", str);
  free (str);

  str = i_metric_valstr (hostiface->topology, NULL);
  i_form_string_add (reqdata->form_out, "topology", "Topology", str);
  free (str);

  str = i_metric_valstr (hostiface->actual_topology, NULL);
  i_form_string_add (reqdata->form_out, "actual_topology", "Actual Topology", str);
  free (str);

  str = i_metric_valstr (hostiface->speed, NULL);
  i_form_string_add (reqdata->form_out, "speed", "Speed", str);
  free (str);

  str = i_metric_valstr (hostiface->hardloopid_enabled, NULL);
  i_form_string_add (reqdata->form_out, "hardloopid_enabled", "Hardware Loop ID Enabled", str);
  free (str);

  str = i_metric_valstr (hostiface->type, NULL);
  i_form_string_add (reqdata->form_out, "type", "Type", str);
  free (str);

  str = i_metric_valstr (hostiface->max_speed, NULL);
  i_form_string_add (reqdata->form_out, "max_speed", "Maximum Speed", str);
  free (str);

  str = i_metric_valstr (hostiface->actual_speed, NULL);
  i_form_string_add (reqdata->form_out, "actual_speed", "Actual Speed", str);
  free (str);

  str = i_metric_valstr (hostiface->wwn, NULL);
  i_form_string_add (reqdata->form_out, "wwn", "WWN", str);
  free (str);

  str = i_metric_valstr (hostiface->linkfailure_count, NULL);
  i_form_string_add (reqdata->form_out, "linkfailure_count", "Link Failure Count", str);
  free (str);

  str = i_metric_valstr (hostiface->syncloss_count, NULL);
  i_form_string_add (reqdata->form_out, "syncloss_count", "Sync Loss Count", str);
  free (str);

  str = i_metric_valstr (hostiface->signalloss_count, NULL);
  i_form_string_add (reqdata->form_out, "signalloss_count", "Signal Loss Count", str);
  free (str);

  str = i_metric_valstr (hostiface->protocolerror_count, NULL);
  i_form_string_add (reqdata->form_out, "protocolerror_count", "Protocol Error Count", str);
  free (str);

  str = i_metric_valstr (hostiface->invalidword_count, NULL);
  i_form_string_add (reqdata->form_out, "invalidword_count", "Invalid Word Count", str);
  free (str);

  str = i_metric_valstr (hostiface->invalidcrc_count, NULL);
  i_form_string_add (reqdata->form_out, "invalidcrc_count", "Invalid CRC Count", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  i_form_frame_start (reqdata->form_out, "lun", "LUNs");
  
  str = i_metric_valstr (hostiface->lun0_array, NULL);
  i_form_string_add (reqdata->form_out, "lun0_array", "LUN 0 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun0_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun0_slice", "LUN 0 Slice", str);
  free (str);

  str = i_metric_valstr (hostiface->lun1_array, NULL);
  i_form_string_add (reqdata->form_out, "lun1_array", "LUN 1 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun1_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun1_slice", "LUN 1 Slice", str);
  free (str);

  str = i_metric_valstr (hostiface->lun2_array, NULL);
  i_form_string_add (reqdata->form_out, "lun2_array", "LUN 2 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun2_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun2_slice", "LUN 2 Slice", str);
  free (str);

  str = i_metric_valstr (hostiface->lun3_array, NULL);
  i_form_string_add (reqdata->form_out, "lun3_array", "LUN 3 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun3_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun3_slice", "LUN 3 Slice", str);
  free (str);

  str = i_metric_valstr (hostiface->lun4_array, NULL);
  i_form_string_add (reqdata->form_out, "lun4_array", "LUN 4 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun4_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun4_slice", "LUN 4 Slice", str);
  free (str);

  str = i_metric_valstr (hostiface->lun5_array, NULL);
  i_form_string_add (reqdata->form_out, "lun5_array", "LUN 5 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun5_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun5_slice", "LUN 5 Slice", str);
  free (str);

  str = i_metric_valstr (hostiface->lun6_array, NULL);
  i_form_string_add (reqdata->form_out, "lun6_array", "LUN 6 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun6_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun6_slice", "LUN 6 Slice", str);
  free (str);

  str = i_metric_valstr (hostiface->lun7_array, NULL);
  i_form_string_add (reqdata->form_out, "lun7_array", "LUN 7 Array", str);
  free (str);
  str = i_metric_valstr (hostiface->lun7_slice, NULL);
  i_form_string_add (reqdata->form_out, "lun7_slice", "LUN 7 Slice", str);
  free (str);

  i_form_frame_end (reqdata->form_out, "lun");
  
  return 1;
}



