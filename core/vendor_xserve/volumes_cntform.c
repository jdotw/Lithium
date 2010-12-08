#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <lithium/snmp.h>

#include "volumes.h"

int v_volumes_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Volumes Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Volumes Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[4];
    v_volumes_item *volume;
    
    item = i_form_table_create (reqdata->form_out, "volumelist", NULL, 4);
    if (!item) { i_printf (1, "v_volume_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "Size";
    labels[2] = "Free";
    labels[3] = "Used (%)";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (volume=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (volume->obj->desc_str) 
      { labels[0] = volume->obj->desc_str; }
      else
      { labels[0] = volume->obj->name_str; }

      labels[1] = i_metric_valstr (volume->bytes_total, NULL);
      labels[2] = i_metric_valstr (volume->bytes_free, NULL);
      labels[3] = i_metric_valstr (volume->used_pc, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(volume->obj), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(volume->bytes_free), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(volume->used_pc), NULL, 0, NULL, 0);

      if (labels[1]) free (labels[1]);
      if (labels[2]) free (labels[2]);
      if (labels[3]) free (labels[3]);
    }

    i_form_string_add (reqdata->form_out, "volumelist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Volumes list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Volumes list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Volumes list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
