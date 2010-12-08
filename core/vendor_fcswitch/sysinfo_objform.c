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

#include "sysinfo.h"

int v_sysinfo_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_sysinfo_item *sysinfo = obj->itemptr;

  /* 
   * System Information
   */

  i_form_frame_start (reqdata->form_out, obj->name_str, "System Information");
  
  str = i_metric_valstr (sysinfo->uptime, NULL);
  i_form_string_add (reqdata->form_out, "uptime", "Uptime", str);
  free (str);

  str = i_metric_valstr (sysinfo->descr, NULL);
  i_form_string_add (reqdata->form_out, "descr", "Description", str);
  free (str);

  str = i_metric_valstr (sysinfo->contact, NULL);
  i_form_string_add (reqdata->form_out, "contact", "Contact", str);
  free (str);

  str = i_metric_valstr (sysinfo->name, NULL);
  i_form_string_add (reqdata->form_out, "name", "Name", str);
  free (str);

  str = i_metric_valstr (sysinfo->location, NULL);
  i_form_string_add (reqdata->form_out, "location", "Location", str);
  free (str);

  str = i_metric_valstr (sysinfo->vendor, NULL);
  i_form_string_add (reqdata->form_out, "vendor", "Vendor", str);
  free (str);

  str = i_metric_valstr (sysinfo->serial, NULL);
  i_form_string_add (reqdata->form_out, "serial", "Serial", str);
  free (str);

  str = i_metric_valstr (sysinfo->product, NULL);
  i_form_string_add (reqdata->form_out, "product", "Product", str);
  free (str);

  str = i_metric_valstr (sysinfo->type, NULL);
  i_form_string_add (reqdata->form_out, "type", "Type", str);
  free (str);

  str = i_metric_valstr (sysinfo->state, NULL);
  i_form_string_add (reqdata->form_out, "state", "State", str);
  free (str);

  str = i_metric_valstr (sysinfo->status, NULL);
  i_form_string_add (reqdata->form_out, "status", "Status", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}


