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

#include "state.h"

int v_state_sumform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj;
  i_container *cnt = (i_container *) ent;

  obj = v_state_obj ();
  if (obj)
  {
    i_form_item *item;
    v_state_item *state = obj->itemptr;

    i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
    item = i_form_metcgraph_add (self, reqdata->form_out, state->state_cg, GRAPHSIZE_SMALL, 0, GRAPHPERIOD_LAST48HR);
    i_form_frame_end (reqdata->form_out, cnt->name_str);
  }

  return 1;
}

