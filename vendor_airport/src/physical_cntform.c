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

#include "physical.h"

int v_physical_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Physical Interface Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Memory Pool Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[8];
    v_physical_item *physical;
    
    item = i_form_table_create (reqdata->form_out, "physicallist", NULL, 8);
    if (!item) { i_printf (1, "v_physical_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "State";
    labels[2] = "Speed";
    labels[3] = "Duplex";
    labels[4] = "TX Throughput";
    labels[5] = "RX Throughput";
    labels[6] = "TX Errors";
    labels[7] = "RX Errors";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (physical=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (physical->obj->desc_str) 
      { labels[0] = physical->obj->desc_str; }
      else
      { labels[0] = physical->obj->name_str; }

      labels[1] = i_metric_valstr (physical->state, NULL);
      labels[2] = i_metric_valstr (physical->speed, NULL);
      labels[3] = i_metric_valstr (physical->duplex, NULL);
      labels[4] = i_metric_valstr (physical->tx_rate, NULL);
      labels[5] = i_metric_valstr (physical->rx_rate, NULL);
      labels[6] = i_metric_valstr (physical->tx_eps, NULL);
      labels[7] = i_metric_valstr (physical->rx_eps, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(physical->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);
      free (labels[5]);
      free (labels[6]);
      free (labels[7]);
    }

    i_form_string_add (reqdata->form_out, "physicallist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Client list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Client list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Client list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
