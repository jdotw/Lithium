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
#include "fans.h"

int v_fans_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Volumes Container Main Form
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
    int row;
    char *labels[8];
    i_object *objects[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };;
    v_fans_item *fan;

    if (v_xserve_intel_extras())
    { item = i_form_table_create (reqdata->form_out, "fanlist", NULL, 7); }
    else
    { item = i_form_table_create (reqdata->form_out, "fanlist", NULL, 8); }
    if (!item) { i_printf (1, "v_fan_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    if (v_xserve_intel_extras())
    {
      labels[0] = "CPU Fan 1";
      labels[1] = "CPU Fan 2";
      labels[2] = "CPU Fan 3";
      labels[3] = "CPU Fan 4";
      labels[4] = "FBDIMM Fan 1";
      labels[5] = "FBDIMM Fan 2";
      labels[6] = "FBDIMM Fan 3";
    }
    else
    {
      labels[0] = "CPU A1";
      labels[1] = "CPU A2";
      labels[2] = "CPU A3";
      labels[3] = "Sys Ctrl";
      labels[4] = "CPU B1";
      labels[5] = "CPU B2";
      labels[6] = "CPU B3";
      labels[7] = "PCI Slots";
    }
    i_form_table_add_row (item, labels);
    labels[0] = NULL;
    labels[1] = NULL;
    labels[2] = NULL;
    labels[3] = NULL;
    labels[4] = NULL;
    labels[5] = NULL;
    labels[6] = NULL;
    labels[7] = NULL;

    for (i_list_move_head(cnt->item_list); (fan=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      /* Get RPM */
      char *rpm_str;
      if (v_xserve_intel_extras())
      { rpm_str = i_metric_valstr (fan->in_rpm, NULL); }
      else
      { rpm_str = i_metric_valstr (fan->rpm, NULL); }

      /* Select fan */
      if (v_xserve_intel_extras())
      {
        if (strcmp(fan->obj->name_str, "CPU_Fan_1") == 0)
        { labels[0] = rpm_str; objects[0] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_Fan_2") == 0)
        { labels[1] = rpm_str; objects[1] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_Fan_3") == 0)
        { labels[2] = rpm_str; objects[2] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_Fan_4") == 0)
        { labels[3] = rpm_str; objects[3] = fan->obj; }
        if (strcmp(fan->obj->name_str, "FBDIMM_Fan_1") == 0)
        { labels[4] = rpm_str; objects[4] = fan->obj; }
        if (strcmp(fan->obj->name_str, "FBDIMM_Fan_2") == 0)
        { labels[5] = rpm_str; objects[5] = fan->obj; }
        if (strcmp(fan->obj->name_str, "FBDIMM_Fan_3") == 0)
        { labels[6] = rpm_str; objects[6] = fan->obj; }
      }
      else
      {
        if (strcmp(fan->obj->name_str, "CPU_A1") == 0)
        { labels[0] = rpm_str; objects[0] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_A2") == 0)
        { labels[1] = rpm_str; objects[1] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_A3") == 0)
        { labels[2] = rpm_str; objects[2] = fan->obj; }
        if (strcmp(fan->obj->name_str, "System_Controller") == 0)
        { labels[3] = rpm_str; objects[3] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_B1") == 0)
        { labels[4] = rpm_str; objects[4] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_B2") == 0)
        { labels[5] = rpm_str; objects[5] = fan->obj; }
        if (strcmp(fan->obj->name_str, "CPU_B3") == 0)
        { labels[6] = rpm_str; objects[6] = fan->obj; }
        if (strcmp(fan->obj->name_str, "PCI_Slot") == 0)
        { labels[7] = rpm_str; objects[7] = fan->obj; }
      }
    }

    row = i_form_table_add_row (item, labels);
    if (objects[0]) i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(objects[0]), NULL, 0, NULL, 0);
    if (objects[1]) i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(objects[1]), NULL, 0, NULL, 0);
    if (objects[2]) i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(objects[2]), NULL, 0, NULL, 0);
    if (objects[3]) i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(objects[3]), NULL, 0, NULL, 0);
    if (objects[4]) i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(objects[4]), NULL, 0, NULL, 0);
    if (objects[5]) i_form_table_add_link (item, 5, row, RES_ADDR(self), ENT_ADDR(objects[5]), NULL, 0, NULL, 0);
    if (objects[6]) i_form_table_add_link (item, 6, row, RES_ADDR(self), ENT_ADDR(objects[6]), NULL, 0, NULL, 0);
    if (objects[7]) i_form_table_add_link (item, 7, row, RES_ADDR(self), ENT_ADDR(objects[7]), NULL, 0, NULL, 0);
    
    if (labels[0]) free (labels[0]);
    if (labels[1]) free (labels[1]);
    if (labels[2]) free (labels[2]);
    if (labels[3]) free (labels[3]);
    if (labels[4]) free (labels[4]);
    if (labels[5]) free (labels[5]);
    if (labels[6]) free (labels[6]);
    if (labels[7]) free (labels[7]);

    i_form_string_add (reqdata->form_out, "fanlist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Blowers list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Blowers list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Blowers list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
