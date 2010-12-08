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
      
#include "procregistry.h"
        
int l_procregistry_remove (i_resource *self, i_resource_address *devaddr, i_entity_descriptor *objent)
{       
  /* Remove entries from the procregistry tables */
        
  i_list *device_entries;
  i_hashtable *device_table;
  i_hashtable *obj_table;
  i_hashtable_key *key;
  l_procregistry_entry *entry;

  /* Retrieve tables */  
  device_table = l_procregistry_table_device ();
  obj_table = l_procregistry_table_object ();

  /* Firstly, find the entry in the ip_table to which
   * the specified ipent refers to, if one was specified.
   */

  if (objent)
  {
    char *entaddr_str = i_entity_address_string (NULL, (i_entity_address *)objent);
    key = i_hashtable_create_key_string (entaddr_str, obj_table->size);
    if (key)
    {
      i_hashtable_remove (obj_table, key);
    }
    i_hashtable_free_key (key);
  }


  /* 
   * Remove relevant entries from the device_table
   */
  
  /* Find the device's entry in the device_table */
  key = i_hashtable_key_resource (devaddr, device_table->size);
  if (!key)
  { i_printf (1, "l_procregistry_remove failed to create key for device_table"); return -1; }
  
  device_entries = i_hashtable_get (device_table, key);
  if (!device_entries)
  { i_printf (1, "l_procregistry_remove failed, no entries for specified device"); i_hashtable_free_key (key); return -1; }

  /* Loop through each device entry */
  for (i_list_move_head(device_entries); (entry=i_list_restore(device_entries))!=NULL; i_list_move_next(device_entries))
  {
    if (objent)
    {
      /* This is a specific object remove operation */
      if (strcmp(entry->objent->name_str, objent->name_str)==0)
      { i_list_delete (device_entries); }
    }
    else
    {
      /* This is a remove-all-objects operation */
      char *entaddr_str = i_entity_address_string (NULL, (i_entity_address *)entry->objent);
      i_hashtable_key *obj_key = i_hashtable_create_key_string (entaddr_str, obj_table->size);
      if (obj_key)
      { 
        i_hashtable_remove (obj_table, obj_key); 
        i_hashtable_free_key (obj_key);
      }
      i_list_delete (device_entries);
    }
  }

  if (device_entries->size < 1)
  {
    /* Device entries is now empty, remove from hashtable */
    i_hashtable_remove (device_table, key);
  }

  i_hashtable_free_key (key); 
  
  return 0;
}

