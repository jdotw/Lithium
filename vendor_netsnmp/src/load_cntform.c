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

#include "load.h"

int l_snmp_nsload_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Load Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  l_snmp_nsload_item *load;
  i_object *obj;
  char *labels[4];
  
  i_list_move_head (cnt->obj_list);
  obj = i_list_restore (cnt->obj_list);
  if (!obj)
  { i_printf (1, "l_snmp_nsload_cntform failed to retrieve master object"); return -1; }
  load = obj->itemptr;

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  item = i_form_table_create (reqdata->form_out, "loadtable", NULL, 4);
  if (!item) { i_printf (1, "l_snmp_nsload_formsection failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
        
  labels[0] = "Description";
  labels[1] = "One Minute";
  labels[2] = "Five Minutes";
  labels[3] = "Fifteen Minutes";
  i_form_table_add_row (item, labels);

  labels[0] = "Master Load Averages";
  labels[1] = i_metric_valstr (load->one_min, NULL);
  labels[2] = i_metric_valstr (load->five_min, NULL);
  labels[3] = i_metric_valstr (load->fifteen_min, NULL);
  i_form_table_add_row (item, labels);

  i_form_table_add_link (item, 0, 1, RES_ADDR(self), ENT_ADDR(obj), NULL, 0, NULL, 0);
  i_form_table_add_link (item, 1, 1, RES_ADDR(self), ENT_ADDR(load->one_min), NULL, 0, NULL, 0);
  i_form_table_add_link (item, 2, 1, RES_ADDR(self), ENT_ADDR(load->five_min), NULL, 0, NULL, 0);
  i_form_table_add_link (item, 3, 1, RES_ADDR(self), ENT_ADDR(load->fifteen_min), NULL, 0, NULL, 0);

  free (labels[1]);
  free (labels[2]);
  free (labels[3]);

  i_form_string_add (reqdata->form_out, "loadlist_note", "Note", "'*' next to a value indicates the data is not current");

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
