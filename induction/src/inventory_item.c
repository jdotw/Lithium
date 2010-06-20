#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "hierarchy.h"
#include "inventory.h"

i_inventory_item* i_inventory_item_create ()
{
  i_inventory_item *item;

  item = (i_inventory_item *) malloc (sizeof(i_inventory_item));
  if (!item)
  { i_printf (1, "i_inventory_item_create failed to malloc item"); return NULL; }
  memset (item, 0, sizeof(i_inventory_item));

  return item;
}

void i_inventory_item_free (void *itemptr)
{
  i_inventory_item *item = itemptr;

  if (item->vendor_str) free (item->vendor_str);
  if (item->product_str) free (item->product_str);
  if (item->version_str) free (item->version_str);
  if (item->serial_str) free (item->serial_str);
  if (item->platform_str) free (item->platform_str);
  if (item->feature_str) free (item->feature_str);

  free (item);
}
