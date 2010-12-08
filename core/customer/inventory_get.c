#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

/* Get product_table for a specific vendor */

i_hashtable* l_inventory_get_vendor (i_resource *self, char *vendor_str)
{
  i_hashtable *vendor_table;
  i_hashtable *product_table;
  i_hashtable_key *key;

  vendor_table = l_inventory_table_vendor ();
  if (!vendor_table)
  { i_printf (1, "l_inventory_get_vendor failed to get l_inventory vendor table"); return NULL; }

  key = i_hashtable_create_key_string (vendor_str, vendor_table->size);
  if (!key)
  { i_printf (1, "l_inventory_get_vendor failed to create hashtable key for vendor"); return NULL; }

  product_table = i_hashtable_get (vendor_table, key);
  i_hashtable_free_key (key);

  return product_table;
}

/* Get list of entries for a product */

i_list* l_inventory_get_product (i_resource *self, i_hashtable *product_table, char *product_str)
{
  i_list *list;
  i_hashtable_key *key;

  key = i_hashtable_create_key_string (product_str, product_table->size);
  if (!key)
  { i_printf (1, "l_inventory_get_product failed to create hashtable key for product"); return NULL; }

  list = i_hashtable_get (product_table, key);
  i_hashtable_free_key (key);

  return list;
}

/* Get list of entries for a device */

i_list* l_inventory_get_device_str (i_resource *self, char *device_addr_str)
{
  i_list *list;
  i_resource_address *device_addr;

  device_addr = i_resource_address_string_to_struct (device_addr_str);
  if (!device_addr)
  { i_printf (1, "l_inventory_get_device_str failed to convert device_addr_str to device_addr struct"); return NULL; }

  list = l_inventory_get_device (self, device_addr);
  i_resource_address_free (device_addr);

  return list;
}

i_list* l_inventory_get_device (i_resource *self, i_resource_address *device_addr)
{
  i_list *list;
  i_hashtable *device_table;
  i_hashtable_key *key;

  device_table = l_inventory_table_device ();
  if (!device_table)
  { i_printf (1, "l_inventory_get_device failed to get l_inventory Device table"); return NULL; }

  key = i_hashtable_key_resource (device_addr, device_table->size);
  if (!key)
  { i_printf (1, "l_inventory_get_device failed to create key for device_addr"); return NULL; }

  list = i_hashtable_get (device_table, key);
  i_hashtable_free_key (key);

  return list;
}

/* Get list of entries for a site */

i_list* l_inventory_get_site (i_resource *self, char *site_name)
{
  i_list *list;
  i_hashtable *site_table;
  i_hashtable_key *key;

  site_table = l_inventory_table_site ();
  if (!site_table)
  { i_printf (1, "l_inventory_get_site failed to get l_inventory site table"); return NULL; }

  key = i_hashtable_create_key_string (site_name, site_table->size);
  if (!key)
  { i_printf (1, "l_inventory_get_site failed to create key for site_addr"); return NULL; }

  list = i_hashtable_get (site_table, key);
  i_hashtable_free_key (key);

  return list;
}

