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

int l_procregistry_add (i_resource *self, l_procregistry_entry *entry)
{
  /* Add a new entry to the procregistry tables */

  int num;
  i_list *list;
  i_hashtable *device_table;
  i_hashtable *obj_table;
  i_hashtable_key *key;

  /* Retrieve tables */
  device_table = l_procregistry_table_device ();
  obj_table = l_procregistry_table_object ();

  /* 
   * Add entry to object table 
   */

  char *entaddr_str = i_entity_address_string (NULL, (i_entity_address *) entry->objent);
  if (entaddr_str)
  {
    key = i_hashtable_create_key_string (entaddr_str, obj_table->size);
    i_hashtable_put (obj_table, key, entry);
    i_hashtable_key_free (key);
    key = NULL;
    free (entaddr_str);
  }
  
  /* 
   * Add entry to device table
   */

  key = i_hashtable_key_resource (entry->devaddr, device_table->size);
  if (key)
  {
    list = i_hashtable_get (device_table, key);
    if (!list)
    {
      list = i_list_create ();
      if (list)
      { i_hashtable_put (device_table, key, list); }
      else
      { i_printf (1, "l_procregistry_add warning, failed to create list for device_table"); }
    }
    i_hashtable_free_key (key);
    key = NULL;

    num = i_list_enqueue (list, entry);
    if (num != 0)
    { i_printf (1, "l_procregistry_add warning, failed to enqueue entry to device_table list"); }
  }
  else
  { i_printf (1, "l_procregistry_add warning, failed to create key for device_table"); }


  /* Finished */

  return 0;
}

