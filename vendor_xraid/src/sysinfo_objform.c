#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ip.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "sysinfo.h"

int v_sysinfo_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_sysinfo_item *sitem = obj->itemptr;

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, "sysinfo", "System Information");  

  /* 
   * Info Strings 
   */

  str = i_metric_valstr (sitem->name, NULL);
  i_form_string_add (reqdata->form_out, "name", "Name", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->contact, NULL);
  i_form_string_add (reqdata->form_out, "contact", "Contact", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->location, NULL);
  i_form_string_add (reqdata->form_out, "location", "Location", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->version, NULL);
  i_form_string_add (reqdata->form_out, "version", "Version", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->uptime, NULL);
  i_form_string_add (reqdata->form_out, "uptime", "Uptime", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->mac, NULL);
  i_form_string_add (reqdata->form_out, "mac", "MAC", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->ip, NULL);
  i_form_string_add (reqdata->form_out, "ip", "IP Address", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->subnet, NULL);
  i_form_string_add (reqdata->form_out, "subnet", "Subnet", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->firmware_top, NULL);
  i_form_string_add (reqdata->form_out, "firmware_top", "Upper Firmware", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->firmware_bottom, NULL);
  i_form_string_add (reqdata->form_out, "firmware_bottom", "Lower Firmware", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->serial, NULL);
  i_form_string_add (reqdata->form_out, "serial", "Serial Number", str);
  if (str) free (str);


  /* End Frame */
  i_form_frame_end (reqdata->form_out, "sysinfo");

  return 1;
}

