#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
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
#include <induction/entity.h>
#include <induction/metric.h>
#include <induction/object.h>
#include <induction/timer.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

l_ipregistry_entry* l_ipregistry_entry_create ()
{
  l_ipregistry_entry *entry;

  entry = (l_ipregistry_entry *) malloc (sizeof(l_ipregistry_entry));
  if (!entry)
  { i_printf (1, "l_ipregistry_entry_create failed to malloc entry"); return NULL; }
  memset (entry, 0, sizeof(l_ipregistry_entry));

  entry->device_table = i_hashtable_create (10);
  i_hashtable_set_destructor (entry->device_table, l_ipregistry_entry_device_free);
  
  return entry;
}

void l_ipregistry_entry_free (void *entryptr)
{
  l_ipregistry_entry *entry = entryptr;

  if (entry->ip_str) free (entry->ip_str);
  if (entry->mask_str) free (entry->mask_str);
  if (entry->network_str) free (entry->network_str);
  if (entry->dns_str) free (entry->dns_str);
  if (entry->device_table) i_hashtable_free (entry->device_table);
  if (entry->dead_timer) i_timer_remove (entry->dead_timer);

  free (entry);
}

l_ipregistry_entry_device* l_ipregistry_entry_device_create ()
{
  l_ipregistry_entry_device *dev;

  dev = (l_ipregistry_entry_device *) malloc (sizeof(l_ipregistry_entry_device));
  memset (dev, 0, sizeof(l_ipregistry_entry_device));

  return dev;
}

void l_ipregistry_entry_device_free (void *devptr)
{
  l_ipregistry_entry_device *dev = devptr;

  if (!dev) return;
  if (dev->ipent) i_entity_descriptor_free (dev->ipent);
  if (dev->iface) i_entity_descriptor_free (dev->iface);

  free (dev);
}
