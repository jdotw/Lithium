#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "entity.h"
#include "hashtable.h"
#include "container.h"
#include "metric.h"
#include "navtree.h"
#include "object.h"

/** \addtogroup object Objects
 * @ingroup entity
 * @{
 */

/* 
 * CEMent Objects
 */

/* Struct manipulation */

i_object* i_object_create (char *name_str, char *desc_str)
{
  i_object *obj;

  obj = (i_object *) i_entity_create (name_str, desc_str, ENT_OBJECT, sizeof(i_object));
  if (!obj)
  { i_printf (1, "i_object_create failed to create object entity"); return NULL; }
  obj->refresh_func = i_entity_refresh_children;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;
  obj->authorative = 1;

  return obj;
}

void i_object_free (void *objptr)
{
  /* Just free the object-specific 
   * portions of the struct. The struct
   * and everything else will be freed
   * by i_entity_free
   */
  i_object *obj = objptr;
  
  if (!obj) return;

  if (obj->cgraph_list) i_list_free (obj->cgraph_list);
  if (obj->tset_list) i_list_free (obj->tset_list);
  if (obj->tset_ht) i_hashtable_free (obj->tset_ht);
}

/* 
 * Object Specific Registration/Deregistration 
 * (Should only be called by i_entity_register and i_entity_register)
 */

int i_object_register (i_resource *self, i_container *cnt, i_object *obj)
{
  /* Stub - Nothing done here */
  return 0;
}

int i_object_deregister (i_resource *self, i_object *obj)
{
  /* Stub - Nothing done here */
  return 0;
}

/* @} */
