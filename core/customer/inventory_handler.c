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
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>
#include <induction/data.h>

#include "inventory.h"

/* Item registration handler */

int l_inventory_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */

  int num;
  int offset;
  int deviceaddr_datasize;
  int entdesc_datasize;
  char *dataptr;
  char *deviceaddr_data;
  char *entdesc_data;
  i_entity_descriptor *entdesc;
  i_inventory_item *item;

  if (!msg) return 0;

  item = i_inventory_item_create ();
  if (!item)
  { i_printf (1, "l_inventory_handler_register failed to create item struct"); return 0; }

  dataptr = msg->data;

  /* Device Address (FIX POSSIBLY DEFUNCT) */
  deviceaddr_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &deviceaddr_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get deviceaddr_data from msg->data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;
  free (deviceaddr_data);

  /* Entity Descriptor */
  entdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &entdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get entdesc_data from msg->data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  /* Convert descriptor to struct */
  entdesc = i_entity_descriptor_struct (entdesc_data, entdesc_datasize);
  free (entdesc_data);
  if (!entdesc)
  { i_printf (1, "l_inventory_handler_register failed to convert descriptor data to struct"); i_inventory_item_free (item); return 0; }

  /* Retrieve device */
  item->dev = (i_device *) i_entity_local_get (self, (i_entity_address *) entdesc);
  i_entity_descriptor_free (entdesc);
  if (!item->dev)
  { 
    /* Device no longer present */
    i_inventory_item_free (item);
    return 0;
  }

  /* Item info */

  item->type = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get item type from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->vendor_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get vendor_ste from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->product_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get product_str from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->version_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get version_str from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->serial_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get serial_str from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->platform_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get platform_str from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->feature_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get feature_str from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->v_major = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get v_major from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->v_minor = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get v_minor from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->v_micro = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get v_micro from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->v_patch = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get v_patch from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->v_build = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get v_build from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  item->flags = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_register failed to get flags from data"); i_inventory_item_free (item); return 0; }
  dataptr += offset;

  /* Add to registry */

  if (item->type & INV_SOFTWARE)
  {
    i_printf (2, "l_inventory_handler_register recvd software registration of %s %s version %s (%i.%i.%i patch=%i build=%i platform='%s' feature='%s' type='%s') registered to %s at %s", item->vendor_str, item->product_str, item->version_str, item->v_major, item->v_minor, item->v_micro, item->v_patch, item->v_build, item->platform_str, item->feature_str, i_inventory_type_str (item->type), item->dev->name_str, item->dev->site->name_str);
  }
  else if (item->type & INV_HARDWARE)
  {
    i_printf (2, "l_inventory_handler_register recvd software registration of %s %s serial number %s (type='%s') registered to %s at %s", item->vendor_str, item->product_str, item->serial_str, i_inventory_type_str (item->type), item->dev->name_str, item->dev->site->name_str);
  }
  else
  {
    i_printf (2, "l_inventory_handler_register recvd other registration type of %s %s version %s serial %s (%i.%i.%i patch=%i build=%i platform='%s' feature='%s' type='%s') registered to %s at %s", item->vendor_str, item->product_str, item->version_str, item->feature_str, item->v_major, item->v_minor, item->v_micro, item->v_patch, item->v_build, item->platform_str, item->feature_str, i_inventory_type_str (item->type), item->dev->name_str, item->dev->site->name_str);
  }

  num = l_inventory_add (self, item);
  if (num != 0)
  { 
    /* If num is -1 an error occurred, is num=1 the item was ignored.
     * either way, it needs to be freed as it didnt go into the table
     */

    i_inventory_item_free (item);
    if (num == -1)
    { i_printf (1, "l_inventory_handler_register failed to call l_inventory_add to add recvd software registry item"); }
  }

  return 0;
}

/* Item deregistration handler */

int l_inventory_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */

  int num;
  int type;
  int offset;
  int deviceaddr_datasize;
  char *dataptr;
  char *deviceaddr_data;
  char *vendor_str;
  char *product_str;
  char *verser_str;
  i_resource_address *device_addr;

  dataptr = msg->data;

  /* Device address */

  deviceaddr_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &deviceaddr_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_deregister failed to get deviceaddr_data from msg->data"); return 0; }
  dataptr += offset;

  device_addr = i_resource_address_string_to_struct (deviceaddr_data);
  free (deviceaddr_data);
  if (!device_addr)
  { i_printf (1, "l_inventory_handler_deregister failed to convert deviceaddr_data to device_addr struct"); return 0; }

  /* Software info */

  type = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_deregister failed to get item type from msg->data"); i_resource_address_free (device_addr); return 0; }
  dataptr += offset;

  vendor_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_inventory_handler_deregister failed to get vendor_str from msg->data"); i_resource_address_free (device_addr); return 0; }
  dataptr += offset;

  product_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { 
    i_printf (1, "l_inventory_handler_deregister failed to get product_str from msg->data"); 
    i_resource_address_free (device_addr); 
    if (vendor_str) free (vendor_str); 
    return 0; 
  }
  dataptr += offset;

  verser_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { 
    i_printf (1, "l_inventory_handler_deregister failed to get verser_str (version/serial) from msg->data"); 
    i_resource_address_free (device_addr); 
    if (vendor_str) free (vendor_str); 
    if (product_str) free (product_str); 
    return 0; 
  }
  dataptr += offset;

  /* Remove from registry */

  if (vendor_str && product_str && verser_str)
  { i_printf (2, "l_inventory_handler_deregister removing %s %s (%s) item (type: %s) for device %s", vendor_str, product_str, verser_str, i_inventory_type_str (type), device_addr->ident_str); }
  else if (vendor_str && product_str)
  { i_printf (2, "l_inventory_handler_deregister removing all %s %s items (type: %s) for device %s", vendor_str, product_str, i_inventory_type_str (type), device_addr->ident_str); }
  else if (vendor_str)
  { i_printf (2, "l_inventory_handler_deregister removing all items (type: %s) for vendor %s for device %s", i_inventory_type_str (type), vendor_str, device_addr->ident_str); }
  else
  { i_printf (2, "l_inventory_handler_deregister removing all items for device %s (type: %s)", device_addr->ident_str, i_inventory_type_str (type)); }

  num = l_inventory_remove (self, device_addr, type, vendor_str, product_str, verser_str);
  i_resource_address_free (device_addr);
  if (vendor_str) free (vendor_str);
  if (product_str) free (product_str);
  if (verser_str) free (verser_str);
  if (num != 0)
  { i_printf (1, "l_inventory_handler_deregister failed to remove item"); return 0; }

  return 0;
}

/* Static Item Registration Handler */

  
