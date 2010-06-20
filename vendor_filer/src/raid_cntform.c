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
#include <induction/value.h>
#include <induction/str.h>

#include "raid.h"

int v_raid_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * RAID Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Disk Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[6];
    v_raid_item *raid;
    
    item = i_form_table_create (reqdata->form_out, "raidlist", NULL, 6);
    if (!item) { i_printf (1, "v_raid_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 0, 0, "100", 4);
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 1, 0, "80", 3);
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 2, 0, "80", 3);
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 3, 0, "80", 3);
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 4, 0, "80", 3);
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 5, 0, "80", 3);
        

    labels[0] = "Description";
    labels[1] = "Capacity";
    labels[2] = "Available";
    labels[3] = "Used";
    labels[4] = "Percent Used";
    labels[5] = "Status";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (raid=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (raid->obj->desc_str) 
      { labels[0] = raid->obj->desc_str; }
      else
      { labels[0] = raid->obj->name_str; }

      labels[1] = i_metric_valstr (raid->bytes_total, NULL);
      labels[2] = i_metric_valstr (raid->bytes_avail, NULL);
      labels[3] = i_metric_valstr (raid->bytes_used, NULL);
      labels[4] = i_metric_valstr (raid->bytes_used_pc, NULL);
      labels[5] = i_metric_valstr (raid->opstate, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(raid->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);
      free (labels[5]);
    }

    i_form_string_add (reqdata->form_out, "raidlist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "RAID drive list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "RAID drive list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "RAID drive list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
