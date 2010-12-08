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

  str = i_metric_valstr (drive->capacity, NULL);
  i_form_string_add (reqdata->form_out, "capacity", "Capacity", str);
  free (str);

  str = i_metric_valstr (drive->interconnect, NULL);
  i_form_string_add (reqdata->form_out, "interconnect", "Interconnect", str);
  free (str);

  str = i_metric_valstr (drive->diskid, NULL);
  i_form_string_add (reqdata->form_out, "diskid", "Disk ID", str);
  free (str);

  str = i_metric_valstr (drive->vendor, NULL);
  i_form_string_add (reqdata->form_out, "vendor", "Vendor", str);
  free (str);

  str = i_metric_valstr (drive->model, NULL);
  i_form_string_add (reqdata->form_out, "model", "Model", str);
  free (str);

  str = i_metric_valstr (drive->write_bps, NULL);
  i_form_string_add (reqdata->form_out, "write_bps", "Write Throughput", str);
  free (str);

  str = i_metric_valstr (drive->read_bps, NULL);
  i_form_string_add (reqdata->form_out, "read_bps", "Read Throughput", str);
  free (str);

  str = i_metric_valstr (drive->smart, NULL);
  i_form_string_add (reqdata->form_out, "smart", "SMART", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_drives_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_drives_item *drive = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, drive->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, drive->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, drive->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, drive->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}




