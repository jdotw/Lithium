#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

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
#include <induction/name.h>

#include "snmp.h"
#include "modb.h"

int l_modb_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  /*
   * Table of values 
   */
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[cnt->wview_metrics->size+1];
    
    item = i_form_table_create (reqdata->form_out, "table", NULL, cnt->wview_metrics->size+1);
    if (!item) { i_printf (1, "l_snmp_iface_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    unsigned int i = 1;
    char *met_desc;
    labels[0] = "Object";
    for (i_list_move_head(cnt->wview_metrics); (met_desc=i_list_restore(cnt->wview_metrics))!=NULL; i_list_move_next(cnt->wview_metrics))
    {
      labels[i] = met_desc;
      i++;
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

      i = 1;
      for (i_list_move_head(cnt->wview_metrics); (met_desc=i_list_restore(cnt->wview_metrics))!=NULL; i_list_move_next(cnt->wview_metrics))
      {
        char *met_name = strdup (met_desc);
        i_name_parse (met_name);
        i_metric *met = (i_metric *) i_entity_child_get (ENTITY(obj), met_name);
        free (met_name);

        labels[i] = i_metric_valstr (met, NULL);

        i++;
      }
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(obj), NULL, 0, NULL, 0);

      for (i=1; i < (cnt->wview_metrics->size+1); i++)
      { free (labels[i]); }
    }

    i_form_string_add (reqdata->form_out, "modb_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Object list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Object list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Object list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
