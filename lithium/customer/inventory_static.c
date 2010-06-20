#include <stdlib.h>

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

/* Static item functions */

/* Add */

int l_inventory_static_add (i_resource *self, i_inventory_item *invitem)
{
  int num;

  /* Add item to SQL database */
  num = l_inventory_sql_insert (self, INV_STATIC_TABLE, invitem);
  if (num != 0)
  { i_printf (1, "l_inventory_static_add failed to request addition of static item %s %s to static item table", invitem->vendor_str, invitem->product_str); return -1; }

  /* Add item to inventory */
  num = l_inventory_add (self, invitem);
  if (num != 0)
  { i_printf (1, "l_inventory_static_add failed to add static item %s %s to inventory", invitem->vendor_str, invitem->product_str); return -1; }

  return 0;
}

/* Remove */

int l_inventory_static_remove_item (i_resource *self, i_inventory_item *invitem)
{
  int num = -1;

  /* Remove from SQL database */
  if (invitem->type & INV_HARDWARE)
  { num = l_inventory_sql_delete (self, INV_STATIC_TABLE, invitem->dev->site->name_str, invitem->dev->name_str, invitem->vendor_str, invitem->product_str, NULL, invitem->serial_str); }
  else if (invitem->type & INV_SOFTWARE)
  { num = l_inventory_sql_delete (self, INV_STATIC_TABLE, invitem->dev->site->name_str, invitem->dev->name_str, invitem->vendor_str, invitem->product_str, invitem->version_str, NULL); }
  else
  { num = l_inventory_sql_delete (self, INV_STATIC_TABLE, invitem->dev->site->name_str, invitem->dev->name_str, invitem->vendor_str, invitem->product_str, NULL, NULL); }

  if (num != 0)
  { 
    i_printf (1, "l_inventory_static_remove_item warning, failed to request removal of static item %s %s from static item table",
      invitem->vendor_str, invitem->product_str);
  }

  /* Remove from inventory */
  num = l_inventory_remove_item (self, invitem);
  if (num != 0)
  { 
    i_printf (1, "l_inventory_static_remove_item faild to remove static item %s %s from inventory",
      invitem->vendor_str, invitem->product_str);
    return -1;
  }

  return 0;
}

/* Register all */

int l_inventory_static_registerall (i_resource *self, i_device *dev)
{
  /* Load list of static items for given hierarchy
   * and then add each item to the current inventory
   */

  int num;

  num = l_inventory_sql_get_list (self, INV_STATIC_TABLE, dev->site->name_str, dev->name_str, NULL, NULL, NULL, NULL, l_inventory_static_registerall_sqlcb, i_entity_address_string (ENTITY(dev), NULL));
  if (num != 0)
  { 
    i_printf (1, "l_inventory_static_registerall failed to request all entries for device %s at %s from static items table",
      dev->site->name_str, dev->name_str);
    return -1; 
  }

  return 0;
}

int l_inventory_static_registerall_sqlcb (i_resource *self, i_list *list, void *passdata)
{
  /* Called when the list of static items
   * has been retrieved for a static_registerall
   * operation
   */

  i_entity_address *entaddr;
  i_inventory_item *invitem;
  i_device *dev;

  /* Convert entity address */
  entaddr = i_entity_address_struct ((char *) passdata);
  free (passdata);

  /* Retrieve device */
  dev = (i_device *) i_entity_local_get (self, entaddr);
  i_entity_address_free (entaddr);
  if (!dev)
  {
    /* Device no longer present */
    return 0;
  }

  for (i_list_move_head(list); (invitem=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    int num;

    /* Set device */
    invitem->dev = dev;
    
    /* Add item */
    num = l_inventory_add (self, invitem);
    if (num != 0)
    { 
      i_printf (1, "l_inventory_static_registerall_sqlcb failed to add static item %s %s to inventory", invitem->vendor_str, invitem->product_str); 
      return -1; 
    }

    /* Remove item from list so that
     * it isnt freed by l_inventory_sql_get_list
     * when the function returns.
     */
    i_list_delete (list);
  }

  return 0;
}  
