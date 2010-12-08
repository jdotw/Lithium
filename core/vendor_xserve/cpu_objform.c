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

#include "osx_server.h"
#include "cpu.h"

int v_cpu_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_cpu_item *cpu = obj->itemptr;

  /* 
   * CPU resource
   */

  asprintf (&str, "CPU '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (cpu->usage, NULL);
  i_form_string_add (reqdata->form_out, "usage", "Utilisation", str);
  free (str);

  if (v_xserve_extras())
  {
    str = i_metric_valstr (cpu->power_watts, NULL);
    i_form_string_add (reqdata->form_out, "power_watts", "Power", str);
    free (str);

    str = i_metric_valstr (cpu->power_vcore, NULL);
    i_form_string_add (reqdata->form_out, "power_vcore", "Vcore", str);
    free (str);

    str = i_metric_valstr (cpu->temp_inlet, NULL);
    i_form_string_add (reqdata->form_out, "temp_inlet", "Inlet Temperature", str);
    free (str);

    if (v_xserve_intel_extras())
    {
      /* Intel Xserve */

      str = i_metric_valstr (cpu->temp_ambient, NULL);
      i_form_string_add (reqdata->form_out, "temp_heatsink", "Heatsink Temperature", str);
      free (str);

      str = i_metric_valstr (cpu->v_12v, NULL);
      i_form_string_add (reqdata->form_out, "v_12v", "12v VRM Feed", str);
      free (str);

    }
    else
    {
      /* PPC Xserve */

      str = i_metric_valstr (cpu->temp_ambient, NULL);
      i_form_string_add (reqdata->form_out, "temp_ambient", "Ambient Temperature", str);
      free (str);

      str = i_metric_valstr (cpu->temp_internal, NULL);
      i_form_string_add (reqdata->form_out, "temp_internal", "Internal Temperature", str);
      free (str);

      str = i_metric_valstr (cpu->current_core, NULL);
      i_form_string_add (reqdata->form_out, "current_core", "Vcore Current", str);
      free (str);

      str = i_metric_valstr (cpu->current_12v, NULL);
      i_form_string_add (reqdata->form_out, "current_12v", "12v Current", str);
      free (str);
    }

  }

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_cpu_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_cpu_item *cpu = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, cpu->usage, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, cpu->usage, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, cpu->usage, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, cpu->usage, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}




