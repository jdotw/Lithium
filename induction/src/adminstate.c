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
#include "incident.h"
#include "adminstate.h"

/** \addtogroup entity_state Entity State
 * @{
 */

int i_adminstate_change (i_resource *self, i_entity *ent, unsigned short state)
{
  /* Changes the admin state of the specified entity and
   * also acts upon that change in state with respect
   * to the entity's child states.
   */

  void *p = NULL;
  i_entity *child;

  /* Set admin State */
  ent->adminstate = state;
  if (ent->authorative) ent->version = time(NULL);

  /* Change operational state to UNKNOWN */
  i_opstate_change (self, ent, ENTSTATE_UNKNOWN);

  /* Entity-specific handling */
  i_metric *met = NULL;
  switch (ent->ent_type)
  {
    case ENT_METRIC:
    case ENT_TRIGGER:
      if (state == ENTADMIN_DISABLED)
      {
        /* Clear any operational incident */
        if (ent->ent_type == ENT_METRIC) met = (i_metric *) ent;
        else if (ent->ent_type == ENT_TRIGGER) met = (i_metric *) ent->parent;
        if (met && met->op_inc)
        { 
          i_incident_clear (self, ENTITY(met), met->op_inc); met->op_inc = NULL; 
        }
      }
      else if (state == ENTADMIN_ENABLED)
      {
        /* Re-evaluate triggers now that this trigger is enabled */

      }
      /* Dont break, follow to default action */
    default:
      /* By default, all child entities inherit the new admin state */
      
      /* Save list pointer */
      if (ent->child_list)
      { p = ent->child_list->p; }
      
      /* Set the admin state of each child to the new admin state value. */
      for (i_list_move_head(ent->child_list); (child=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
      { i_adminstate_change (self, child, state); }

      /* Restore list pointer */
      if (ent->child_list && p)
      { ent->child_list->p = p; }
  }

  return 0;
}

/* @} */
