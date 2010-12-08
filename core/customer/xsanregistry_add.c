#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "xsanregistry.h"

int l_xsanregistry_add (i_resource *self, l_xsanregistry_entry *entry)
{
  /* Add a new entry to the xsanregistry tables */

  int num;
  i_hashtable *device_table;
  i_hashtable *obj_table;
  i_hashtable_key *key;

  /* Retrieve tables */
  device_table = l_xsanregistry_table_device ();
  obj_table = l_xsanregistry_table_object ();

  /* 
   * Add volume entry to object table (if volume is specified)
   */

  if (entry->objent)
  {
    char *entaddr_str = i_entity_address_string (NULL, (i_entity_address *) entry->objent);
    if (entaddr_str)
    {
      key = i_hashtable_create_key_string (entaddr_str, obj_table->size);
      i_hashtable_put (obj_table, key, entry);
      i_hashtable_free_key (key);
      free (entaddr_str);
    }
  }
  
  /* 
   * Add entry to device table
   */

  key = i_hashtable_key_resource (entry->devaddr, device_table->size);
  if (key)
  {
    l_xsanregistry_device *device;
    device = (l_xsanregistry_device *) i_hashtable_get (device_table, key);
    if (!device)
    {
      device = l_xsanregistry_device_create (entry->devaddr, entry->devent, entry->devtype);
      if (device)
      { i_hashtable_put (device_table, key, device); }
      else
      { i_printf (1, "l_xsanregistry_add warning, failed to create list for device_table"); }
    }
    i_hashtable_free_key (key);
    key = NULL;

    if (entry->objent)
    {
      num = i_list_enqueue (device->entry_list, entry);
      if (num != 0)
      { i_printf (1, "l_xsanregistry_add warning, failed to enqueue entry to device_table list"); }
    }
  }
  else
  { i_printf (1, "l_xsanregistry_add warning, failed to create key for device_table"); }


  /* Finished */

  return 0;
}

