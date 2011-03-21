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
#include "snmp_route.h"

int l_snmp_route_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_container *cnt = (i_container *) ent;
  i_form_item *item = NULL;

  /* 
   * Interface table 
   */

  i_form_frame_start (reqdata->form_out, cnt->name_str, cnt->desc_str);
  
  if (cnt->item_list && cnt->item_list_state == ITEMLIST_STATE_NORMAL)
  {
    char *labels[8];
    l_snmp_route_item *route;
    
    item = i_form_table_create (reqdata->form_out, "routelist", NULL, 8);
    if (!item) { i_printf (1, "l_snmp_route_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Destination";
    labels[1] = "Mask";
    labels[2] = "Next Hop";
    labels[3] = "Egress Interface";
    labels[4] = "Primary Metric";
    labels[5] = "Age";
    labels[6] = "Protocol";
    labels[7] = "Type";    
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (route=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;

      if (route->obj->desc_str) 
      { labels[0] = route->obj->desc_str; }
      else
      { labels[0] = route->obj->name_str; }

      labels[1] = i_metric_valstr (route->mask, NULL);
      labels[2] = i_metric_valstr (route->nexthop, NULL);
      if (route->iface && route->iface->obj && route->iface->obj->desc_str)
      {
        labels[3] = strdup (route->iface->obj->desc_str);
      }
      else
      {
        labels[3] = strdup ("");
      }
      labels[4] = i_metric_valstr (route->metric1, NULL);
      labels[5] = i_metric_valstr (route->age, NULL);
      labels[6] = i_metric_valstr (route->protocol, NULL);
      labels[7] = i_metric_valstr (route->type, NULL);
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(route->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);
      free (labels[5]);
      free (labels[6]);
      free (labels[7]);
    }

    i_form_string_add (reqdata->form_out, "snmp_route_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Route list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Route list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Route list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
