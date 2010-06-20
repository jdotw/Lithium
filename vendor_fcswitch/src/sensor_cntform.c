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

#include "sensor.h"

int v_sensor_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Sensor Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Sensor Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[4];
    v_sensor_item *sensor;
    
    item = i_form_table_create (reqdata->form_out, "sensorlist", NULL, 4);
    if (!item) { i_printf (1, "v_sensor_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use prosensorional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "Status";
    labels[2] = "Info";
    labels[3] = "Message";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (sensor=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (sensor->obj->desc_str) 
      { labels[0] = sensor->obj->desc_str; }
      else
      { labels[0] = sensor->obj->name_str; }

      labels[1] = i_metric_valstr (sensor->status, NULL);
      labels[2] = i_metric_valstr (sensor->info, NULL);
      labels[3] = i_metric_valstr (sensor->message, NULL);
      
      row = i_form_table_add_row (item, labels);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
    }

    i_form_string_add (reqdata->form_out, "sensorlist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Sensor list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Sensor list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Sensor list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
