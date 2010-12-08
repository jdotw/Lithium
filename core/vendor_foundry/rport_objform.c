#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/form.h"
#include "induction/str.h"

#include "rserv.h"
#include "rport.h"

int v_rport_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  char *rserv_str;
  i_object *obj = (i_object *) ent;
  v_rport_item *rport = obj->itemptr;
  i_form_item *item;
  i_list *list;

  if (v_rport_cnt_validate(obj->cnt) != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!rport)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /* Start Frame */
  rserv_str = i_metric_valstr (rport->rservname, NULL);
  asprintf (&str, "%s Real Port %s - Current Data", rserv_str, ent->desc_str);
  free (rserv_str);
  i_form_frame_start (reqdata->form_out, ent->name_str, str);
  free (str);

  /* Port Stats */
  list = i_list_create ();
  i_list_enqueue (list, rport);
  item = v_rport_cntform_portlist (self, reqdata, list, NULL);
  i_list_free (list);
  if (item)
  { i_form_string_add (reqdata->form_out, "current_note", "Note", "'*' next to a value indicates the data is not current"); }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, ent->name_str);

  return 1;
}

int v_rport_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_rport_item *rport = obj->itemptr;

  /* Check Item */
  if (v_rport_cnt_validate(obj->cnt) != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!rport)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, rport->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, rport->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, rport->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, rport->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}

