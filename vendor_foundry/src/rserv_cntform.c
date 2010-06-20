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

#include "rserv.h"
#include "rport.h"

int v_rserv_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item;
  i_form_item *table;
  v_rserv_item *rserv;

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    /* 
     * Real Server List
     */
    
    table = v_rserv_cntform_servlist (self, reqdata, cnt->item_list, NULL);
    if (table)
    { i_form_string_add (reqdata->form_out, "current_note", "Note", "'*' next to a value indicates the data is not current"); }
    i_form_spacer_add (reqdata->form_out);

    /* 
     * Real Port List 
     */

    table = NULL;
    for (i_list_move_head(cnt->item_list); (rserv=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      if (rserv->rport_cnt && rserv->rport_cnt->item_list)
      { table = v_rport_cntform_portlist (self, reqdata, rserv->rport_cnt->item_list, table); }
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

i_form_item* v_rserv_cntform_servlist (i_resource *self, i_form_reqdata *reqdata, i_list *item_list, i_form_item *table)
{
  char *labels[9] = { "Description", "IP Address", "Admin State", "Op State", "Current Conns", "Peak Conns", "Conn. Rate", "Conn. Retention", "Reassign Rate" };
  v_rserv_item *rserv;

  table = i_form_table_create (reqdata->form_out, "rservlist", "Server Statistics", 9);
  if (!table) { i_printf (1, "v_rserv_cntform failed to create table"); return NULL; }
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "100", 4);
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "80", 3);

  i_form_table_add_row (table, labels);

  for (i_list_move_head(item_list); (rserv=i_list_restore(item_list))!=NULL; i_list_move_next(item_list))
  {
    int row;

    if (rserv->obj->desc_str)
    { labels[0] = rserv->obj->desc_str; }
    else
    { labels[0] = rserv->obj->name_str; }
    labels[1] = i_metric_valstr (rserv->ipaddr, NULL);
    labels[2] = i_metric_valstr (rserv->adminstate, NULL);
    labels[3] = i_metric_valstr (rserv->opstate, NULL);
    labels[4] = i_metric_valstr (rserv->curconn, NULL);
    labels[5] = i_metric_valstr (rserv->peakconn, NULL);
    labels[6] = i_metric_valstr (rserv->connps, NULL);
    labels[7] = i_metric_valstr (rserv->connretainpc, NULL);
    labels[8] = i_metric_valstr (rserv->reassignps, NULL);

    row = i_form_table_add_row (table, labels);

    i_form_table_add_link (table, 0, row, RES_ADDR(self), ENT_ADDR(rserv->obj), NULL, 0, NULL, 0);

    free (labels[1]);
    free (labels[2]);
    free (labels[3]);
    free (labels[4]);
    free (labels[5]);
    free (labels[6]);
    free (labels[7]);
    free (labels[8]);
  }

  return table;
}
