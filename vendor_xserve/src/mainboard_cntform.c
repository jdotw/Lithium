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

#include "osx_server.h"
#include "mainboard.h"

int v_mainboard_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Mainboard Resource Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Mainboard Resource Table 
   */

  char *labels[7];
  v_mainboard_item *mainboard;

  item = i_form_table_create (reqdata->form_out, "mainboardlist", NULL, 6);
  if (!item) { i_printf (1, "v_mainboard_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  labels[0] = "Main 12v";
  labels[1] = "Main 3.3v";
  labels[2] = "Standby";
  labels[3] = "FBDIMM 12v VRM";
  labels[4] = "NS Bridge Power";
  labels[5] = "North Bridge Temp";

  i_form_table_add_row (item, labels);

  for (i_list_move_head(cnt->item_list); (mainboard=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    labels[0] = i_metric_valstr (mainboard->main_12v, NULL);
    labels[1] = i_metric_valstr (mainboard->main_3_3v, NULL);
    labels[2] = i_metric_valstr (mainboard->standby, NULL);
    labels[3] = i_metric_valstr (mainboard->fbdimm_vrm_12v, NULL);
    labels[4] = i_metric_valstr (mainboard->ns_bridge_power, NULL);
    labels[5] = i_metric_valstr (mainboard->nbridge_temp, NULL);
      
    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(mainboard->main_12v), NULL, 0, NULL, 0);
    i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(mainboard->main_3_3v), NULL, 0, NULL, 0);
    i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(mainboard->standby), NULL, 0, NULL, 0);
    i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(mainboard->fbdimm_vrm_12v), NULL, 0, NULL, 0);
    i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(mainboard->ns_bridge_power), NULL, 0, NULL, 0);
    i_form_table_add_link (item, 5, row, RES_ADDR(self), ENT_ADDR(mainboard->nbridge_temp), NULL, 0, NULL, 0);

    if (labels[0]) free (labels[0]);
    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
    if (labels[3]) free (labels[3]);
    if (labels[4]) free (labels[4]);
    if (labels[5]) free (labels[5]);
  }

    i_form_string_add (reqdata->form_out, "mainboardlist_note", "Note", "'*' next to a value indicates the data is not current");

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
