#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>

#include "inventory.h"

int form_inventory_devicelist (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *labels[2] = { "Device", "Site" };
  i_form_item *item;
  i_inventory_item *invitem;
  i_list *list;
  i_hashtable *device_table;
  
  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_devicelist failed to create form"); return -1; }

  i_form_frame_start (reqdata->form_out, "devicelist", "Inventory - Device List");

  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  list = i_list_create ();
  if (!list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list struct"); return 1; }

  device_table = l_inventory_table_device ();
  if (!device_table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Inventory Device Table not present"); return 1; }

  /* Create a list of the devices */
  
  i_hashtable_iterate (self, device_table, l_inventory_devicelist_iterate, list);

  /* Sort the list */

  i_list_sort (list, l_inventory_listsort_devicedesc_func);

  /* Add the list table */

  item = i_form_table_create (reqdata->form_out, "invitem_list", "Device List", 2);
  if (!item) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list table"); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    labels[0] = invitem->dev->desc_str;
    labels[1] = invitem->dev->site->desc_str;

    row = i_form_table_add_row (item, labels);

    if (invitem->dev->resaddr)
    {
      char *addr_str;
      addr_str = i_resource_address_struct_to_string (invitem->dev->resaddr);
      i_form_table_add_link (item, 0, row, NULL, ENT_ADDR(invitem->dev), "inventory_devicemain", 0, addr_str, strlen(addr_str)+1);
      free (addr_str);
    }
  }

  /* Finished */

  i_list_free (list);

  i_form_frame_end (reqdata->form_out, "devicelist");

  return 1;
}

void l_inventory_devicelist_iterate (i_resource *self, i_hashtable *device_table, void *data, void *passdata)
{
  int num;
  i_list *device_list = passdata;
  i_list *invitem_list = data;
  i_inventory_item *invitem;

  if (!device_list || !invitem_list) return;

  i_list_move_head(invitem_list);
  invitem=i_list_restore(invitem_list);

  if (invitem)
  {
    num = i_list_enqueue (device_list, invitem);
    if (num != 0)
    { i_printf (1, "l_inventory_devicelist_iterate failed to enqueue invitem for %s (%s)", invitem->dev->desc_str, invitem->dev->name_str); }
  }

  return;
}

