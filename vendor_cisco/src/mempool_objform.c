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

  asprintf (&str, "Memory Pool '%s' Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);
  free (str);

  i_form_string_add (reqdata->form_out, "descr", "Description", obj->desc_str);

  str = i_metric_valstr (pool->size, NULL);
  i_form_string_add (reqdata->form_out, "size", "Size", str);
  free (str);

  str = i_metric_valstr (pool->used, NULL);
  i_form_string_add (reqdata->form_out, "used", "Used", str);
  free (str);

  str = i_metric_valstr (pool->free, NULL);
  i_form_string_add (reqdata->form_out, "free", "Free", str);
  free (str);

  str = i_metric_valstr (pool->largest_free, NULL);
  i_form_string_add (reqdata->form_out, "largest_free", "Largest Free Block", str);
  free (str);

  str = i_metric_valstr (pool->used_pc, NULL);
  i_form_string_add (reqdata->form_out, "used_pc", "Used (%)", str);
  free (str);

  str = i_metric_valstr (pool->contig_free_pc, NULL);
  i_form_string_add (reqdata->form_out, "contig_free_pc", "Contiguous Free (%)", str);
  free (str);

  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int v_mempool_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj = (i_object *) ent;
  v_mempool_item *pool = obj->itemptr;

  /*
   * Historic Data 
   */

  /* Day */
  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY); 
  /* Week */
  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);
  /* Month */
  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);
  /* Year */
  i_form_metgraph_add (self, reqdata->form_out, pool->used_pc, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}



