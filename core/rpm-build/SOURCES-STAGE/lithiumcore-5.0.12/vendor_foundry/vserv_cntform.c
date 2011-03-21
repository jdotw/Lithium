#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"

#include "vserv.h"
#include "vport.h"

int v_vserv_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item;
  i_form_item *table;
  v_vserv_item *vserv;

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    /* 
     * Virtual Server List
     */
    
    table = v_vserv_cntform_servlist (self, reqdata, cnt->item_list, NULL);
    if (table)
    { i_form_string_add (reqdata->form_out, "current_note", "Note", "'*' next to a value indicates the data is not current"); }
    i_form_spacer_add (reqdata->form_out);

    /* 
     * Virtual Port List 
     */

    table = NULL;
    for (i_list_move_head(cnt->item_list); (vserv=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      if (vserv->vport_cnt && vserv->vport_cnt->item_list)
      { table = v_vport_cntform_portlist (self, reqdata, vserv->vport_cnt->item_list, table); }
    }
    if (table)
    { i_form_string_add (reqdata->form_out, "current_note", "Note", "'*' next to a value indicates the data is not current"); }
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Server list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Server list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Server list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}

i_form_item* v_vserv_cntform_servlist (i_resource *self, i_form_reqdata *reqdata, i_list *item_list, i_form_item *table)
{
  char *labels[6] = { "Description", "IP Address", "Admin State", "Symmetric State", "SDA Type", "Conn. Rate" };
  v_vserv_item *vserv;

  table = i_form_table_create (reqdata->form_out, "vservlist", "Server Statistics", 6);
  if (!table) { i_printf (1, "v_vserv_cntform failed to create table"); return NULL; }
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "100", 4);
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "80", 3);

  i_form_table_add_row (table, labels);

  for (i_list_move_head(item_list); (vserv=i_list_restore(item_list))!=NULL; i_list_move_next(item_list))
  {
    int row;
    
    if (vserv->obj->desc_str)
    { labels[0] = vserv->obj->desc_str; }
    else
    { labels[0] = vserv->obj->name_str; }
    labels[1] = i_metric_valstr (vserv->ipaddr, NULL);
    labels[2] = i_metric_valstr (vserv->adminstate, NULL);
    labels[3] = i_metric_valstr (vserv->symstate, NULL);
    labels[4] = i_metric_valstr (vserv->sdatype, NULL);
    labels[5] = i_metric_valstr (vserv->connps, NULL);

    row = i_form_table_add_row (table, labels);

    i_form_table_add_link (table, 0, row, RES_ADDR(self), ENT_ADDR(vserv->obj), NULL, 0, NULL, 0);

    free (labels[1]);
    free (labels[2]);
    free (labels[3]);
    free (labels[4]);
    free (labels[5]);
  }

  return table;
}
