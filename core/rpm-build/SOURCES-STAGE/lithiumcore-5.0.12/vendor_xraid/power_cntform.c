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

#include "power.h"

int v_power_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Power Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Power Resource Table 
   */

  int row;
  char *labels[2];
  v_power_item *left_power;
  v_power_item *right_power;

  item = i_form_table_create (reqdata->form_out, "powerlist", NULL, 2);
  if (!item) { i_printf (1, "v_power_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  labels[0] = "Left Power Supply";
  labels[1] = "Right Power Supply";
  i_form_table_add_row (item, labels);

  labels[0] = NULL;
  labels[1] = NULL;
  
  left_power = v_power_static_left_item ();
  if (left_power)
  { labels[0] = i_metric_valstr (left_power->status, NULL); }
  
  right_power = v_power_static_right_item ();
  if (right_power)
  { labels[1] = i_metric_valstr (right_power->status, NULL); }
  
  row = i_form_table_add_row (item, labels);

  i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(left_power->obj), NULL, 0, NULL, 0);
  i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(right_power->obj), NULL, 0, NULL, 0);

  if (labels[0]) free (labels[0]);
  if (labels[1]) free (labels[1]);

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
