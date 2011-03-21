#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
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

int form_inventory_historymain (i_resource *self, i_form_reqdata *reqdata)
{
  int num;
  char *dev_addr_str;
  char *invitemptr_str;
  void *invitemptr;
  i_list *invitem_list;
  i_inventory_item *invitem;
  
  if (!self || !reqdata) return -1;

  /* Form setup */
  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_historymain failed to create form"); return -1; }
  i_form_frame_start (reqdata->form_out, "historymain", "Inventory - Item History");

  if (!reqdata->form_passdata || reqdata->form_passdata_size < 1)
  { i_form_string_add (reqdata->form_out, "error", "Error", "No device/invitem specified"); return 1; }

  dev_addr_str = (char *) reqdata->form_passdata;
  invitemptr_str = strrchr (dev_addr_str, ':');
  if (!invitemptr_str)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Passdata is corrupt"); return 1; }
  *invitemptr_str = '\0';
  invitemptr_str++;

  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  /* Entry list for specified device */

  invitem_list = l_inventory_get_device_str (self, dev_addr_str);
  if (!invitem_list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified device not found"); return 1; }

  /* Find specified invitem */

  invitemptr = (void *) strtoul (invitemptr_str, NULL, 16);
  
  for (i_list_move_head(invitem_list); (invitem=i_list_restore(invitem_list))!=NULL; i_list_move_next(invitem_list))
  {
    if (invitem == invitemptr)
    { break; }
  }

  if (!invitem)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Specified invitem not found"); return 1; }

  /* Stats */

  i_form_string_add (reqdata->form_out, "device_str", "Device", invitem->dev->desc_str);
  i_form_string_add (reqdata->form_out, "site_str", "Site", invitem->dev->site->desc_str);
  i_form_string_add (reqdata->form_out, "vendor_str", "Vendor", invitem->vendor_str);
  i_form_string_add (reqdata->form_out, "product_str", "Product", invitem->product_str);

  i_form_spacer_add (reqdata->form_out);

  /* Get version history list */

  num = l_inventory_sql_get_list (self, INV_HISTORY_TABLE, invitem->dev->site->name_str, invitem->dev->name_str, invitem->vendor_str, invitem->product_str, NULL, NULL, l_inventory_form_historymain_sqlcb, reqdata);
  if (num != 0)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to request item history list from SQL database"); return 1; }

  /* Finished for now */

  return 0;
}

int l_inventory_form_historymain_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  char *labels[3] = { "First Registration", "", "Item Type" };
  i_form_item *table;
  i_inventory_item *invitem;
  i_form_reqdata *reqdata = passdata;

  /* Check list */

  if (!list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to retrieve item history from SQL database"); i_form_deliver (self, reqdata); return 0; }

  if (list->size < 1)
  { i_form_string_add (reqdata->form_out, "error", "No Data", "Item history data not available"); i_form_deliver (self, reqdata); return 0; }

  /* Item table */

  table = i_form_table_create (reqdata->form_out, "history_table", "Item History", 3);
  if (!table) { i_printf (1, "form_inventory_devicemain failed to create history_table"); return -1; }
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 0, 0, "200", 4);
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 1, 0, "200", 4);
  i_form_item_add_option (table, ITEM_OPTION_WIDTH, 1, 2, 0, "200", 4);
  
  i_list_move_head (list);
  invitem = i_list_restore (list);
  if (!invitem)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to restore first invitem from list"); i_form_deliver (self, reqdata); return 0; }

  if (invitem->type & INV_HARDWARE)
  { labels[1] = "Serial Number"; }
  else if (invitem->type & INV_SOFTWARE)
  { labels[1] = "Version"; }
  else
  { labels[1] = ""; }

  i_form_table_add_row (table, labels);

  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int row;

    labels[0] = ctime ((time_t *) &invitem->register_time.tv_sec);
    
    if (invitem->type & INV_HARDWARE)
    { labels[1] = invitem->serial_str; }
    else if (invitem->type & INV_SOFTWARE)
    { labels[1] = invitem->version_str; }
    else
    { labels[1] = ""; }

    labels[2] = i_inventory_type_str (invitem->type);

    row = i_form_table_add_row (table, labels);
  }

  i_form_frame_end (reqdata->form_out, "historymain");

  /* Deliver form */

  i_form_deliver (self, reqdata);
    
  /* Finished */

  return 0;
}

