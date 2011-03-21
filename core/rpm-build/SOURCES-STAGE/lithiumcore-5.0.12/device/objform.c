#include <stdlib.h>
#include <stdio.h>
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
#include <induction/interface.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "objform.h"

/*
 * Generic Object Formn
 */

int l_objform_generic (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;

  asprintf (&str, "%s '%s' Current Data", obj->cnt->desc_str, obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);
  
  i_metric *met;
  for (i_list_move_head(obj->met_list); (met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
  {
    str = i_metric_valstr (met, NULL);
    i_form_string_add (reqdata->form_out, met->name_str, met->desc_str, str);
    free (str);
  }

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

