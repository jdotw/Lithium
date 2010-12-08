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

#include "snmp.h"
#include "icmp.h"

int l_icmp_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  /* 
   * ICMP Address Table
   */

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[4];
    l_icmp_item *icmp;
    
    item = i_form_table_create (reqdata->form_out, "icmpitemlist", NULL, 4);
    if (!item) { i_printf (1, "l_icmp_cntform failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "IP Address";
    labels[1] = "Reachable";
    labels[2] = "Response";
    labels[3] = "Record Availability";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (icmp=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      labels[0] = icmp->obj->desc_str;
      labels[1] = i_metric_valstr (icmp->reachable, NULL);
      labels[2] = i_metric_valstr (icmp->response, NULL);
      if (icmp->avail_flag == 1)
      { labels[3] = "Yes"; }
      else
      { labels[3] = "No"; }
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(icmp->obj), NULL, 0, NULL, 0);

      free (labels[1]);
    }

    i_form_string_add (reqdata->form_out, "icmplist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Address list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Address list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Address list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
