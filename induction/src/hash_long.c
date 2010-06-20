#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "induction.h"
#include "hashtable.h"

unsigned long i_hash_long (long value, unsigned long htsize)
{
  unsigned long hash;

  hash = (value % htsize-1) + 1;
  
  if (hash >= htsize)
  {
    i_printf (1, "HT_FAILURE : i_hash_long calculated invalid hash %li (htsize = %i), returning fail-safe hash of 0\n", hash, htsize);
    return 0;
  }

  return hash;
}

i_hashtable_key* i_hashtable_create_key_long (long value, unsigned long htsize)
{
	i_hashtable_key *key;

	key = i_hashtable_create_key (&value, sizeof(long));
	if (!key) return NULL;
	
	key->hash = i_hash_long (value, htsize);
	
	return key;
}
