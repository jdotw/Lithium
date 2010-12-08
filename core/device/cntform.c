#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
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
#include <induction/str.h>

#include "cntform.h"

/*
 * Generic Container Form 
 */

int l_cntform_generic (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* Get first object */
  i_list_move_head (cnt->obj_list);
  i_object *first_object = i_list_restore (cnt->obj_list);
  if (!first_object)
  {
    item = i_form_string_add (reqdata->form_out, "error", "No objects not present", NULL);
    i_form_frame_end (reqdata->form_out, cnt->name_str);
    return 1;
  }

  /* Establish summary candidates */
  unsigned int max_candidate = 8;
  i_list *candidate_list = i_list_create ();
  i_metric *met;
  for (i_list_move_head(first_object->met_list); (met=i_list_restore(first_object->met_list))!=NULL; i_list_move_next(first_object->met_list))
  {
    /* Check record/trigger/summary */
    if (met->record_enabled == 1 || (met->trg_list && met->trg_list->size > 0) || met->summary_flag == 1)
    { i_list_enqueue (candidate_list, met); }
    if (candidate_list->size >= max_candidate) break;
  }
  if (candidate_list->size < 1)
  { 
    /* Add any metric - maximum 8 */
    for (i_list_move_head(first_object->met_list); (met=i_list_restore(first_object->met_list))!=NULL; i_list_move_next(first_object->met_list))
    { 
      i_list_enqueue (candidate_list, met); 
      if (candidate_list->size >= max_candidate) break;
    }
  }

  /* Determine static/dynamic container */
  if (strstr(first_object->name_str, "master"))
  {
    /* Static */
    i_metric *candidate;
    for (i_list_move_head(candidate_list); (candidate=i_list_restore(candidate_list))!=NULL; i_list_move_next(candidate_list))
    {
      char *value_str = i_metric_valstr (candidate, NULL);
      item = i_form_string_add (reqdata->form_out, candidate->name_str, candidate->desc_str, value_str);
      free (value_str);
    }
  }
  else
  {
    /* Dynamic */
    if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
    {
      char *labels[candidate_list->size+1];
    
      item = i_form_table_create (reqdata->form_out, "objtable", NULL, candidate_list->size + 1);
      if (!item) { i_printf (1, "l_snmp_iface_formsection failed to create table"); return -1; }
      i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
      i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

      unsigned int i;
      i_list_move_head (candidate_list);
      for (i=0; i < candidate_list->size; i++)
      {
        i_metric *met = i_list_restore (candidate_list);
        labels[i+1] = met->desc_str;
        i_list_move_next (candidate_list);
      }
      i_form_table_add_row (item, labels);

      i_object *obj;
      for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
      {
        int row;

        if (obj->desc_str) 
        { labels[0] = obj->desc_str; }
        else
        { labels[0] = obj->name_str; }

        i_list_move_head (candidate_list);
        i = 1;
        for (i=0; i < candidate_list->size; i++)
        {
          i_metric *candidate = i_list_restore (candidate_list);
          i_metric *met = (i_metric *) i_entity_child_get (ENTITY(obj), candidate->name_str);
          labels[i+1] = i_metric_valstr (met, NULL);
          i_list_move_next (candidate_list);
        }
      
        row = i_form_table_add_row (item, labels);
        i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(obj), NULL, 0, NULL, 0);

        for (i=0; i < candidate_list->size; i++)
        {
          if (labels[i+1]) free (labels[i+1]);
        }
      }

      i_form_string_add (reqdata->form_out, "note", "Note", "'*' next to a value indicates the data is not current");
    }
    else
    {
      if (cnt->item_list_state == ITEMLIST_STATE_NONE)
      { item = i_form_string_add (reqdata->form_out, "error", "No objects not present", NULL); }
      else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
      { item = i_form_string_add (reqdata->form_out, "error", "Object list population is in progress", NULL); }
      else
      { item = i_form_string_add (reqdata->form_out, "error", "Object list is in an unknown state", NULL); }
    }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
