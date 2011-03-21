#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

/* Get list of entries for an IP */

l_ipregistry_entry* l_ipregistry_get_ip_str (i_resource *self, char *ip_str)
{
  int num;
  struct in_addr ip_addr;

  num = inet_aton (ip_str, &ip_addr);
  if (num == 0)
  { i_printf (1, "l_ipregistry_get_ip_str failed to convert ip_str to ip_addr"); return NULL; }

  return l_ipregistry_get_ip (self, ip_addr.s_addr);
}

l_ipregistry_entry* l_ipregistry_get_ip (i_resource *self, unsigned long int ip)
{
  i_hashtable *ip_table;
  i_hashtable_key *key;

  ip_table = l_ipregistry_table_ip ();
  if (!ip_table)
  { i_printf (1, "l_ipregistry_get_ip failed to get l_ipregistry IP table"); return NULL; }

  key = i_hashtable_key_ip (ip, 0, 0, 0, ip_table->size);
  if (!key)
  { i_printf (1, "l_ipregistry_get_ip failed to create hashtable key for ip"); return NULL; }

  l_ipregistry_entry *entry = i_hashtable_get (ip_table, key);
  i_hashtable_free_key (key);

  return entry;
}

/* Get IP entry device struct */

l_ipregistry_entry_device* l_ipregistry_get_ip_device (i_resource *self, l_ipregistry_entry *entry, i_resource_address *devaddr)
{
  i_hashtable_key *key;

  key = i_hashtable_key_resource (devaddr, entry->device_table->size);
  if (!key)
  { i_printf (1, "l_ipregistry_get_ip_device failed to create hashtable key for devaddr"); return NULL; }

  l_ipregistry_entry_device *ipdev = i_hashtable_get (entry->device_table, key);
  i_hashtable_free_key (key);

  return ipdev;
}

/* Get list of entries for a Network */

l_ipregistry_network* l_ipregistry_get_network (i_resource *self, char *network_str, char *mask_str)
{
  char *str;
  l_ipregistry_network *network;
  i_hashtable *network_table;
  i_hashtable_key *key;
  
  network_table = l_ipregistry_table_network ();
  if (!network_table)
  { i_printf (1, "l_ipregistry_get_network failed to get l_ipregistry Network table"); return NULL; }

  asprintf (&str, "%s/%s", network_str, mask_str);
  key = i_hashtable_create_key_string (str, network_table->size);
  free (str);
  if (!key)
  { i_printf (1, "l_ipregistry_get_network failed to create hashtable key for ip"); return NULL; }

  network = i_hashtable_get (network_table, key);
  i_hashtable_free_key (key);

  return network;
}

/* Get list of entries for a device */

i_list* l_ipregistry_get_device_str (i_resource *self, char *device_addr_str)
{
  i_list *list;
  i_resource_address *device_addr;

  device_addr = i_resource_address_string_to_struct (device_addr_str);
  if (!device_addr)
  { i_printf (1, "l_ipregistry_get_device_str failed to convert device_addr_str to device_addr struct"); return NULL; }

  list = l_ipregistry_get_device (self, device_addr);
  i_resource_address_free (device_addr);

  return list;
}

i_list* l_ipregistry_get_device (i_resource *self, i_resource_address *device_addr)
{
  i_list *list;
  i_hashtable *device_table;
  i_hashtable_key *key;

  device_table = l_ipregistry_table_device ();
  if (!device_table)
  { i_printf (1, "l_ipregistry_get_device failed to get l_ipregistry Device table"); return NULL; }

  key = i_hashtable_key_resource (device_addr, device_table->size);
  if (!key)
  { i_printf (1, "l_ipregistry_get_device failed to create key for device_addr"); return NULL; }

  list = i_hashtable_get (device_table, key);
  i_hashtable_free_key (key);

  return list;
}


