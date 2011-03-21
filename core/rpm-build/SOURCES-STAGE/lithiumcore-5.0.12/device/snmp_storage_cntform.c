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
#include "snmp_storage.h"

int l_snmp_storage_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  /* 
   * Storage Table
   */

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[5];
    l_snmp_storage_item *store;
    
    item = i_form_table_create (reqdata->form_out, "storagelist", NULL, 5);
    if (!item) { i_printf (1, "l_snmp_storage_cntform failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "Size";
    labels[2] = "Used";
    labels[3] = "Free";
    labels[4] = "Used %";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (store=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      labels[0] = store->obj->desc_str;
      labels[1] = i_metric_valstr (store->size, NULL);
      labels[2] = i_metric_valstr (store->used, NULL);
      labels[3] = i_metric_valstr (store->free, NULL);
      labels[4] = i_metric_valstr (store->used_pc, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(store->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);
    }

    i_form_string_add (reqdata->form_out, "storagelist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Storage Resource list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Storage Resource list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Storage Resource list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
