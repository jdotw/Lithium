#include <stdio.h>
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

#include "avail.h"

int l_avail_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_form_item *fitem;
  i_object *obj = (i_object *) ent;
  l_avail_item *availitem = obj->itemptr;

  /* Check entity */
  if (ent->parent != (i_entity *) l_avail_cnt())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item"); return 1; }

  /* Start Frame */
  asprintf (&str, "%s Availability - Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);  
  free (str);

  /* Last 24Hr Graphs */
  if (availitem->avail_cg)
  { fitem = i_form_metcgraph_add (self, reqdata->form_out, availitem->avail_cg, GRAPHSIZE_MEDIUM, 0, GRAPHPERIOD_LAST48HR); }
  if (availitem->resptime)
  { fitem = i_form_metgraph_add (self, reqdata->form_out, availitem->resptime, GRAPHSIZE_MEDIUM, 0, GRAPHPERIOD_LAST48HR); }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int l_avail_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_form_item *fitem;
  i_object *obj = (i_object *) ent;
  l_avail_item *availitem = obj->itemptr;

  /* Day Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, availitem->avail_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  fitem = i_form_metgraph_add (self, reqdata->form_out, availitem->resptime, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  i_form_spacer_add (reqdata->form_out);

  /* Week Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, availitem->avail_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  fitem = i_form_metgraph_add (self, reqdata->form_out, availitem->resptime, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  i_form_spacer_add (reqdata->form_out);

  /* Month Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, availitem->avail_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  fitem = i_form_metgraph_add (self, reqdata->form_out, availitem->resptime, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  i_form_spacer_add (reqdata->form_out);

  /* Year Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, availitem->avail_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  fitem = i_form_metgraph_add (self, reqdata->form_out, availitem->resptime, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  i_form_spacer_add (reqdata->form_out);

  return 1;
}
