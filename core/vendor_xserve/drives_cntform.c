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

#include "drives.h"

int v_drives_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Drives Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Volumes Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[7];
    v_drives_item *drive;
    
    item = i_form_table_create (reqdata->form_out, "drivelist", NULL, 7);
    if (!item) { i_printf (1, "v_drive_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "Capacity";
    labels[2] = "Disk ID";
    labels[3] = "Vendor";
    labels[4] = "Read Throughput";
    labels[5] = "Write Throughput";
    labels[6] = "SMART";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (drive=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (drive->obj->desc_str) 
      { labels[0] = drive->obj->desc_str; }
      else
      { labels[0] = drive->obj->name_str; }

      labels[1] = i_metric_valstr (drive->capacity, NULL);
      labels[2] = i_metric_valstr (drive->diskid, NULL);
      labels[3] = i_metric_valstr (drive->vendor, NULL);
      labels[4] = i_metric_valstr (drive->read_bps, NULL);
      labels[5] = i_metric_valstr (drive->write_bps, NULL);
      labels[6] = i_metric_valstr (drive->smart, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(drive->obj), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(drive->read_bps), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 5, row, RES_ADDR(self), ENT_ADDR(drive->write_bps), NULL, 0, NULL, 0);

      if (labels[1]) free (labels[1]);
      if (labels[2]) free (labels[2]);
      if (labels[3]) free (labels[3]);
      if (labels[4]) free (labels[4]);
      if (labels[5]) free (labels[5]);
      if (labels[6]) free (labels[6]);
    }

    i_form_string_add (reqdata->form_out, "drivelist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Drives list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Drives list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Drives list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
