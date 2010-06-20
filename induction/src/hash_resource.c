#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "induction.h"
#include "hashtable.h"

i_hashtable_key* i_hashtable_key_resource (void *res_addr_ptr, unsigned long htsize)
{
  char *addr_str;
  i_resource_address *addr = res_addr_ptr;
  i_hashtable_key *key;

  if (!addr) return NULL;

  addr_str = i_resource_address_struct_to_string (addr);
  if (!addr_str) 
  { i_printf (1, "i_hashtable_key_resource failed to create addr_str from addr"); return NULL; }

  key = i_hashtable_create_key_string (addr_str, htsize);
  free (addr_str);
  if (!key) 
  { i_printf (1, "i_hashtable_key_resource failed to create key from addr_str"); return NULL; }
  
  return key;
}
