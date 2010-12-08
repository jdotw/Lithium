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

#include "osx_server.h"
#include "psu.h"

int v_psu_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * PSU Container Main Form
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
    char *labels[7];
    v_psu_item *psu;

    item = i_form_table_create (reqdata->form_out, "psulist", NULL, 7);
    if (!item) { i_printf (1, "v_psu_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "12v";
    labels[2] = "5v Standby";
    labels[3] = "Power";
    labels[4] = "Exhaust Temp";
    labels[5] = "Fan Inlet RPM";
    labels[6] = "Fan Outlet RPM";

    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (psu=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (psu->obj->desc_str) 
      { labels[0] = psu->obj->desc_str; }
      else
      { labels[0] = psu->obj->name_str; }

      labels[1] = i_metric_valstr (psu->v_12v, NULL);
      labels[2] = i_metric_valstr (psu->v_5v_standby, NULL);
      labels[3] = i_metric_valstr (psu->power, NULL);
      labels[4] = i_metric_valstr (psu->exhaust_temp, NULL);
      labels[5] = i_metric_valstr (psu->fan_in_rpm, NULL);
      labels[6] = i_metric_valstr (psu->fan_out_rpm, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(psu->obj), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(psu->v_12v), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(psu->v_5v_standby), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(psu->power), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(psu->exhaust_temp), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 5, row, RES_ADDR(self), ENT_ADDR(psu->fan_in_rpm), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 6, row, RES_ADDR(self), ENT_ADDR(psu->fan_out_rpm), NULL, 0, NULL, 0);

      if (labels[1]) free (labels[1]);
      if (labels[2]) free (labels[2]);
      if (labels[3]) free (labels[3]);
      if (labels[4]) free (labels[4]);
      if (labels[5]) free (labels[5]);
      if (labels[6]) free (labels[6]);
    }

    i_form_string_add (reqdata->form_out, "psulist_note", "Note", "'*' next to a value indicates the data is not current");
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
