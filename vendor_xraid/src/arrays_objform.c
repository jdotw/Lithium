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

#include "arrays.h"

int v_arrays_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_arrays_item *array = obj->itemptr;

  /* 
   * CPU resource
   */

  asprintf (&str, "Array '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (array->stripesize, NULL);
  i_form_string_add (reqdata->form_out, "stripesize", "Stripe Size", str);
  free (str);

  str = i_metric_valstr (array->raidlevel, NULL);
  i_form_string_add (reqdata->form_out, "raidlevel", "RAID Level", str);
  free (str);

  str = i_metric_valstr (array->membercount, NULL);
  i_form_string_add (reqdata->form_out, "membercount", "Member Count", str);
  free (str);

  str = i_metric_valstr (array->raidtype, NULL);
  i_form_string_add (reqdata->form_out, "raidtype", "RAID Type", str);
  free (str);

  str = i_metric_valstr (array->raidstatus, NULL);
  i_form_string_add (reqdata->form_out, "raidstatus", "RAID Status", str);
  free (str);

  str = i_metric_valstr (array->sectorcapacity, NULL);
  i_form_string_add (reqdata->form_out, "sectorcapacity", "Sector Capacity", str);
  free (str);

  str = i_metric_valstr (array->capacity, NULL);
  i_form_string_add (reqdata->form_out, "capacity", "Capacity", str);
  free (str);

  str = i_metric_valstr (array->initialize, NULL);
  i_form_string_add (reqdata->form_out, "initialize", "Initialization", str);
  free (str);

  str = i_metric_valstr (array->addmember, NULL);
  i_form_string_add (reqdata->form_out, "addmember", "Add Member", str);
  free (str);

  str = i_metric_valstr (array->verify, NULL);
  i_form_string_add (reqdata->form_out, "verify", "Verify", str);
  free (str);

  str = i_metric_valstr (array->expand, NULL);
  i_form_string_add (reqdata->form_out, "expand", "Expand", str);
  free (str);

  str = i_metric_valstr (array->slice1_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_1", "Slice 1 Size", str);
  free (str);

  str = i_metric_valstr (array->slice2_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_2", "Slice 2 Size", str);
  free (str);

  str = i_metric_valstr (array->slice3_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_3", "Slice 3 Size", str);
  free (str);

  str = i_metric_valstr (array->slice4_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_4", "Slice 4 Size", str);
  free (str);

  str = i_metric_valstr (array->slice5_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_5", "Slice 5 Size", str);
  free (str);

  str = i_metric_valstr (array->slice6_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_6", "Slice 6 Size", str);
  free (str);

  str = i_metric_valstr (array->slice7_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_7", "Slice 7 Size", str);
  free (str);

  str = i_metric_valstr (array->slice8_size, NULL);
  i_form_string_add (reqdata->form_out, "slice_8", "Slice 8 Size", str);
  free (str);

  str = i_metric_valstr (array->member1_slot, NULL);
  i_form_string_add (reqdata->form_out, "member1_slot", "Member Drive 1", str);
  free (str);

  str = i_metric_valstr (array->member2_slot, NULL);
  i_form_string_add (reqdata->form_out, "member2_slot", "Member Drive 2", str);
  free (str);

  str = i_metric_valstr (array->member3_slot, NULL);
  i_form_string_add (reqdata->form_out, "member3_slot", "Member Drive 3", str);
  free (str);

  str = i_metric_valstr (array->member4_slot, NULL);
  i_form_string_add (reqdata->form_out, "member4_slot", "Member Drive 4", str);
  free (str);

  str = i_metric_valstr (array->member5_slot, NULL);
  i_form_string_add (reqdata->form_out, "member5_slot", "Member Drive 5", str);
  free (str);

  str = i_metric_valstr (array->member6_slot, NULL);
  i_form_string_add (reqdata->form_out, "member6_slot", "Member Drive 6", str);
  free (str);

  str = i_metric_valstr (array->member7_slot, NULL);
  i_form_string_add (reqdata->form_out, "member7_slot", "Member Drive 7", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}




