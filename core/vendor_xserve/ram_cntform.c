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
#include "device/snmp.h"

#include "ram.h"

int v_ram_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * RAM Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * RAM Table
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[4];
    v_ram_item *ram;
    
    item = i_form_table_create (reqdata->form_out, "ramalist", NULL, 4);
    if (!item) { i_printf (1, "v_ram_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Location";
    labels[1] = "Size";
    labels[2] = "Speed";
    labels[3] = "Type";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (ram=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (ram->obj->desc_str) 
      { labels[0] = ram->obj->desc_str; }
      else
      { labels[0] = ram->obj->name_str; }

      labels[1] = i_metric_valstr (ram->size, NULL);
      labels[2] = i_metric_valstr (ram->speed, NULL);
      labels[3] = i_metric_valstr (ram->type, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(ram->obj), NULL, 0, NULL, 0);

      if (labels[1]) free (labels[1]);
      if (labels[2]) free (labels[2]);
      if (labels[3]) free (labels[3]);
    }

    i_form_string_add (reqdata->form_out, "ramlist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "RAM list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "RAM list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "RAM list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
