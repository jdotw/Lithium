#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "interface.h"
#include "ip.h"
#include "hierarchy.h"
#include "cement.h"
#include "entity.h"
#include "ipregistry.h"
#include "list.h"

i_ipregistry_entry* i_ipregistry_entry_create ()
{
  i_ipregistry_entry *entry;

  entry = (i_ipregistry_entry *) malloc (sizeof(i_ipregistry_entry));
  if (!entry)
  { i_printf (1, "i_ipregistry_entry_create failed to malloc entry"); return NULL; }
  memset (entry, 0, sizeof(i_ipregistry_entry));

  return entry;
}

void i_ipregistry_entry_free (void *entryptr)
{
  i_ipregistry_entry *entry = entryptr;

  if (entry->ip_str) free (entry->ip_str);
  if (entry->mask_str) free (entry->mask_str);
  if (entry->dns_str) free (entry->dns_str);
  if (entry->entities) i_list_free (entry->entities);
  if (entry->devices) i_list_free (entry->devices);

  free (entry);
}
