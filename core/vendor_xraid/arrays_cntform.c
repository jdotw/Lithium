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

#include "arrays.h"

int v_arrays_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Arrays Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Arrays Table 
   */
  
  char *labels[6];
  v_arrays_item *array;
    
  item = i_form_table_create (reqdata->form_out, "arraylist", NULL, 6);
  if (!item) { i_printf (1, "v_array_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  labels[0] = "Description";
  labels[1] = "Capacity";
  labels[2] = "Type";
  labels[3] = "RAID Level";
  labels[4] = "Status";
  labels[5] = "Member Count";
  i_form_table_add_row (item, labels);

  for (i_list_move_head(cnt->item_list); (array=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    if (array->obj->desc_str) 
    { labels[0] = array->obj->desc_str; }
    else
    { labels[0] = array->obj->name_str; }

    labels[1] = i_metric_valstr (array->capacity, NULL);
    labels[2] = i_metric_valstr (array->raidtype, NULL);
    labels[3] = i_metric_valstr (array->raidlevel, NULL);
    labels[4] = i_metric_valstr (array->raidstatus, NULL);
    labels[5] = i_metric_valstr (array->membercount, NULL);
      
    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(array->obj), NULL, 0, NULL, 0);

    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
    if (labels[3]) free (labels[3]);
    if (labels[4]) free (labels[4]);
    if (labels[5]) free (labels[5]);
  }

   i_form_string_add (reqdata->form_out, "arraylist_note", "Note", "'*' next to a value indicates the data is not current");

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
