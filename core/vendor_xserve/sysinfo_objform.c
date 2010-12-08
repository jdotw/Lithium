#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/ip.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/form.h"
#include "induction/str.h"

#include "osx_server.h"
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

  str = i_metric_valstr (sitem->computername, NULL);
  i_form_string_add (reqdata->form_out, "computername", "Computer Name", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->hostname, NULL);
  i_form_string_add (reqdata->form_out, "hostname", "Host Name", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->boottime, NULL);
  i_form_string_add (reqdata->form_out, "boottime", "Boot Time", str);
  if (str) free (str);

  str = i_metric_valstr (sitem->version, NULL);
  i_form_string_add (reqdata->form_out, "version", "Version", str);
  if (str) free (str);

  if (v_xserve_extras())
  {
    str = i_metric_valstr (sitem->cpu_count, NULL);
    i_form_string_add (reqdata->form_out, "cpu_count", "CPU Count", str);
    if (str) free (str);

    str = i_metric_valstr (sitem->cpu_speed, NULL);
    i_form_string_add (reqdata->form_out, "cpu_speed", "CPU Speed", str);
    if (str) free (str);

    if (v_xserve_intel_extras())
    {
      str = i_metric_valstr (sitem->cpu_type, NULL);
      i_form_string_add (reqdata->form_out, "cpu_type", "CPU Type", str);
      if (str) free (str);
    }
    else
    {
      str = i_metric_valstr (sitem->cpu_l2cache, NULL);
      i_form_string_add (reqdata->form_out, "cpu_l2cache", "CPU L2Cache", str);
      if (str) free (str);

      str = i_metric_valstr (sitem->ram_size, NULL);
      i_form_string_add (reqdata->form_out, "ram_size", "RAM Size", str);
      if (str) free (str);
      
      str = i_metric_valstr (sitem->bootrom, NULL);
      i_form_string_add (reqdata->form_out, "bootrom", "Boot ROM", str);
      if (str) free (str);
    }
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, "sysinfo");

  return 1;
}

