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

#include "snmp_sysinfo.h"

int l_snmp_sysinfo_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  l_snmp_sysinfo_item *sitem = obj->itemptr;

  /* Check Item */
  if (obj != l_snmp_sysinfo_obj())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!sitem)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, "sysinfo", "System Information");  

  /* 
   * Info Strings 
   */

  str = i_metric_valstr (sitem->uptime, NULL);
  i_form_string_add (reqdata->form_out, "uptime", "Uptime", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->name, NULL);
  i_form_string_add (reqdata->form_out, "name", "Name", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->descr, NULL);
  i_form_string_add (reqdata->form_out, "descr", "Description", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->location, NULL);
  i_form_string_add (reqdata->form_out, "location", "Location", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->contact, NULL);
  i_form_string_add (reqdata->form_out, "contact", "Contact", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->services, NULL);
  i_form_string_add (reqdata->form_out, "services", "Services", str);
  if (str) free (str);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "sysinfo");

  return 1;
}

