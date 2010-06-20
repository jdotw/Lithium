#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "hashtable.h"
#include "timer.h"
#include "name.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "triggerset.h"

/** \addtogroup triggerset Trigger Sets
 * @ingroup trigger
 * @{
 */

/* 
 * CEMent Trigggers
 */

/* Struct manipulation */

i_triggerset* i_triggerset_create (char *name_str, char *desc_str, char *metname_str)
{
  i_triggerset *tset;

  tset = (i_triggerset *) malloc (sizeof(i_triggerset));
  if (!tset)
  { i_printf (1, "i_triggerset_create failed to malloc tset"); return NULL; }
  memset (tset, 0, sizeof(i_triggerset));
  
  tset->name_str = strdup (name_str);
  i_name_parse (tset->name_str);
  tset->desc_str = strdup (desc_str);
  tset->metname_str = strdup (metname_str);
  tset->obj_list = i_list_create ();
  if (!tset->obj_list)
  { i_printf (1, "i_triggerset_create failed to create obj_list"); i_triggerset_free (tset); return NULL; }
  tset->trg_list = i_list_create ();
  if (!tset->trg_list)
  { i_printf (1, "i_triggerset_create failed to create trg_list"); i_triggerset_free (tset); return NULL; }
  i_list_set_destructor (tset->trg_list, i_entity_free);
  tset->default_applyflag = 1;

  return tset;
}

void i_triggerset_free (void *tsetptr)
{
  i_triggerset *tset = tsetptr;

  if (!tset) return;

  if (tset->name_str) free (tset->name_str);
  if (tset->desc_str) free (tset->desc_str);
  if (tset->metname_str) free (tset->metname_str);
  if (tset->obj_list) i_list_free (tset->obj_list);
  if (tset->trg_list) i_list_free (tset->trg_list);

  free (tset);
}

/* Assignment 
 *
 * Trigger set assignment is the process of associating a
 * trigger set with a particular cement container
 */

int i_triggerset_assign (i_resource *self, i_container *cnt, i_triggerset *tset)
{
  int num;
  i_hashtable_key *key;

  /* Set container */
  tset->cnt = cnt;

  /* Enqueue trigger set */
  num = i_list_enqueue (cnt->tset_list, tset);
  if (num != 0)
  { i_printf (1, "i_triggerset_assign failed to enqueue tset into container %s's tset_list", cnt->name_str); return -1; }

  /* Add trigger set to hashtable 
   *
   * The trigger set is added to the hashtable
   * using a key string based on the trigger sets
   * metname_str, not the trigger sets name_str.
   */ 
  key = i_hashtable_create_key_string (tset->metname_str, cnt->tset_ht->size);
  num = i_hashtable_put (cnt->tset_ht, key, tset);
  i_hashtable_free_key (key);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_assign failed to put tset into container %s's tset_ht", cnt->name_str);
    num = i_list_search (cnt->tset_list, tset);
    if (num == 0) i_list_delete (cnt->tset_list);
    return -1; 
  }

  /* Apply the trigger set to all existing objects */
//  num = i_triggerset_apply_allobjs (self, cnt, tset);
//  if (num != 0)
//  { 
//    i_printf (1, "i_triggerset_assign warning, failed to apply triggerset %s to all existing objects in container %s",
//      tset->name_str, cnt->name_str);
//  }

  return 0;
}

int i_triggerset_assign_obj (i_resource *self, i_object *obj, i_triggerset *tset)
{
  int num;
  i_hashtable_key *key;

  if (!obj->tset_list || !obj->tset_ht)
  { i_printf (1, "i_triggerset_assign_obj failed, object %s has no tset_ht or tset_list", obj->name_str); return -1; }

  /* Set container */
  tset->cnt = obj->cnt;

  /* Enqueue trigger set */
  i_list_enqueue (obj->tset_list, tset);

  /* Add trigger set to hashtable 
   *
   * The trigger set is added to the hashtable
   * using a key string based on the trigger sets
   * metname_str, not the trigger sets name_str.
   */ 
  key = i_hashtable_create_key_string (tset->metname_str, obj->tset_ht->size);
  num = i_hashtable_put (obj->tset_ht, key, tset);
  i_hashtable_free_key (key);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_assign failed to put tset into object %s's tset_ht", obj->name_str);
    num = i_list_search (obj->tset_list, tset);
    if (num == 0) i_list_delete (obj->tset_list);
    return -1; 
  }

  /* Apply the trigger set to all existing objects */
//  num = i_triggerset_apply (self, obj, tset);
//  if (num != 0)
//  { 
//    i_printf (1, "i_triggerset_assign warning, failed to apply triggerset %s to all existing objects in container %s",
//      tset->name_str, obj->name_str);
//  }

  return 0;
}

int i_triggerset_unassign (i_resource *self, i_container *cnt, i_triggerset *tset)
{
  int num;
  i_hashtable_key *key;
    
  /* Remove trigger set from list */
  num = i_list_search (cnt->tset_list, tset);
  if (num == 0) i_list_delete (cnt->tset_list);

  /* Remove trigger set from ht */
  key = i_hashtable_create_key_string (tset->metname_str, cnt->tset_ht->size);
  i_hashtable_remove (cnt->tset_ht, key);
  i_hashtable_free_key (key);

  /* Strip the triggerset from all existing objects */
  i_triggerset_strip_allobjs (self, cnt, tset);

  return 0;
}
/* @} */
