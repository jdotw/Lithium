#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "name.h"
#include "hashtable.h"
#include "callback.h"
#include "cement.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "entity.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * CEMent Entities - Child Entity Related Functions
 */

/* Child Entity Retrieval */

i_entity* i_entity_child_get (i_entity *parent, char *childname_str)
{
  /* This function retrieves the child entity with the
   * unique name of childname_str from the parent 
   * entity
   */

  i_hashtable_key *key;
  i_entity *child;

  if (!parent || !parent->child_ht || !childname_str) return NULL;

  /* Create key */
  key = i_hashtable_create_key_string (childname_str, parent->child_ht->size);

  /* Get child */
  child = (i_entity *) i_hashtable_get (parent->child_ht, key);

  /* Free key */
  i_hashtable_free_key (key);

  return child;
}

/* Sort Function */

int i_entity_child_sortfunc (void *curptr, void *nextptr)
{
  i_entity *cur = curptr;
  i_entity *next = nextptr;

  if (next->prio > cur->prio) return 1;
  
  return 0;
}

/* @} */
