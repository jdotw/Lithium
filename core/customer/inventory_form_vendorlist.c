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
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>

#include "inventory.h"

static char *static_last_vendor_str = NULL;

int form_inventory_vendorlist (i_resource *self, i_form_reqdata *reqdata)
{
  int row;
  char *labels[1] = { "Vendor" };
  i_list *list;
  i_form_item *item;
  i_inventory_item *invitem;
  i_hashtable *vendor_table;
  
  if (!self || !reqdata) return -1;

  if (!reqdata->auth || reqdata->auth->level < AUTH_LEVEL_CLIENT)
  { return i_form_deliver_denied(self, reqdata); }

  reqdata->form_out = i_form_create (reqdata, 0);
  if (!reqdata->form_out)
  { i_printf (1, "form_inventory_vendorlist failed to create form"); return -1; }

  i_form_frame_start (reqdata->form_out, "vendorlist", "Inventory - Vendor List");

  l_inventory_form_links (self, reqdata);
  i_form_spacer_add (reqdata->form_out);

  list = i_list_create ();
  if (!list)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list struct"); return 1; }

  vendor_table = l_inventory_table_vendor ();
  if (!vendor_table)
  { i_form_string_add (reqdata->form_out, "error", "Error", "Inventory Vendor Table not present"); return 1; }

  /* Create a list of the vendors */
  
  i_hashtable_iterate (self, vendor_table, l_inventory_vendorlist_vendortable_iterate, list);

  /* Sort the list */

  i_list_sort (list, l_inventory_listsort_typevendor_func);

  /* Add the list table */

  item = i_form_table_create (reqdata->form_out, "invitem_list", "Vendor List", 1);
  if (!item) { i_form_string_add (reqdata->form_out, "error", "Error", "Failed to create list table"); return 1; }
  i_form_item_add_option (item, ITEM_OPTION_SPACING_PROP, 0, 0, 0, NULL, 0);            /* Use proportional spacing */
  i_form_item_add_option (item, ITEM_OPTION_WIDTH, 0, 0, 0, "100%", 5);                 /* 100% Width for main table */

  i_form_table_add_row (item, labels);

  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    labels[0] = invitem->vendor_str;

    row = i_form_table_add_row (item, labels);

    i_form_table_add_link (item, 0, row, RES_ADDR(self), NULL, "inventory_vendormain", 0, invitem->vendor_str, strlen(invitem->vendor_str)+1);
  }

  /* Finished */

  i_list_free (list);

  i_form_frame_end (reqdata->form_out, "vendorlist");

  return 1;
}

void l_inventory_vendorlist_vendortable_iterate (i_resource *self, i_hashtable *vendor_table, void *data, void *passdata)
{
  /* Iterate each cell in the vendortable */
  i_list *vendor_list = passdata;
  i_hashtable *product_table = data;

  static_last_vendor_str = NULL;
  i_hashtable_iterate (self, product_table, l_inventory_vendorlist_producttable_iterate, vendor_list);  
  static_last_vendor_str = NULL;

  return;
}

void l_inventory_vendorlist_producttable_iterate (i_resource *self, i_hashtable *product_table, void *data, void *passdata)
{
  /* Iterate each cell in the product table */

  int num;
  i_inventory_item *invitem;
  i_list *invitem_list = data;
  i_list *vendor_list = passdata;

  /* Get first invitem from this product table
   * invitem list
   */
  
  i_list_move_head(invitem_list);
  invitem=i_list_restore(invitem_list);
  if (!invitem) return;

  /* Check to see we're not on a
   * vendor that has already been added
   */
  
  if (static_last_vendor_str && !strcmp(static_last_vendor_str, invitem->vendor_str))
  { return; }     /* Same vendor as last, exit without adding */
  static_last_vendor_str = invitem->vendor_str;

  /* Enqueue the invitem to the vendor list */
  
  num = i_list_enqueue (vendor_list, invitem);
  if (num != 0)
  { i_printf (1, "l_inventory_vendorlist_producttable_iterate failed to enqueue invitem for %s", invitem->vendor_str); }
  
}
