#include <stdlib.h>
#include <unistd.h>
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
#include <induction/entity.h>
#include <induction/metric.h>

#include "procregistry.h"

l_procregistry_entry* l_procregistry_entry_create ()
{
  l_procregistry_entry *entry;

  entry = (l_procregistry_entry *) malloc (sizeof(l_procregistry_entry));
  if (!entry)
  { i_printf (1, "l_procregistry_entry_create failed to malloc entry"); return NULL; }
  memset (entry, 0, sizeof(l_procregistry_entry));

  return entry;
}

void l_procregistry_entry_free (void *entryptr)
{
  l_procregistry_entry *entry = entryptr;

  if (entry->objent) i_entity_descriptor_free (entry->objent);
  if (entry->devaddr) i_resource_address_free (entry->devaddr);

  free (entry);
}