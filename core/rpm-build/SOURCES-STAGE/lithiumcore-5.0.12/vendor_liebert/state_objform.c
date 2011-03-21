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

#include "state.h"

int v_state_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_state_item *cpu = obj->itemptr;
  i_metric *met;

  /* Check Item */
  if (obj != v_state_obj())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!cpu)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, obj->cnt->name_str, obj->cnt->desc_str);  

  /* 
   * Info Strings 
   */

  for (i_list_move_head(obj->met_list); (met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
  {
    str = i_metric_valstr (met, NULL);
    i_form_string_add (reqdata->form_out, met->name_str, met->desc_str, str);
    if (str) free (str);
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, obj->cnt->name_str);

  return 1;
}

