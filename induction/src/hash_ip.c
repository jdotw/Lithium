#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "induction.h"
#include "hashtable.h"

unsigned long i_hash_ip (unsigned long int srcaddr, unsigned short int srcport, 
		unsigned long int dstaddr, unsigned short int dstport,
		unsigned long htsize)
{
  unsigned long hash;

  hash = ((srcaddr ^ srcport) ^ (dstaddr ^ dstport));
  hash ^= hash>>16;
  hash ^= hash>>8;

  hash = hash & (htsize - 1);
  
  if (hash >= htsize)
  {
    i_printf (1, "HT_FAILURE : i_hash_ip calculated an invalid hash of %li (htsize is %li). Returning failsafe hash of 0", hash, htsize);
    return 0;
  }

  return (hash);
}

i_hashtable_key* i_hashtable_key_ip (unsigned long int srcaddr, unsigned short int srcport,
					unsigned long int dstaddr, unsigned short int dstport,
					unsigned long htsize)
{
	int keysize;
	char *keydata;
	char *dataptr;
	i_hashtable_key *key;

	keysize = 2*(sizeof(unsigned long int)) + 2*(sizeof(unsigned short int));
	keydata = (char *) malloc (keysize);
	if (!keydata) return NULL;
	dataptr = keydata;

	memcpy (dataptr, &srcaddr, sizeof(unsigned long int));
	dataptr += sizeof(unsigned long int);
	memcpy (dataptr, &srcport, sizeof(unsigned short int));
	dataptr += sizeof(unsigned short int);
	memcpy (dataptr, &dstaddr, sizeof(unsigned long int));
	dataptr += sizeof(unsigned long int);
	memcpy (dataptr, &dstport, sizeof(unsigned short int));
	dataptr += sizeof(unsigned short int);

	key = i_hashtable_create_key (keydata, keysize);
	free (keydata);
	if (!key) return NULL;

	key->hash = i_hash_ip (srcaddr, srcport, dstaddr, dstport, htsize);

	return key;
}	
