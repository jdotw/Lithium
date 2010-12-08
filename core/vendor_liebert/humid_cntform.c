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

#include "humid.h"

int v_humid_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj;
  v_humid_item *humid;
  i_metric *met;

  obj = v_humid_obj();
  humid = obj->itemptr;

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, obj->cnt->name_str, obj->cnt->desc_str);

  i_form_metcgraph_add (self, reqdata->form_out, humid->humid_cg, GRAPHSIZE_SMALL, 0, GRAPHPERIOD_LAST48HR);
      
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

