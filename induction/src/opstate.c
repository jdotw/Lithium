#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "form.h"
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
#include "opstate.h"
#include "devnormal.h"

/*
 * CEMent Entity State
 */

/** \addtogroup entity_state Entity State
 * @ingroup entity
 * @{
 */

int i_opstate_change (i_resource *self, i_entity *ent, short state)
{
  /* Changes the state of the specified entity and
   * also acts upon that change in state with respect
   * to the entity's parent states.
   */

  short parent_opstate;

  /* Check registration */
  if (ent->registered == 0) return -1;

  /* Check marking */
  if (ent->ent_type == ENT_DEVICE && ent->opstate <= ENTSTATE_TESTING) return 0;

  /* Check for devnormal handling */
  if (self->type == RES_DEVICE && ent->ent_type == ENT_DEVICE && ent->opstate < ENTSTATE_NORMAL && state == ENTSTATE_NORMAL)
  { i_devnormal_report (self); }
  
  /* Set State */
  ent->opstate = state;
  if (ent->authorative) ent->version = time (NULL);

  /* Entity-specific handling */
  switch (ent->ent_type)
  {
    case ENT_CUSTOMER:
      /* Entity state inheritance stops at
       * the customer entity level. Break and
       * do not perform further inheritance
       * regardless of the type of the local
       * resource.
       */
      break;
    case ENT_DEVICE:
      /* Local entity state inheritance for device
       * resources stops at the device entity level. 
       * Break and do not perform further inheritance
       * only if the local resource type is RES_DEVICE
       */

      if (self->type == RES_DEVICE) break;
    default:
      /* By default, unless break is called above, 
       * perform entity state inheritance.
       */

      /* Evaluate parents state after child state change */
      parent_opstate = i_opstate_eval (self, ent->parent);

      /* If the evaluated state doesnt equal the current
       * state of the parent, the parent's opstate should
       * be changed but only if the evaluated state is greater
       * than the admin state of the parent entity
       */
      if (parent_opstate != ent->parent->opstate)
      {
        /* Parent's state need to be adjusted */
        i_opstate_change (self, ent->parent, parent_opstate); 
      }
  }

  return 0;
}

short i_opstate_eval (i_resource *self, i_entity *ent)
{
  /* Evaluates the opstate of the specified entity.
   * OR'd opstate flags are ignored
   */
  void *p = NULL;
  i_entity *child;
  short opstate = ENTSTATE_UNKNOWN;

  /* Save list pointer */
  if (ent->child_list)
  { p = ent->child_list->p; }

  for (i_list_move_head(ent->child_list); (child=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
  {
    /* Check child's state */
    if (child->opstate > opstate)
    { opstate = child->opstate; }
  }

  /* Restore list pointer */
  if (ent->child_list && p)
  { ent->child_list->p = p; }

  return opstate;
}

/* @} */
