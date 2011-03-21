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

#include "drives.h"

int v_drives_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_drives_item *drive = obj->itemptr;

  /* 
   * Drive
   */

  asprintf (&str, "Drive '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (drive->smart_status, NULL);
  i_form_string_add (reqdata->form_out, "smart_status", "SMART Status", str);
  free (str);

  str = i_metric_valstr (drive->vendor, NULL);
  i_form_string_add (reqdata->form_out, "vendor", "Vendor", str);
  free (str);

  str = i_metric_valstr (drive->firmware, NULL);
  i_form_string_add (reqdata->form_out, "firmware", "Firmware", str);
  free (str);

  str = i_metric_valstr (drive->sectorcapacity, NULL);
  i_form_string_add (reqdata->form_out, "sectorcapacity", "Sector Capacity", str);
  free (str);

  str = i_metric_valstr (drive->capacity, NULL);
  i_form_string_add (reqdata->form_out, "capacity", "Capacity", str);
  free (str);

  str = i_metric_valstr (drive->badblockcount, NULL);
  i_form_string_add (reqdata->form_out, "badblockcount", "Bad Block Count", str);
  free (str);

  str = i_metric_valstr (drive->remapcount, NULL);
  i_form_string_add (reqdata->form_out, "remapcount", "Re-Map Count", str);
  free (str);

  str = i_metric_valstr (drive->online, NULL);
  i_form_string_add (reqdata->form_out, "online", "Online", str);
  free (str);

  str = i_metric_valstr (drive->arraymember, NULL);
  i_form_string_add (reqdata->form_out, "arraymember", "Member Array", str);
  free (str);

  str = i_metric_valstr (drive->rebuilding, NULL);
  i_form_string_add (reqdata->form_out, "rebuilding", "Rebuilding", str);
  free (str);

  str = i_metric_valstr (drive->brokenraidmember, NULL);
  i_form_string_add (reqdata->form_out, "brokenraidmember", "Broken RAID Member", str);
  free (str);

  str = i_metric_valstr (drive->diskcache_enabled, NULL);
  i_form_string_add (reqdata->form_out, "diskcache_enabled", "Disk Cache Enabled", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}


