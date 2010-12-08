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

#include "ram.h"

int v_ram_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_ram_item *ram = obj->itemptr;

  /* 
   * CPU resource
   */

  asprintf (&str, "RAM Slot '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (ram->size, NULL);
  i_form_string_add (reqdata->form_out, "size", "Size", str);
  free (str);

  str = i_metric_valstr (ram->speed, NULL);
  i_form_string_add (reqdata->form_out, "speed", "Speed", str);
  free (str);

  str = i_metric_valstr (ram->type, NULL);
  i_form_string_add (reqdata->form_out, "type", "Type", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}



