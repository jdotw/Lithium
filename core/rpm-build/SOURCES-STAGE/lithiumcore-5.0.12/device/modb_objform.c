#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

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
#include <induction/name.h>

#include "snmp.h"
#include "modb.h"

int l_modb_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;

  asprintf (&str, "%s '%s' Current Data", obj->cnt->desc_str, obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  char *met_desc;
  for (i_list_move_head(obj->cnt->wview_metrics); (met_desc=i_list_restore(obj->cnt->wview_metrics))!=NULL; i_list_move_next(obj->cnt->wview_metrics))
  {
    char *met_name = strdup (met_desc);
    i_name_parse (met_name);
    i_metric *met = (i_metric *) i_entity_child_get (ENTITY(obj), met_name);
    free (met_name);

    str = i_metric_valstr (met, NULL);
    i_form_string_add (reqdata->form_out, met->name_str, met->desc_str, str);
    free (str);
  }
  
  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int l_modb_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{

  /*
   * Historic Data 
   */

    i_form_string_add (reqdata->form_out, "FIX", "FIX", "FIX: Not yet implemented");

  /* Day */
  //i_form_metgraph_add (self, reqdata->form_out, proc->load_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
  //i_form_metgraph_add (self, reqdata->form_out, proc->load_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  //i_form_metgraph_add (self, reqdata->form_out, proc->load_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  //i_form_metgraph_add (self, reqdata->form_out, proc->load_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);
  
  return 1;
}
