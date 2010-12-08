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
#include <induction/device.h>
#include <induction/site.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>

#include "inventory.h"
        
int l_inventory_remove (i_resource *self, i_resource_address *device_addr, int type, char *vendor_str, char *product_str, char *verser_str)
{       
  /* Remove entries from the inventory tables */
        
  i_list *device_entries;
  i_list *remove_list;
  i_hashtable *vendor_table;
  i_hashtable *device_table;
  i_hashtable *site_table;
  i_hashtable_key *key;
  i_inventory_item *item;

  vendor_table = l_inventory_table_vendor ();
  device_table = l_inventory_table_device ();
  site_table = l_inventory_table_site ();

  /* First we find the device's item in the device_table */

  key = i_hashtable_key_resource (device_addr, device_table->size);
  if (!key)
  { i_printf (1, "l_inventory_remove failed to create key for device_table"); return -1; }
  
  device_entries = i_hashtable_get (device_table, key);
  if (!device_entries)
  { i_printf (2, "l_inventory_remove warning, no entries for specified device"); i_hashtable_free_key (key); return 0; }

  /* Create and fill the remove list */

  remove_list = i_list_create ();
  if (!remove_list)
  { i_printf (1, "l_inventory_remove failed to create remove_list"); i_hashtable_free_key (key); return -1; }

  for (i_list_move_head(device_entries); (item=i_list_restore(device_entries))!=NULL; i_list_move_next(device_entries))
  {
    /* Loop through each item for the specified device */

    /* Check type */
    if (type != INV_ALL && type != item->type)
    { continue; }

    /* Check vendor/product/verser */
    if (vendor_str)
    {
      /* Test vendor_str to see if it matches item->vendor_str */
      if (!strcmp(vendor_str, item->vendor_str))
      { 
        /* Check if a product_str was specified */
        if (product_str)
        {
          /* Product string given, compare it */
          if (!strcmp(product_str, item->product_str))
          {
            /* Product string matches */
            if (verser_str)
            {
              /* Version or serial specified, check it */
              if ((item->type & INV_SOFTWARE && !strcmp(item->version_str, verser_str)) || (item->type & INV_HARDWARE && !strcmp(item->serial_str, verser_str)))
              {
                /* Version or serial match */
                i_list_enqueue (remove_list, item);
                i_list_delete (device_entries);
              }
            }
            else
            {
              /* No version of serial specified, remove record */
              i_list_enqueue (remove_list, item);
              i_list_delete (device_entries);
            }
          }
        }
        else
        {
          /* No product string given, remove entry */
          i_list_enqueue (remove_list, item);
          i_list_delete (device_entries);
        }
      }
    }
    else
    { 
      /* No vendor string given, removing all entries for device */
      i_list_enqueue (remove_list, item); 
      i_list_delete (device_entries); 
    }
  }

  if (device_entries->size < 1)
  {
    /* Device entries is now empty, remove from hashtable */
    i_hashtable_remove (device_table, key);
  }

  i_hashtable_free_key (key); 
  
  /* Remove each of the applicable entries */

  for (i_list_move_head(remove_list); (item=i_list_restore(remove_list))!=NULL; i_list_move_next(remove_list))
  {
    /* Loop through each item to be removed and
     * remove it from the other tables 
     */

    /* First remove the applicable entries from the site_table */
    key = i_hashtable_create_key_string (item->dev->site->name_str, site_table->size);
    if (key)
    { 
      i_list *site_entries;
      i_inventory_item *site_item;
      
      site_entries = i_hashtable_get (site_table, key);
      for (i_list_move_head(site_entries); (site_item=i_list_restore(site_entries))!=NULL; i_list_move_next(site_entries))
      {
        if (site_item == item)
        { i_list_delete (site_entries); }
      }

      if (site_entries && site_entries->size < 1)
      {
        /* Site entries is now empty, remove from hashtable */
        i_hashtable_remove (site_table, key);
      }

      i_hashtable_free_key (key);
    }
    else
    { i_printf (1, "l_inventory_remove warning, failed to create key for site_table"); }

    /* Then remove the applicable entries from the vendor table */

    key = i_hashtable_create_key_string (item->vendor_str, vendor_table->size);
    if (key)
    {
      i_hashtable *product_table;

      product_table = i_hashtable_get (vendor_table, key);
      i_hashtable_free_key (key);
      if (product_table)
      {
        i_list *product_entries;
        i_inventory_item *product_item;

        key = i_hashtable_create_key_string (item->product_str, product_table->size);
        if (key)
        {
          product_entries = i_hashtable_get (product_table, key);
          for (i_list_move_head(product_entries); (product_item=i_list_restore(product_entries))!=NULL; i_list_move_next(product_entries))
          {
            if (product_item == item)
            { i_list_delete (product_entries); }
          }
  
          if (product_entries && product_entries->size < 1)
          {
            /* product entries is now empty, remove from hashtable */
            i_hashtable_remove (product_table, key);
          }
          
          i_hashtable_free_key (key);
        }
        else
        { i_printf (1, "l_inventory_remove warning, failed to create product_table key for product %s", item->product_str); }
      }
      else
      { i_printf (1, "l_inventory_remove warning, failed to get product_table for %s", item->product_str); }
    }
    else
    { i_printf (1, "l_inventory_remove warning, failed to create vendor_table key for vendor %s", item->product_str); }
  }

  /* Finished */

  return 0;
}

int l_inventory_remove_item (i_resource *self, i_inventory_item *invitem)
{
  if (invitem->type & INV_HARDWARE)
  { return l_inventory_remove (self, invitem->dev->resaddr, invitem->type, invitem->vendor_str, invitem->product_str, invitem->serial_str); }
  else if (invitem->type & INV_SOFTWARE)
  { return l_inventory_remove (self, invitem->dev->resaddr, invitem->type, invitem->vendor_str, invitem->product_str, invitem->version_str); }
  else
  { return l_inventory_remove (self, invitem->dev->resaddr, invitem->type, invitem->vendor_str, invitem->product_str, NULL); }

  return -1;
}
