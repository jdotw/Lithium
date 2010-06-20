#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int form_ipregistry_devicelist (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *labels[2] = { "Device", "Site" };
  i_form_item *item;
  i_ipregistry_entry *entry;
  i_list *list;
  i_hashtable *device_table;
  
  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_ipregistry_devicelist failed to create form"); return -1; }

  i_form_frame_start (reqdata->form_out, "devicelist", "Device List");

  l_ipregistry_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  list = i_list_create ();
  if (!list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list struct"); return 1; }

  device_table = l_ipregistry_table_device ();
  if (!device_table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "IP Registry Device Table not present"); return 1; }

  /* Create a list of the IP addresses */
  
  i_hashtable_iterate (self, device_table, l_ipregistry_devicelist_iterate, list);

  /* Sort the list */

  i_list_sort (list, l_ipregistry_listsort_devicedesc_func);

  /* Add the list table */

  item = i_form_table_create (reqdata->form_out, "entry_list", "Device List", 2);
  if (!item) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list table"); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  for (i_list_move_head(list); (entry=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    char *device_addr_str;
    labels[0] = entry->ipent->dev_desc;
    labels[1] = entry->ipent->site_desc;

    row = i_form_table_add_row (item, labels);

    device_addr_str = i_resource_address_struct_to_string (entry->devaddr);
    if (device_addr_str)
    { i_form_table_add_link (item, 0, row, NULL, NULL, "ipregistry_devicemain", 0, device_addr_str, strlen(device_addr_str)+1); }
    free (device_addr_str);

    i_form_table_add_link (item, 1, row, NULL, NULL, "ipregistry_sitemain", 0, entry->ipent->site_name, strlen(entry->ipent->site_name)+1);
  }

  /* Finished */

  i_list_free (list);

  i_form_frame_end (reqdata->form_out, "devicelist");

  return 1;
}

void l_ipregistry_devicelist_iterate (i_resource *self, i_hashtable *ip_table, void *data, void *passdata)
{
  int num;
  i_list *ip_list = passdata;
  i_list *entry_list = data;
  i_ipregistry_entry *entry;

  if (!ip_list || !entry_list) return;

  i_list_move_head(entry_list);
  entry=i_list_restore(entry_list);

  if (entry)
  {
    num = i_list_enqueue (ip_list, entry);
    if (num != 0)
    { i_printf (1, "l_ipregistry_devicelist_iterate failed to enqueue entry for %s (%s)", entry->ipent->dev_desc, entry->ipent->dev_name); }
  }

  return;
}

