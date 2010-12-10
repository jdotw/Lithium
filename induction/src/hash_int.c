#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "induction.h"
#include "hashtable.h"

extern i_resource *global_self;

unsigned long i_hash_int (int integer, unsigned long htsize)
{
  unsigned long hash;

  hash = integer % htsize;
  
  if (hash >= htsize) 
  {
    i_printf (1, "HT_FAILURE : i_hash_int calculated an invalid hash of %li (htsize = %li). Returning failsafe hash of 0", hash, htsize);
    return 0;
  }
  
  return hash;
}

i_hashtable_key* i_hashtable_create_key_int (int integer, unsigned long htsize)
{
  i_hashtable_key *key;

  key = i_hashtable_create_key (&integer, sizeof(integer));
  if (!key) return NULL;
	
  key->hash = i_hash_int (integer, htsize);
	
  return key;
}
