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
#include <lithium/snmp.h>

#include "status.h"

int v_status_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Controller Status Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * First table
   */

  char *labels[7];
  v_status_item *status;

  item = i_form_table_create (reqdata->form_out, "statuslist_1", NULL, 7);
  if (!item) { i_printf (1, "v_status_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  labels[0] = "Controller";
  labels[1] = "Power";
  labels[2] = "Buzzer";
  labels[3] = "Service ID";
  labels[4] = "Line Power Down";
  labels[5] = "Battery In Use";
  labels[6] = "Battery Low";
  i_form_table_add_row (item, labels);

  for (i_list_move_head(cnt->item_list); (status=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    if (status->obj->desc_str) 
    { labels[0] = status->obj->desc_str; }
    else
    { labels[0] = status->obj->name_str; }

    labels[1] = i_metric_valstr (status->powerstate, NULL);
    labels[2] = i_metric_valstr (status->buzzer, NULL);
    labels[3] = i_metric_valstr (status->serviceid, NULL);
    labels[4] = i_metric_valstr (status->linepowerdown, NULL);
    labels[5] = i_metric_valstr (status->batteryinuse, NULL);
    labels[6] = i_metric_valstr (status->batterylow, NULL);
      
    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(status->obj), NULL, 0, NULL, 0);

    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
    if (labels[3]) free (labels[3]);
    if (labels[4]) free (labels[4]);
    if (labels[5]) free (labels[5]);
    if (labels[6]) free (labels[6]);
  }

  item = i_form_table_create (reqdata->form_out, "statuslist_1", NULL, 7);
  if (!item) { i_printf (1, "v_status_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  labels[0] = "Controller";
  labels[1] = "RAID Comms";
  labels[2] = "RAID Controller";
  labels[3] = "Blower";
  labels[4] = "Blower Speed OK";
  labels[5] = "SMART";
  labels[6] = "Battery";
  i_form_table_add_row (item, labels);
    
  for (i_list_move_head(cnt->item_list); (status=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    if (status->obj->desc_str)  
    { labels[0] = status->obj->desc_str; }
    else
    { labels[0] = status->obj->name_str; }

    labels[1] = i_metric_valstr (status->raidcomms, NULL);
    labels[2] = i_metric_valstr (status->raidcont_state, NULL);
    labels[3] = i_metric_valstr (status->blower, NULL);
    labels[4] = i_metric_valstr (status->blowerspeed, NULL);
    labels[5] = i_metric_valstr (status->smart, NULL);
    labels[6] = i_metric_valstr (status->battery, NULL);

    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(status->obj), NULL, 0, NULL, 0);

    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
    if (labels[3]) free (labels[3]);
    if (labels[4]) free (labels[4]);
    if (labels[5]) free (labels[5]);
    if (labels[6]) free (labels[6]);
  }

  item = i_form_table_create (reqdata->form_out, "statuslist_2", NULL, 7);
  if (!item) { i_printf (1, "v_status_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  labels[0] = "Controller";
  labels[1] = "Ambient Temp";
  labels[2] = "Ambient";
  labels[3] = "Drive Temp";
  labels[4] = "Blower Temp";
  labels[5] = "EMU Temp";
  labels[6] = "RAID Temp";
  i_form_table_add_row (item, labels);

  for (i_list_move_head(cnt->item_list); (status=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    if (status->obj->desc_str)
    { labels[0] = status->obj->desc_str; }
    else
    { labels[0] = status->obj->name_str; }

    labels[1] = i_metric_valstr (status->ambient_temp, NULL);
    labels[2] = i_metric_valstr (status->ambientstate, NULL);
    labels[3] = i_metric_valstr (status->drivetemp, NULL);
    labels[4] = i_metric_valstr (status->blowertemp, NULL);
    labels[5] = i_metric_valstr (status->emutemp, NULL);
    labels[6] = i_metric_valstr (status->raidtemp, NULL);

    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(status->obj), NULL, 0, NULL, 0);

    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
    if (labels[3]) free (labels[3]);
    if (labels[4]) free (labels[4]);
    if (labels[5]) free (labels[5]);
    if (labels[6]) free (labels[6]);
  }

  
  i_form_string_add (reqdata->form_out, "statuslist_note", "Note", "'*' next to a value indicates the data is not current");

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
