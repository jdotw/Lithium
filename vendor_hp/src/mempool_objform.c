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
#include <induction/form.h>
#include <induction/str.h>

#include "mempool.h"

int v_mempool_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_object *obj = (i_object *) ent;
  v_mempool_item *pool = obj->itemptr;

  /* 
   * Storage Resource
   */

  asprintf (&str, "Memory: '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (pool->local_slab, NULL);
  i_form_string_add (reqdata->form_out, "local_slab", "Local Memory Size", str);
  free (str);

  str = i_metric_valstr (pool->local_used, NULL);
  i_form_string_add (reqdata->form_out, "local_used", "Local Memory Used", str);
  free (str);

  str = i_metric_valstr (pool->local_free, NULL);
  i_form_string_add (reqdata->form_out, "local_free", "Local Memory Free", str);
  free (str);

  str = i_metric_valstr (pool->local_used_pc, NULL);
  i_form_string_add (reqdata->form_out, "local_used_pc", "Local Memory Used (%)", str);
  free (str);
  
  str = i_metric_valstr (pool->global_size, NULL);
  i_form_string_add (reqdata->form_out, "global_size", "Global Memory Size", str);
  free (str);
  
  str = i_metric_valstr (pool->global_used, NULL);
  i_form_string_add (reqdata->form_out, "global_used", "Global Memory Used", str);
  free (str);

  str = i_metric_valstr (pool->global_free, NULL);
  i_form_string_add (reqdata->form_out, "global_free", "Global Memory Free", str);
  free (str);
 
  str = i_metric_valstr (pool->global_used_pc, NULL);
  i_form_string_add (reqdata->form_out, "global_used_pc", "Glogab Memory Used (%)", str);
  free (str);


  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_mempool_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{

  /*
   * Historic Data 
   */

  /* Day */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
//  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



