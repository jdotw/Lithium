#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "induction.h"
#include "list.h"
#include "config.h"

/* Configuration Keys */

/* Struct manipulation */

i_config_key* i_config_key_create (char *name_str, char *desc_str, unsigned short val_type)
{
  i_config_key *key;

  key = (i_config_key *) malloc (sizeof(i_config_key));
  if (!key)
  { i_printf (1, "i_config_key_create failed to malloc i_config_key struct"); return NULL; }
  memset (key, 0, sizeof(i_config_key));

  key->name_str = strdup (name_str);
  i_name_parse (key->name_str);
  key->desc_str = strdup (desc_str);
  key->enumstr_list = i_list_create ();
  i_list_set_destructor (key->enumstr_list, i_value_enumstr_free);
  key->val_type = val_type;

  return key;
}

void i_config_key_free (void *keyptr)
{
  i_config_key *key = keyptr;

  if (!key) return;

  if (key->name_str) free (key->name_str);
  if (key->desc_str) free (key->desc_str);
  if (key->enumstr_list) i_list_free (key->enumstr_list);
  if (key->unit_str) free (key->unit_str);

  free (key);
}

