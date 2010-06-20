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

#include "client.h"

int v_client_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Memory Pool Container Main Form
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
    char *labels[7];
    v_client_item *client;
    
    item = i_form_table_create (reqdata->form_out, "clientlist", NULL, 7);
    if (!item) { i_printf (1, "v_client_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "MAC Address";
    labels[1] = "Signal Strength";
    labels[2] = "Noise";
    labels[3] = "TX Throughput";
    labels[4] = "RX Throughput";
    labels[5] = "TX Errors";
    labels[6] = "RX Errors";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (client=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (client->obj->desc_str) 
      { labels[0] = client->obj->desc_str; }
      else
      { labels[0] = client->obj->name_str; }

      labels[1] = i_metric_valstr (client->strength, NULL);
      labels[2] = i_metric_valstr (client->noise, NULL);
      labels[3] = i_metric_valstr (client->tx_rate, NULL);
      labels[4] = i_metric_valstr (client->rx_rate, NULL);
      labels[5] = i_metric_valstr (client->tx_eps, NULL);
      labels[6] = i_metric_valstr (client->rx_eps, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(client->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);
      free (labels[5]);
      free (labels[6]);
    }

    i_form_string_add (reqdata->form_out, "clientlist_note", "Note", "'*' next to a value indicates the data is not current");
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
