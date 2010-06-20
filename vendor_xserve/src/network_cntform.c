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

#include "osx_server.h"
#include "network.h"

int v_network_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Network Interface ontainer Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * CPU Resource Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[6];
    v_network_item *network;

    if (v_xserve_extras())
    { item = i_form_table_create (reqdata->form_out, "networkalist", NULL, 6); }
    else
    { item = i_form_table_create (reqdata->form_out, "networkalist", NULL, 5); }
    if (!item) { i_printf (1, "v_network_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "IP Address";
    labels[2] = "Subnet Mask";
    labels[3] = "Input Rate";
    labels[4] = "Output Rate";
    labels[5] = "Link State";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (network=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (network->obj->desc_str) 
      { labels[0] = network->obj->desc_str; }
      else
      { labels[0] = network->obj->name_str; }

      labels[1] = i_metric_valstr (network->ip, NULL);
      labels[2] = i_metric_valstr (network->subnetmask, NULL);
      labels[3] = i_metric_valstr (network->bps_in, NULL);
      labels[4] = i_metric_valstr (network->bps_out, NULL);
      if (v_xserve_extras())
      { labels[5] = i_metric_valstr (network->linkstate, NULL); }
      else
      { labels[5] = NULL; }
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(network->obj), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(network->bps_in), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(network->bps_out), NULL, 0, NULL, 0);

      if (labels[1]) free (labels[1]);
      if (labels[2]) free (labels[2]);
      if (labels[3]) free (labels[3]);
      if (labels[4]) free (labels[4]);
      if (labels[5]) free (labels[5]);
    }

    i_form_string_add (reqdata->form_out, "networklist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Interface list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Interface list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Interface list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
