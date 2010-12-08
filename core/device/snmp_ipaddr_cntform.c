#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ip.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/interface.h>
#include <induction/form.h>
#include <induction/str.h>

#include "snmp.h"
#include "snmp_ipaddr.h"

int l_snmp_ipaddr_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  char *labels[3] = { "IP Address", "Netmask", "Interface" };
  i_form_item *item;
  i_container *cnt = (i_container *) ent;
  l_snmp_ipaddr *addr;

  /* Check Item */
  if (cnt != l_snmp_ipaddr_cnt())
  { i_form_string_add (reqdata->form_out, "error", "Error", "Invalid item."); return 1; }

  /* Start Frame */
  i_form_frame_start (reqdata->form_out, ent->name_str, ent->desc_str);  

  /* 
   * Address table 
   */

  item = i_form_table_create (reqdata->form_out, "ipaddrlist", NULL, 3);
  if (!item) { i_printf (1, "l_snmp_ipaddr_cntform failed to create table"); return -1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */
  i_form_table_add_row (item, labels);

  for (i_list_move_head(cnt->item_list); (addr=i_list_restore(cnt->item_list))!=NULL; i_list_move_next(cnt->item_list))
  {
    int row;

    labels[0] = addr->obj->desc_str;
    labels[1] = i_metric_valstr (addr->netmask, NULL);
    if (addr->iface)
    { labels[2] = addr->iface->obj->desc_str; }
    else
    { labels[2] = NULL; }

    row = i_form_table_add_row (item, labels);
    i_form_table_add_link (item, 0, row, RES_ADDR(self), ENT_ADDR(addr->obj), NULL, 0, NULL, 0);

    if (labels[1]) free (labels[1]);
  }
    
  /* End Frame */
  i_form_frame_end (reqdata->form_out, ent->name_str);

  return 1;
}

