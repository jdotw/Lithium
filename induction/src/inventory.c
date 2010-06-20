#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "data.h"
#include "message.h"
#include "cement.h"
#include "entity.h"
#include "device.h"
#include "hierarchy.h"
#include "inventory.h"

/* Register item */

long i_inventory_register (i_resource *self, i_inventory_item *item)
{
  /* Send a messagr to the customer resource to register the given
   * inventory item.
   *
   * Message data structure is:
   *
   * int deviceaddr_datasize;
   * char *deviceaddr_data;
   * int entdesc_datasize;
   * char *entdesc_data;
   * 
   * int type
   * int vendor_str_size
   * char *vendor_str
   * int product_str_size
   * char *product_str
   * 
   * int version_str_size
   * char *version_str
   * int serial_str_size
   * char *serial_str
   * int platform_str_size
   * char *platform_str
   * int feature_str_size
   * char *feature_str
   * 
   * int v_major
   * int v_minor
   * int v_micro
   * int v_patch
   * int v_build
   *
   * int flags
   */

  int datasize;
  int deviceaddr_datasize;
  int entdesc_datasize;
  char *data;
  char *dataptr;
  char *deviceaddr_data;
  char *entdesc_data;
  long msgid;

  if (!item) return -1;

  /* Convert data */

  deviceaddr_data = i_resource_address_struct_to_string (RES_ADDR(self));
  if (!deviceaddr_data)
  { i_printf (1, "i_inventory_register failed to convert self to deviceaddr_data"); return -1; }
  deviceaddr_datasize = strlen (deviceaddr_data)+1;

  entdesc_data = i_entity_descriptor_data (NULL, ENTITY(self->hierarchy->dev), &entdesc_datasize);
  if (!entdesc_data)
  { i_printf (1, "i_inventory_register failed to convert self->hierarchy->dev to entdesc_data"); free (deviceaddr_data); return -1; }

  /* Calculate datasize and malloc */

  datasize = (15*(sizeof(int))) + deviceaddr_datasize + entdesc_datasize;
  if (item->vendor_str) datasize += strlen(item->vendor_str)+1; 
  if (item->product_str) datasize += strlen(item->product_str)+1; 
  if (item->version_str) datasize += strlen(item->version_str)+1;
  if (item->serial_str) datasize += strlen(item->serial_str)+1;
  if (item->platform_str) datasize += strlen(item->platform_str)+1;
  if (item->feature_str) datasize += strlen(item->feature_str)+1;
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_inventory_register failed to malloc data"); free (deviceaddr_data); free (entdesc_data); return -1; }
  memset (data, 0, datasize);
  dataptr = data;

  /* Add chunks */

  dataptr = i_data_add_chunk (data, dataptr, datasize, deviceaddr_data, deviceaddr_datasize);
  free (deviceaddr_data);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add deviceaddr_data chunk to data"); free (entdesc_data); free (data); return -1; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, entdesc_data, entdesc_datasize);
  free (entdesc_data);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add entdesc_data to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &item->type);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add type integer to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, item->vendor_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add vendor_str to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, item->product_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add product_str to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, item->version_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add version_str to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, item->serial_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add serial_str to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, item->platform_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add platform_str to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, item->feature_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add feature_str to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &item->v_major);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add v_major to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &item->v_minor);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add v_minor to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &item->v_micro);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add v_micro to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &item->v_patch);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add v_patch to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &item->v_build);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add v_build to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &item->flags);
  if (!dataptr)
  { i_printf (1, "i_inventory_register failed to add flags to data"); free (data); return -1; }

  /* Send message */

  msgid = i_message_send (self, MSG_INV_REGISTER, data, datasize, self->hierarchy->customer_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_inventory_register failed to send MSG_INV_REGISTER message"); return -1; }

  return msgid;
}

/* Deregister item */

long i_inventory_deregister (i_resource *self, i_resource_address *customer_addr, i_resource_address *device_addr, int type, char *vendor_str, char *product_str, char *verser_str)
{
  /* Send a request to the customer resource to 
   * de-register the specified inventory item. If no
   * vendor is specified, all registrations for that device are 
   * removed. If no product is supplied, all products for that vendor
   * are removed. If no verser (version/serial) string is supplied 
   * then all entries for that vendor and product are removed.
   *
   * Data format:
   *
   * int deviceaddr_datasize;
   * char *deviceaddr_data;
   *
   * int type;
   * 
   * int vendor_str_datasize;
   * char *vendor_str;
   * int product_str_datasize;
   * char *product_str;
   * int verser_str_datasize;
   * char *verser_str;          Either a version string or serial string
   *                            depending on the type.  
   */

  int datasize;
  int deviceaddr_datasize;
  long msgid;
  char *data;
  char *dataptr;
  char *deviceaddr_data;

  deviceaddr_data = i_resource_address_struct_to_string (device_addr);
  if (!deviceaddr_data)
  { i_printf (1, "i_inventory_deregister failed to convert device_addr to deviceaddr_data"); return -1; }
  deviceaddr_datasize = strlen (deviceaddr_data)+1;

  datasize = (5*(sizeof(int))) + deviceaddr_datasize;
  if (vendor_str) datasize += strlen (vendor_str)+1;
  if (product_str) datasize += strlen (product_str)+1;
  if (verser_str) datasize += strlen (verser_str)+1;

  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_inventory_deregister failed to malloc data"); free (deviceaddr_data); return -1; }
  memset (data, 0, datasize);
  dataptr = data;

  dataptr = i_data_add_chunk (data, dataptr, datasize, deviceaddr_data, deviceaddr_datasize);
  free (deviceaddr_data);
  if (!dataptr)
  { i_printf (1, "i_inventory_deregister failed to add deviceaddr_data to data"); free (data); return -1; }

  dataptr = i_data_add_int (data, dataptr, datasize, &type);
  if (!dataptr)
  { i_printf (1, "i_inventory_deregister failed to add item type to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, vendor_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_deregister failed to add vendor_str to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, product_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_deregister failed to add product_str to data"); free (data); return -1; }

  dataptr = i_data_add_string (data, dataptr, datasize, verser_str);
  if (!dataptr)
  { i_printf (1, "i_inventory_deregister failed to add verser_str to data"); free (data); return -1; }

  /* Send message */

  msgid = i_message_send (self, MSG_INV_DEREGISTER, data, datasize, customer_addr, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid < 0)
  { i_printf (1, "i_inventory_deregister failed to send MSG_INV_DEREGISTER message"); return -1; }

  return msgid;
}

/* Deregister by supplying the item */

long i_inventory_deregister_item (i_resource *self, i_resource_address *customer_addr, i_resource_address *device_addr, i_inventory_item *item)
{
  char *verser_str;
  
  if (item->type & INV_HARDWARE)
  { verser_str = item->serial_str; }
  else if (item->type & INV_SOFTWARE)
  { verser_str = item->version_str; }
  else
  { verser_str = NULL; }
  
  return i_inventory_deregister (self, customer_addr, device_addr, item->type, item->vendor_str, item->product_str, verser_str);
}
