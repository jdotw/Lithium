#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ip.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/interface.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_swrun.h"

int l_snmp_swrun_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  /* 
   * Process
   */

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[7];
    l_snmp_swrun *sw;
    
    item = i_form_table_create (reqdata->form_out, "swrunlist", NULL, 7);
    if (!item) { i_printf (1, "l_snmp_swrun_cntform failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "PID";
    labels[1] = "Process Name";
    labels[2] = "Params";
    labels[3] = "Type";
    labels[4] = "Status";
    labels[5] = "CPU C.Sec";
    labels[6] = "Memory Used";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (sw=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      labels[0] = sw->obj->name_str;
      labels[1] = i_metric_valstr (sw->procname, NULL);
      labels[2] = i_metric_valstr (sw->params, NULL);
      labels[3] = i_metric_valstr (sw->type, NULL);
      labels[4] = i_metric_valstr (sw->status, NULL);
      labels[5] = i_metric_valstr (sw->cpu_csec, NULL);
      labels[6] = i_metric_valstr (sw->mem, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(sw->obj), NULL, 0, NULL, 0);
      i_form_table_add_link (item, 1, row, RES_ADDR(self), ENT_ADDR(sw->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);
      free (labels[5]);
      free (labels[6]);
    }

    i_form_string_add (reqdata->form_out, "swrunlist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Process list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Process list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Process list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
