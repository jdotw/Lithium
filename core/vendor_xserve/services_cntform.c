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

#include "services.h"

int v_services_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  /*
   * Services Container Main Form
   */
  i_form_item *item = NULL;
  i_container *cnt = (i_container *) ent;
  
  /* Start Frame */
  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);

  /* 
   * Services Table 
   */

  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[3];
    v_services_item *service;
    
    item = i_form_table_create (reqdata->form_out, "servicelist", NULL, 3);
    if (!item) { i_printf (1, "v_service_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "State";
    labels[2] = "Start Time";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (service=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      /* Check state */
      char *state_str = i_metric_valstr (service->state, NULL);
      if (service->obj->opstate != ENTSTATE_NORMAL || (state_str && strcmp(state_str, "RUNNING") == 0))
      {
        int row;

        if (service->obj->desc_str) 
        { labels[0] = service->obj->desc_str; }
        else
        { labels[0] = service->obj->name_str; }

        labels[1] = i_metric_valstr (service->state, NULL);
        labels[2] = i_metric_valstr (service->start_time, NULL);
      
        row = i_form_table_add_row (item, labels);

        i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(service->obj), NULL, 0, NULL, 0);

        if (labels[1]) free (labels[1]);
        if (labels[2]) free (labels[2]);
      }
      free (state_str);
    }
    i_form_string_add (reqdata->form_out, "servicelist_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Services list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Services list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Services list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
