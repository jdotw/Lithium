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

int v_rserv_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_list *list;
  i_form_item *item;
  i_object *obj = (i_object *) ent;
  v_rserv_item *rserv = obj->itemptr;

  /* Check Item */
  if (obj->cnt != v_rserv_cnt())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!rserv)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /* Start Frame */
  asprintf (&str, "Real Server %s - Current Data", ent->desc_str);
  i_form_frame_start (reqdata->form_out, ent->name_str, str);
  free (str);

  /* 
   * Info Strings 
   */

  if (ent->desc_str)
  { i_form_string_add (reqdata->form_out, "desc", "Description", ent->desc_str); }
  else
  { i_form_string_add (reqdata->form_out, "desc", "Description", ent->name_str); }

  str = i_metric_valstr (rserv->ipaddr, NULL);
  i_form_string_add (reqdata->form_out, "ipaddr", "IP Address", str);
  if (str) free (str);

  str = i_metric_valstr (rserv->weight, NULL);
  i_form_string_add (reqdata->form_out, "weight", "Weight", str);
  if (str) free (str);

  i_form_spacer_add (reqdata->form_out);

  /* Server Stats */
  list = i_list_create ();
  i_list_enqueue (list, rserv);
  item = v_rserv_cntform_servlist (self, reqdata, list, NULL);
  i_list_free (list);
  if (item)
  { i_form_string_add (reqdata->form_out, "current_note", "Note", "'*' next to a value indicates the data is not current"); }
  i_form_spacer_add (reqdata->form_out);

  /* Port Stats */
  if (rserv->rport_cnt && rserv->rport_cnt->item_list)
  { 
    item = v_rport_cntform_portlist (self, reqdata, rserv->rport_cnt->item_list, NULL);
    if (item)
    { i_form_string_add (reqdata->form_out, "current_note", "Note", "'*' next to a value indicates the data is not current"); }
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, ent->name_str);

  return 1;
}

int v_rserv_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_rserv_item *rserv = obj->itemptr;

  /* Check Item */
  if (obj->cnt != v_rserv_cnt())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }
  if (!rserv)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No item present."); return 1; }

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metcgraph_add (self, reqdata->form_out, rserv->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);
  /* Week */
  i_form_metcgraph_add (self, reqdata->form_out, rserv->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metcgraph_add (self, reqdata->form_out, rserv->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metcgraph_add (self, reqdata->form_out, rserv->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}
