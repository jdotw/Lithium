#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>
#include <induction/value.h>

#include "trgform.h"

/* Trigger main form */

int l_trgform_generate (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_trigger *trg = (i_trigger *) ent;
  char *str;
  char *valstr;

  /* 
   * Metric Form
   */

  /* Start Frame */
  asprintf (&str, "Metric '%s' Trigger '%s' - Current Data", trg->met->desc_str, trg->desc_str);
  i_form_frame_start (reqdata->form_out, trg->name_str, str);
  free (str);

  /*
   * Trigger info
   */

  i_form_string_add (reqdata->form_out, "type", "Trigger Type", i_trigger_typestr(trg->trg_type));

  valstr = i_value_valstr (trg->val_type, trg->val, trg->met->unit_str, trg->met->enumstr_list);
  if (trg->trg_type == TRGTYPE_RANGE)
  {
    /* Range triggers */
    char *yvalstr;

    yvalstr = i_value_valstr (trg->val_type, trg->yval, trg->met->unit_str, trg->met->enumstr_list);
    asprintf (&str, "%s to %s", valstr, yvalstr);
    free (yvalstr);
    
    i_form_string_add (reqdata->form_out, "range", "Trigger Range", str);
    free (str);
  }
  else
  {
    /* Other metrics */
    i_form_string_add (reqdata->form_out, "value", "Trigger Value", valstr);
  }
  free (valstr);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, ent->name_str);

  return 1;
}


