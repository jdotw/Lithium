#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/device.h>
#include <induction/inventory.h>
#include <induction/construct.h>
#include <induction/hierarchy.h>
#include <induction/vendor.h>

#include "metform.h"

/* Metric main form */

int l_metform_generate (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_metric *met = (i_metric *) ent;
  i_form_item *table;
  char *str;
  char *vllabels[2] = { "Timestamp", "Value" };

  /* 
   * Metric Form
   */

  /* Start Frame */
  asprintf (&str, "Metric '%s' - Current Data", ent->desc_str);
  i_form_frame_start (reqdata->form_out, ent->name_str, str);
  free (str);

  /*
   * Recent Values 
   */

  /* Create Table */  
  table = i_form_table_create (reqdata->form_out, "vallist_table", "Recent Values", 2);
  if (!table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create vallist table"); return 1; }
  i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  /* Add header row */
  i_form_table_add_row (table, vllabels);

  /* Set labels */
  if (met->val_list && met->val_list->size > 0)
  {
    i_metric_value *val;

    for (i_list_move_head(met->val_list); (val=i_list_restore(met->val_list))!=NULL; i_list_move_next(met->val_list))
    {
      vllabels[0] = i_time_ctime (val->tstamp.tv_sec);
      vllabels[1] = i_metric_valstr (met, val);
      i_form_table_add_row (table, vllabels);
      if (vllabels[0]) free (vllabels[0]);
      if (vllabels[1]) free (vllabels[1]);
    }
  }
  else
  {
    vllabels[0] = "";
    vllabels[1] = "No values present";
    i_form_table_add_row (table, vllabels);
  }

  /* End Frame */
  i_form_frame_end (reqdata->form_out, ent->name_str);

  return 1;
}

int l_metform_hist_generate (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_form_item *item;
  i_metric *met = (i_metric *) ent;

  if (met->record_method > 0)
  {    
    /*
     * RRD Recorded Data 
     */

    if (met->record_method & RECMETHOD_RRD)
    {
      /* Day */
      item = i_form_metgraph_add (self, reqdata->form_out, met, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_DAY);      
      /* Week */
      item = i_form_metgraph_add (self, reqdata->form_out, met, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_WEEK);      
      /* Month */
      item = i_form_metgraph_add (self, reqdata->form_out, met, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_MONTH);      
      /* Year */
      item = i_form_metgraph_add (self, reqdata->form_out, met, GRAPHSIZE_MEDIUM, reqdata->ref_sec, GRAPHPERIOD_YEAR);      
    }
  }

  return 1;
}

