#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "hashtable.h"
#include "timer.h"
#include "cement.h"
#include "customer.h"
#include "site.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "entity.h"
#include "name.h"
#include "search.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/* Register an entity */

int i_entity_register (i_resource *self, i_entity *parent, i_entity *ent)
{
  int num;
  i_hashtable_key *key;

  /* Check for name and desc */
  if (!ent->name_str || !ent->desc_str)
  { i_printf (1, "i_entity_register failed, name (%s) or desc (%s) was null for parent %s", ent->name_str, ent->desc_str, parent->name_str); return -1; }
  ent->desc_esc_str = strdup (ent->desc_str);
  i_name_parse (ent->desc_esc_str);

  /* Ensure there is no duplicate */
  if (parent && parent->child_ht && i_entity_child_get (parent, ent->name_str))
  { i_printf (1, "i_entity_register WARNING duplicate '%i:%s' entity already existing in '%i:%s'", ent->ent_type, ent->name_str, parent->ent_type, parent->name_str); }

  /* Set UUID if needed */
  if (uuid_is_null(ent->uuid) == 1) uuid_generate (ent->uuid);

  /* Set parent */
  ent->parent = parent;
  
  /* Check/Create child_list */
  if (!parent->child_list)
  { 
    parent->child_list = i_list_create ();
    if (!parent->child_list)
    { i_printf (1, "i_entity_register failed to create parent->child_list"); return -1; }
  }
    
  /* Check/Create child_ht */
  if (!parent->child_ht)
  {
    parent->child_ht = i_hashtable_create (ENT_CHILDHT_SIZE);
    if (!parent->child_ht)
    { i_printf (1, "i_entity_register failed to create parent->child_ht"); return -1; }
  }

  /* Enqueue into parents child list */
  num = i_list_enqueue (parent->child_list, ent);
  if (num != 0)
  { 
    i_printf (1, "i_entity_register failed to enqueue %s entity %s into parent %s entity %s child_list",
      i_entity_typestr (ent->ent_type), ent->name_str, i_entity_typestr (parent->ent_type), parent->name_str);
    i_entity_deregister (self, ent);
    return -1;
  }

  /* Add the child hashtable */
  key = i_hashtable_create_key_string (ent->name_str, parent->child_ht->size);
  num = i_hashtable_put (parent->child_ht, key, ent);
  if (num != 0)
  {
    i_printf (1, "i_entity_register warning, failed to add %s entity %s into parent %s entity %s child_ht",
      i_entity_typestr (ent->ent_type), ent->name_str, i_entity_typestr (parent->ent_type), parent->name_str);
  }
  i_hashtable_free_key (key);

  /* Update parents version */
  if (parent->authorative) parent->version = time (NULL);

  /* Assign SNMP Agent row id */
  ent->agent_rowindex = parent->agent_nextchildindex;
  parent->agent_nextchildindex++;

  /* Call entity-specific registration func */
  switch (ent->ent_type)
  {
    case ENT_CUSTOMER:
      num = i_customer_register (self, NULL, (i_customer *) ent);
      break;
    case ENT_SITE:
      num = i_site_register (self, (i_customer *) parent, (i_site *) ent);
      break;
    case ENT_DEVICE:
      num = i_device_register (self, (i_site *) parent, (i_device *) ent);
      break;
    case ENT_CONTAINER:
      num = i_container_register (self, (i_device *) parent, (i_container *) ent);
      break;
    case ENT_OBJECT:
      num = i_object_register (self, (i_container *) parent, (i_object *) ent);
      break;
    case ENT_METRIC:
      num = i_metric_register (self, (i_object *) parent, (i_metric *) ent);
      break;
    case ENT_TRIGGER:
      num = i_trigger_register (self, (i_metric *) parent, (i_trigger *) ent);
      break;
    default:
      num = 0;
  }
  if (num != 0)
  { 
    i_printf (1, "i_entity_register failed to call the entity-specific registration func for %s entity %s", 
      i_entity_typestr (ent->ent_type), ent->name_str);
    i_entity_deregister (self, ent);
    return -1;
  }

  /* Set registered flag */
  ent->registered = 1;

  /* Set version */
  if (ent->authorative) ent->version = time (NULL);

  /* Add to search cache */
  i_search_cache_insert (self, ent);

  return 0;
}

int i_entity_deregister (i_resource *self, i_entity *ent)
{
  int num;

  /* Deregister and free children */
  if (ent->child_list)
  {
    i_entity *child;
    for (i_list_move_head(ent->child_list); (child=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
    { 
      if (child->registered) i_entity_deregister (self, child); 
      i_entity_free (child);
    }
  }

  /* Remove from search cache */
  i_search_cache_delete (self, ent);
  
  /* Call entity-specific deregistration func */
  switch (ent->ent_type)
  {
    case ENT_CUSTOMER:
      num = i_customer_deregister (self, (i_customer *) ent);
      break;
    case ENT_SITE:
      num = i_site_deregister (self, (i_site *) ent);
      break;
    case ENT_DEVICE:
      num = i_device_deregister (self, (i_device *) ent);
      break;
    case ENT_CONTAINER:
      num = i_container_deregister (self, (i_container *) ent);
      break;
    case ENT_OBJECT:
      num = i_object_deregister (self, (i_object *) ent);
      break;
    case ENT_METRIC:
      num = i_metric_deregister (self, (i_metric *) ent);
      break;
    case ENT_TRIGGER:
      num = i_trigger_deregister (self, (i_trigger *) ent);
      break;
    default:
      num = 0;
  }
  if (num != 0)
  {
    i_printf (1, "i_entity_deregister warning, failed to call the entity-specific deregistration func for %s entity %s",
      i_entity_typestr (ent->ent_type), ent->name_str);
    return -1;
  }

  /* Set registered flah */
  ent->registered = 0;

  /* Remove from parents child_list */  
  if (ent->parent)
  {
    num = i_list_search (ent->parent->child_list, ent);
    if (num == 0)
    { i_list_delete (ent->parent->child_list); }

    /* Remove from parents child_ht */
    i_hashtable_key *key = i_hashtable_create_key_string (ent->name_str, ent->parent->child_ht->size);
    i_hashtable_remove (ent->parent->child_ht, key);
    i_hashtable_free_key (key);
  }

  /* If the entity's parent's refresh
   * func is i_entity_refresh_children then there 
   * is a CHANCE that this entity is in the child_list
   * of the parents refresh operation data (assuming
   * the parent is in the middle of a refresh). Hence,
   * if this chance is present, remove the child 
   * from the parents refresh op
   */
  if (ent->parent && ent->parent->refresh_func == i_entity_refresh_children)
  {
    i_entity_refresh_children_removechild (ent->parent, ent);
  }

  return 0;
}

/* @} */
