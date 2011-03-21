#include <stdlib.h>
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

typedef struct
{
  char *res_addr_str;
  i_inventory_item *item;
} l_inventory_add_cbdata;

/* Add new item */

int l_inventory_add (i_resource *self, i_inventory_item *item)
{
  int num;
  i_list *list;
  i_hashtable *vendor_table;
  i_hashtable *product_table;
  i_hashtable *device_table;
  i_hashtable *site_table;
  i_hashtable_key *key;
  l_inventory_add_cbdata *cbdata;

  vendor_table = l_inventory_table_vendor ();
  device_table = l_inventory_table_device ();
  site_table = l_inventory_table_site ();

  if (!vendor_table)
  { i_printf (1, "l_inventory_add failed, vendor_table not present"); return -1; }

  /* Get/Create Vendor Table from Product Table */

  key = i_hashtable_create_key_string (item->vendor_str, vendor_table->size);
  if (!key)
  { i_printf (1, "l_inventory_add failed to create key for vendor_table"); return -1; }

  product_table = i_hashtable_get (vendor_table, key);
  if (!product_table)
  { 
    /* No product table, create it (first item for this vendor) */
    product_table = i_hashtable_create (DEFAULT_PRODUCT_TABLE_SIZE);
    if (!product_table)
    { i_printf (1, "l_inventory_add failed to create %s product_table for vendor %s", item->product_str, item->vendor_str); i_hashtable_free_key (key); return -1; }
    i_hashtable_set_destructor (product_table, i_list_free);

    num = i_hashtable_put (vendor_table, key, product_table);
    if (num != 0)
    { i_printf (1, "l_inventory_add failed to put product_table into vendor_table"); i_hashtable_free (product_table); i_hashtable_free_key (key); return -1; }
  }
  i_hashtable_free_key (key);
  key = NULL;

  /* Get/Create item list from product_table */

  key = i_hashtable_create_key_string (item->product_str, product_table->size);
  if (!key)
  { i_printf (1, "l_inventory_add failed to create key for product_table"); return -1; }

  list = i_hashtable_get (product_table, key);
  if (!list)
  {
    /* No list of entries for this product, create it */
    list = i_list_create ();
    if (!list)
    { i_printf (1, "l_inventory_add failed to create item list for product %s of vendor %s", item->product_str, item->vendor_str); i_hashtable_free_key (key); return -1; }
    i_list_set_destructor (list, i_inventory_item_free);

    num = i_hashtable_put (product_table, key, list);
    if (num != 0)
    { i_printf (1, "l_inventory_add failed to put new item list into %s product table for %s", item->product_str, item->vendor_str); i_list_free (list); i_hashtable_free_key (key); return -1; }
  }
  i_hashtable_free_key (key);
  key = NULL;

  /* Add item to item list from product table */

  num = i_list_enqueue (list, item);
  if (num != 0)
  { i_printf (1, "l_inventory_add failed to enqueue item for device %s at %s into item list in %s product_table for %s", item->dev->name_str, item->dev->site->name_str, item->product_str, item->vendor_str); return -1; }
  i_list_sort (list, l_inventory_listsort_typeverser_func);

  /* Add item to device table. If it doesnt work, just warn. */

  if (device_table)
  {
    key = i_hashtable_key_resource (item->dev->resaddr, device_table->size);
    if (key)
    { 
      list = i_hashtable_get (device_table, key);
      if (!list)
      {
        list = i_list_create ();
        if (list)
        { i_hashtable_put (device_table, key, list); }
        else
        { i_printf (1, "l_inventory_add failed to put list into device_table"); }
      }
      i_hashtable_free_key (key);
      key = NULL;

      num = i_list_enqueue (list, item);
      if (num != 0)
      { i_printf (1, "l_inventory_add failed to enqueue item into list for device_table"); }
      i_list_sort (list, l_inventory_listsort_devicedesc_func); 
    }
    else
    { i_printf (1, "l_inventory_add failed to create key for device_table"); }
  }
  else
  { i_printf (1, "l_inventory_add warning, device_table not present"); }

  /* Add item to site table. If it doesnt work, just warn. */

  if (site_table)
  {  
    key = i_hashtable_create_key_string (item->dev->site->name_str, site_table->size);  
    if (key)
    { 
      list = i_hashtable_get (site_table, key);
      if (!list)
      {
        list = i_list_create ();
        if (list)
        { i_hashtable_put (site_table, key, list); }
        else
        { i_printf (1, "l_inventory_add failed to put list into site_table"); }
      } 
      i_hashtable_free_key (key);
      key = NULL;
 
      num = i_list_enqueue (list, item);
      if (num != 0)
      { i_printf (1, "l_inventory_add failed to enqueue item into list for site_table"); }
      i_list_sort (list, l_inventory_listsort_sitedesc_func); 
    } 
    else
    { i_printf (1, "l_inventory_add failed to create key for site_table"); }
  } 
  else
  { i_printf (1, "l_inventory_add warning, site_table not present"); }

  /* Set registration time */

  gettimeofday (&item->register_time, NULL);

  /* Get list of matching entries in the 
   * item_history table to determine if a new item is 
   * needed in the database for this version
   */

  cbdata = (l_inventory_add_cbdata *) malloc (sizeof(l_inventory_add_cbdata));
  if (cbdata)
  {
    memset (cbdata, 0, sizeof(l_inventory_add_cbdata));
    cbdata->item = item;
    cbdata->res_addr_str = i_resource_address_struct_to_string (item->dev->resaddr);
    if (cbdata->res_addr_str)
    {
      num = l_inventory_sql_get_list (self, INV_HISTORY_TABLE, item->dev->site->name_str, item->dev->name_str, item->vendor_str, item->product_str, item->version_str, item->serial_str, l_inventory_add_sqlget_callback, cbdata);
      if (num != 0)
      { i_printf (1, "l_inventory_add warning, failed to call l_inventory_sql_get_historylist"); free (cbdata->res_addr_str); free (cbdata); }
    }
    else
    { i_printf (1, "l_inventory_add failed to convert item->dev->resaddr to res_addr_str"); free (cbdata); }
  }
  else
  { i_printf (1, "l_inventory_add failed to malloc cbdata struct"); }

  /* Finished */

  return 0;
}

int l_inventory_add_sqlget_callback (i_resource *self, i_list *list, void *passdata)
{
  int num;
  i_list *dev_entries;
  l_inventory_add_cbdata *cbdata = passdata;

  if (list && list->size > 0)
  {
    /* A list was returned, and it contains
     * entries. Hence, there's going to be no
     * ned to INSERT a new item record whether
     * the item is still active or not.
     */

    free (cbdata->res_addr_str);
    free (cbdata);

    return 0;
  }

  dev_entries = l_inventory_get_device_str (self, cbdata->res_addr_str);
  if (dev_entries)
  {
    /* Check list of entries for the item we're
     * looking for. This is necessary just incase the
     * item is removed from the hashtable in between
     * being added and having its SQL entries checked
     */

    i_inventory_item *item;

    for (i_list_move_head(dev_entries); (item=i_list_restore(dev_entries))!=NULL; i_list_move_next(dev_entries))
    {
      if (item == cbdata->item)
      {
        /* Match, item is still active */
        num = l_inventory_sql_insert (self, INV_HISTORY_TABLE, item);
        if (num != 0)
        { i_printf (1, "l_inventory_add_sqlget_callback failed to call l_inventory_sql_insert"); }

        /* Should only ever be a single
         * match in the dev_entries list.
         * Safe to break the loop now
         */
        break;
      }
    }
  }

  free (cbdata->res_addr_str);
  free (cbdata);

  return 0;
}
