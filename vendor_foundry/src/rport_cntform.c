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

int v_rport_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  /* 
   * Real port table (per server) 
   */

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    v_rport_cntform_portlist (self, reqdata, cnt->item_list, NULL);
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

i_form_item* v_rport_cntform_portlist (i_resource *self, i_form_reqdata *reqdata, i_list *item_list, i_form_item *table)
{
  char *labels[9] = { "Real Server", "Port", "Admin State", "Op State", "Current Conns", "Peak Conns", "Conn. Rate", "Conn. Retention", "Reassign Rate" };
  v_rport_item *rport;

  if (!table)
  {
    table = i_form_table_create (reqdata->form_out, "portlist", "Port Statistics", 9);
    if (!table) { i_printf (1, "v_rport_cntform failed to create table"); return NULL; }
//    i_form_item_add_option (table, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "100", 4);
    i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "80", 3);

    i_form_table_add_row (table, labels);
  }

  for (i_list_move_head(item_list); (rport=i_list_restore(item_list))!=NULL; i_list_move_next(item_list))
  {
    int row;

    labels[0] = i_metric_valstr (rport->rservname, NULL);
    if (rport->obj->desc_str)
    { labels[1] = rport->obj->desc_str; }
    else
    { labels[1] = rport->obj->name_str; }
    labels[2] = i_metric_valstr (rport->adminstate, NULL);
    labels[3] = i_metric_valstr (rport->opstate, NULL);
    labels[4] = i_metric_valstr (rport->curconn, NULL);
    labels[5] = i_metric_valstr (rport->peakconn, NULL);
    labels[6] = i_metric_valstr (rport->connps, NULL);
    labels[7] = i_metric_valstr (rport->connretainpc, NULL);
    labels[8] = i_metric_valstr (rport->reassignps, NULL);

    row = i_form_table_add_row (table, labels);

    if (labels[0] && strlen(labels[0]) > 0)
    {
      i_container *rserv_cnt = v_rserv_cnt ();
      i_object *rserv_obj;
      
      rserv_obj = (i_object *) i_entity_child_get (ENTITY(rserv_cnt), labels[0]);
      if (rserv_obj)
      { i_form_table_add_link (table, 0, row, RES_ADDR(self), ENT_ADDR(rserv_obj), NULL, 0, NULL, 0); }
    }

    i_form_table_add_link (table, 1, row, RES_ADDR(self), ENT_ADDR(rport->obj), NULL, 0, NULL, 0);

    free (labels[0]);
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
