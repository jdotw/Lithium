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

#include "nwtput.h"

int l_nwtput_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *str;
  i_form_item *fitem;
  i_object *obj = (i_object *) ent;
  l_nwtput_item *item = obj->itemptr;
  char *labels[3] = { "Metric", "Value", "Last Updated" };

  /* Check entity */
  if (ent->parent != (i_entity *) l_nwtput_cnt())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item"); return 1; }

  /* Start Frame */
  asprintf (&str, "%s Aggregate Throughput - Current Data", obj->desc_str);
  i_form_frame_start (reqdata->form_out, obj->name_str, str);  
  free (str);

  /* Data */
  fitem = i_form_table_create (reqdata->form_out, "datatable", obj->desc_str, 3);
  i_form_item_add_option (fitem, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_item_add_option (fitem, ITEM_OPTION_WIDTH, 1, 0, 0, "120", 4);
  i_form_item_add_option (fitem, ITEM_OPTION_WIDTH, 1, 1, 0, "100", 4);              
  i_form_item_add_option (fitem, ITEM_OPTION_WIDTH, 1, 2, 0, "100", 4);              
  i_form_table_add_row (fitem, labels);

  labels[0] = "Input";
  labels[1] = i_metric_valstr (item->input, NULL);
  labels[2] = i_metric_lastupdatestr (item->input);
  i_form_table_add_row (fitem, labels);
  if (labels[1]) free (labels[1]);
  if (labels[2]) free (labels[2]);

  labels[0] = "Output";
  labels[1] = i_metric_valstr (item->output, NULL);
  labels[2] = i_metric_lastupdatestr (item->output);
  i_form_table_add_row (fitem, labels);
  if (labels[1]) free (labels[1]);
  if (labels[2]) free (labels[2]);

  /* End Frame */
  i_form_frame_end (reqdata->form_out, obj->name_str);

  return 1;
}

int l_nwtput_objform_hist (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_form_item *fitem;
  i_object *obj = (i_object *) ent;
  l_nwtput_item *item = obj->itemptr;

  /* Day Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, item->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);

  /* Week Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, item->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);

  /* Month Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, item->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);

  /* Year Graph */
  fitem = i_form_metcgraph_add (self, reqdata->form_out, item->tput_cg, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);

  return 1;
}
