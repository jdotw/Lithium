#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/ip.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/interface.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "device/snmp.h"
#include "device/snmp_ipaddr.h"
#include "device/snmp_iface.h"

#include "snmp_iface.h"

int v_snmp_iface_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
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
    i_interface *iface;
    
    item = i_form_table_create (reqdata->form_out, "iflist", NULL, 8);
    if (!item) { i_printf (1, "l_snmp_iface_formsection failed to create table"); return -1; }
    i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
    i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

    labels[0] = "Description";
    labels[1] = "IP Address";
    labels[2] = "Admin State";
    labels[3] = "Op State";
    labels[4] = "Last Change";
    labels[5] = "Input Rate";
    labels[6] = "Output Rate";    
    labels[7] = "Duplex";
    i_form_table_add_row (item, labels);

    for (i_list_move_head(cnt->item_list); (iface=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
    {
      int row;
      l_snmp_ipaddr *ipaddr;
      i_metric *duplex;

      if (iface->obj->desc_str) 
      { labels[0] = iface->obj->desc_str; }
      else
      { labels[0] = iface->obj->name_str; }

      i_list_move_head (iface->ip_list);
      ipaddr = i_list_restore (iface->ip_list);
      if (ipaddr)
      { labels[1] = i_metric_valstr (ipaddr->addr, NULL); }
      else
      { labels[1] = strdup (""); }

      labels[2] = i_metric_valstr (iface->adminstate, NULL);
      labels[3] = i_metric_valstr (iface->opstate, NULL);
      labels[4] = i_metric_valstr (iface->lastchange, NULL);
      labels[5] = i_metric_valstr (iface->bps_in, NULL);
      labels[6] = i_metric_valstr (iface->bps_out, NULL);

      duplex = (i_metric *) i_entity_child_get (ENTITY(iface->obj), "duplex");
      if (duplex)
      { labels[7] = i_metric_valstr (duplex, NULL); }
      else
      { labels[7] = strdup ("N/A"); }
      
      row = i_form_table_add_row (item, labels);

      i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(iface->obj), NULL, 0, NULL, 0);

      free (labels[1]);
      free (labels[2]);
      free (labels[3]);
      free (labels[4]);
      free (labels[5]);
      free (labels[6]);
      free (labels[7]);
    }

    i_form_string_add (reqdata->form_out, "snmp_iface_note", "Note", "'*' next to a value indicates the data is not current");
  }
  else
  {
    if (cnt->item_list_state == ITEMLIST_STATE_NONE)
    { item = i_form_string_add (reqdata->form_out, "error", "Interface list not present", NULL); }
    else if (cnt->item_list_state == ITEMLIST_STATE_POPULATE)
    { item = i_form_string_add (reqdata->form_out, "error", "Interface list population is in progress", NULL); }
    else
    { item = i_form_string_add (reqdata->form_out, "error", "Interface list is in an unknown state", NULL); }
  }

  i_form_frame_end (reqdata->form_out, cnt->name_str);

  return 1;
}
