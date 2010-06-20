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
#include "cpu.h"

int v_cpu_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * CPU Resource Container Main Form
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
    char *labels[6];
    v_cpu_item *cpu;

    if (v_xserve_extras())
    {
      if (v_xserve_intel_extras())
      { item = i_form_table_create (reqdata->form_out, "cpulist", NULL, 6); }
      else
      { item = i_form_table_create (reqdata->form_out, "cpulist", NULL, 5); }
    }
    else
    { item = i_form_table_create (reqdata->form_out, "cpulist", NULL, 2); }
    if (!item) { i_printf (1, "v_cpu_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    if (v_xserve_intel_extras())
    {
      labels[0] = "Description";
      labels[1] = "Utilisation (1)";
      labels[2] = "Utilisation (2)";
      labels[3] = "Inlet Temp";
      labels[4] = "Heatsink Temp";
      labels[5] = "Power";
    }
    else
    {
      labels[0] = "Description";
      labels[1] = "Utilisation";
      labels[2] = "Inlet Temp";
      labels[3] = "Internal Temp";
      labels[4] = "Power";
    }

    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (cpu=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (cpu->obj->desc_str) 
      { labels[0] = cpu->obj->desc_str; }
      else
      { labels[0] = cpu->obj->name_str; }

      if (v_xserve_extras())
      {
        if (v_xserve_intel_extras())
        {
          labels[1] = i_metric_valstr (cpu->usage_1, NULL);
          labels[2] = i_metric_valstr (cpu->usage_2, NULL);
          labels[3] = i_metric_valstr (cpu->temp_inlet, NULL);
          labels[4] = i_metric_valstr (cpu->temp_heatsink, NULL);
          labels[5] = i_metric_valstr (cpu->power_watts, NULL);
        }
        else
        {
          labels[1] = i_metric_valstr (cpu->usage, NULL);
          labels[2] = i_metric_valstr (cpu->temp_inlet, NULL);
          labels[3] = i_metric_valstr (cpu->temp_internal, NULL);
          labels[4] = i_metric_valstr (cpu->power_watts, NULL);
          labels[5] = NULL;
        }
      }
      else
      {
        labels[1] = i_metric_valstr (cpu->usage, NULL);
        labels[2] = NULL;
        labels[3] = NULL;
        labels[4] = NULL;
        labels[5] = NULL;
      }
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(cpu->obj), NULL, 0, NULL, 0);
      if (v_xserve_intel_extras())
      { 
        i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(cpu->usage_1), NULL, 0, NULL, 0); 
        i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(cpu->usage_2), NULL, 0, NULL, 0); 
        i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(cpu->temp_inlet), NULL, 0, NULL, 0); 
        i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(cpu->temp_heatsink), NULL, 0, NULL, 0); 
        i_form_table_add_link (item, 5, row, RES_ADDR(self), ENT_ADDR(cpu->power_watts), NULL, 0, NULL, 0); 
      }
      else if (v_xserve_extras())
      {
        i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(cpu->usage), NULL, 0, NULL, 0); 
        i_form_table_add_link (item, 2, row, RES_ADDR(self), ENT_ADDR(cpu->temp_inlet), NULL, 0, NULL, 0); 
        i_form_table_add_link (item, 3, row, RES_ADDR(self), ENT_ADDR(cpu->temp_internal), NULL, 0, NULL, 0); 
        i_form_table_add_link (item, 4, row, RES_ADDR(self), ENT_ADDR(cpu->power_watts), NULL, 0, NULL, 0); 
      }
      else
      {
        i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(cpu->usage), NULL, 0, NULL, 0); 
      }

      if (labels[1]) free (labels[1]);
      if (labels[2]) free (labels[2]);
      if (labels[3]) free (labels[3]);
      if (labels[4]) free (labels[4]);
      if (labels[5]) free (labels[5]);
    }

    i_form_string_add (reqdata->form_out, "cpulist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "CPU list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "CPU list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "CPU list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
