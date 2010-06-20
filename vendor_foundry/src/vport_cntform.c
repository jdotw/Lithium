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

#include "vserv.h"
#include "vport.h"

int v_vport_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  /* 
   * Virtual port table (per server) 
   */

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    v_vport_cntform_portlist (self, reqdata, cnt->item_list, NULL);
    i_form_string_add (reqdata->form_out, "current_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Port list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Port list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Port list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}

i_form_item* v_vport_cntform_portlist (i_resource *self, i_form_reqdata *reqdata, i_list *item_list, i_form_item *table)
{
  char *labels[7] = { "Virtual Server", "Port", "Admin State", "Current Conns", "Peak Conns", "Conn. Rate", "Conn. Retention" };
  v_vport_item *vport;

  if (!table)
  {
    table = i_form_table_create (reqdata->form_out, "portlist", "Port Statistics", 7);
    if (!table) { i_printf (1, "v_vport_cntform failed to create table"); return NULL; }
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "100", 4);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "80", 3);

    i_form_table_add_row (table, labels);
  }

  for (i_list_move_head(item_list); (vport=i_list_restore(item_list))!=NULL; i_list_move_next(item_list))
  {
    int row;

    labels[0] = i_metric_valstr (vport->vservname, NULL);
    if (vport->obj->desc_str)
    { labels[1] = vport->obj->desc_str; }
    else
    { labels[1] = vport->obj->name_str; }
    labels[2] = i_metric_valstr (vport->adminstate, NULL);
    labels[3] = i_metric_valstr (vport->curconn, NULL);
    labels[4] = i_metric_valstr (vport->peakconn, NULL);
    labels[5] = i_metric_valstr (vport->connps, NULL);
    labels[6] = i_metric_valstr (vport->connretainpc, NULL);

    row = i_form_table_add_row (table, labels);

    if (labels[0] && strlen(labels[0]) > 0)
    {
      i_container *vserv_cnt = v_vserv_cnt ();
      i_object *vserv_obj;
      
      vserv_obj = (i_object *) i_entity_child_get (ENTITY(vserv_cnt), labels[0]);
      if (vserv_obj)
      { i_form_table_add_link (table, 0, row, RES_ADDR(self), ENT_ADDR(vserv_obj), NULL, 0, NULL, 0); }
    }

    i_form_table_add_link (table, 1, row, RES_ADDR(self), ENT_ADDR(vport->obj), NULL, 0, NULL, 0);

    free (labels[0]);
    free (labels[2]);
    free (labels[3]);
    free (labels[4]);
    free (labels[5]);
    free (labels[6]);
  }

  return table;
}
