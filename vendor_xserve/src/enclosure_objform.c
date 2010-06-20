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

#include "enclosure.h"

int v_enclosure_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_enclosure_item *item = obj->itemptr;

  /* 
   * Enclosure Temp Metric
   */

  i_form_frame_start (reqdata->form_out, obj->name_str, "Xserve Enclosure");

  str = i_metric_valstr (item->int_amb_temp, NULL);
  i_form_string_add (reqdata->form_out, "int_amb_temp", "Internal Ambient Temp", str);
  if (str) free (str);

  str = i_metric_valstr (item->sc_amb_temp, NULL);
  i_form_string_add (reqdata->form_out, "sc_amb_temp", "Sys.Ctrl Ambient Temp", str);
  if (str) free (str);

  str = i_metric_valstr (item->sc_int_temp, NULL);
  i_form_string_add (reqdata->form_out, "sc_int_temp", "Sys.Ctrl Internal Temp", str);
  if (str) free (str);

  str = i_metric_valstr (item->memory_temp, NULL);
  i_form_string_add (reqdata->form_out, "memory_temp", "RAM Temperature", str);
  if (str) free (str);

  str = i_metric_valstr (item->pci_temp, NULL);
  i_form_string_add (reqdata->form_out, "pci_temp", "PCI Slot Temp", str);
  if (str) free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}


