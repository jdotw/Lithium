#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "hierarchy.h"
#include "list.h"
#include "timer.h"
#include "name.h"
#include "hashtable.h"
#include "callback.h"
#include "cement.h"
#include "customer.h"
#include "site.h"
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
 * CEMent Entities
 *
 * Functions for manipulating entities that local
 * (i.e reside within) the current resource.
 */

i_entity* i_entity_local_get (i_resource *self, i_entity_address *addr)
{
  /* This function attempts to retrieve the local entity
   * struct that is referred to by the entity address
   */
  int i;
  i_entity *ent = NULL;

  if (!addr) return NULL;
  if (!self->hierarchy || !self->hierarchy->cust) return NULL;
  
  /* Loop through the address levels to find
   * the requested entity. The customer,
   * site, device levels really only apply
   * when this func is called from within a customer, 
   * site or device resource and it is that root entity 
   * being requested. The true purpose of this loop
   * is to find cnt, obj, met and trg entities
   * that are burried in their hierarchies
   */

  for (i=0; i < addr->type; i++)
  {
    i_entity *child = NULL;

    switch (i)
    {
      case 0:       /* Customer Level */
        if (strcmp(self->hierarchy->cust->name_str, addr->cust_name) != 0)
        { return NULL; }
        child = ENTITY(self->hierarchy->cust);
        break;
      case 1:       /* Site Level */
        child = i_entity_child_get (ent, addr->site_name);
        break;
      case 2:       /* Device Level */
        child = i_entity_child_get (ent, addr->dev_name);
        break;   
      case 3:       /* Container Level */
        child = i_entity_child_get (ent, addr->cnt_name);
        break;
      case 4:       /* Object Level */
        child = i_entity_child_get (ent, addr->obj_name);
        break;
      case 5:       /* Metric Level */
        child = i_entity_child_get (ent, addr->met_name);
        break;
      case 6:       /* Trigger Level */
        child = i_entity_child_get (ent, addr->trg_name);
        break;
    }

    ent = child;
    if (ent == NULL) return NULL;
  }

  return ent;
}

i_entity* i_entity_local_get_str (i_resource *self, char *addrstr)
{
  /* This function attempts to retrieve the local entity
   * struct that is referred to by the entity address
   * string. The string is first converted into an 
   * address struct and i_entity_local_get is then called.
   */
  i_entity *ent;
  i_entity_address *addr;

  addr = i_entity_address_struct (addrstr);
  ent = i_entity_local_get (self, addr);
  i_entity_address_free (addr);

  return ent;
}

/* @} */
