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
#include "device/snmp.h"

#include "blower.h"

int v_blower_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Blower Resource Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Blower Resource Table 
   */

  char *labels[3];
  v_blower_item *blower;

  item = i_form_table_create (reqdata->form_out, "blowerlist", NULL, 3);
  if (!item) { i_printf (1, "v_blower_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  labels[0] = "Description";
  labels[1] = "Present";
  labels[2] = "RPM";
  i_form_table_add_row (item, labels);

  for (i_list_move_head(cnt->item_list); (blower=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    if (blower->obj->desc_str) 
    { labels[0] = blower->obj->desc_str; }
    else
    { labels[0] = blower->obj->name_str; }

    labels[1] = i_metric_valstr (blower->present, NULL);
    labels[2] = i_metric_valstr (blower->rpm, NULL);
      
    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(blower->obj), NULL, 0, NULL, 0);

    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
  }

  i_form_string_add (reqdata->form_out, "blowerlist_note", "Note", "'*' next to a value indicates the data is not current");

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
