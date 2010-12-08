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

#include "pixconn.h"

int v_pixconn_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * PIX Connections Container Form
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
    char *labels[3];
    v_pixconn_item *pixconn;
    
    item = i_form_table_create (reqdata->form_out, "pixconnlist", NULL, 3);
    if (!item) { i_printf (1, "v_pixconn_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 0, 0, "100", 4);
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 1, 0, "80", 3);
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 1, 2, 0, "80", 3);

    labels[0] = "Service";
    labels[1] = "Current Connections";
    labels[2] = "Highest Connection Count";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (pixconn=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      labels[0] = pixconn->obj->desc_str;
      labels[1] = i_metric_valstr (pixconn->curconn, NULL);
      labels[2] = i_metric_valstr (pixconn->highconn, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(pixconn->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
    }

    i_form_string_add (reqdata->form_out, "pixconnlist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Service list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Service list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Service list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
