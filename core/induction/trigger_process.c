#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "cement.h"
#include "entity.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "opstate.h"
#include "socket.h"
#include "message.h"
#include "incident.h"
#include "trigger.h"
#include "hierarchy.h"

/** \addtogroup trigger Triggers
 * @ingroup entity
 * @{
 */

/* 
 * CEMent Triggger Process
 */

int i_trigger_process (i_resource *self, i_trigger *trg)
{
  /* This function evaluates the specified trigger
   * against it's parent metrics CURRENT value and 
   * performs an opstate adjustment accordingly.
   *
   * Returns 0 if the trigger is inactive, or 1 if the 
   * trigger is active. 
   *
   * Incident reporting is handled in i_trigger_process_all
   */

  int active;

  /* Check trigger admin state */
  if ((trg->adminstate & ENTADMIN_ALL) == ENTADMIN_DISABLED)
  { return 0; }

  /* Evaluate trigger against value */
  active = i_trigger_eval (self, trg, NULL);

  /* Check if active */
  if (active == 1 && trg->active == 0)
  {
    /* 
     * Trigger is active
     */
    
    /* Set flag */
    trg->active = 1;

    if (trg->opstate != (short) trg->effect && trg->effect > ENTSTATE_NORMAL)
    {
      /* Set flag */
      trg->active = 1;

      /* Change opstate of trigger to trigger's effect value */
      i_opstate_change (self, ENTITY(trg), trg->effect);
    }
  }
  else if (active == 0 && trg->opstate != ENTSTATE_NORMAL)
  {
    /* 
     * Trigger is inactive 
     */
    
    /* Set flag */
    trg->active = 0;

    /* Change opstate of trigger to normal value */
    i_opstate_change (self, ENTITY(trg), ENTSTATE_NORMAL);
  }

  return trg->active;
}

int i_trigger_process_all (i_resource *self, i_metric *met)
{
  /* This function loops through all triggers registered
   * to the specified metric and called i_trigger_process
   * for each trigger
   */
  int num;

  /* Process all triggers to determine opstate */
  i_trigger *trg;
  i_trigger *active_trigger = NULL;
  for (i_list_move_head(met->trg_list); (trg=i_list_restore(met->trg_list))!=NULL; i_list_move_next(met->trg_list))
  { 
    num = i_trigger_process (self, trg); 
    if (num == 1 && !active_trigger)
    { active_trigger = trg; }
  }

  /* Check for an active trigger */
  if (active_trigger)
  {
    /* Trigger is active, incident to be reported */
    if (met->op_inc && met->op_inc->local_trg != active_trigger)
    {
      /* An incident has already been reported off this trigger, 
       * and it was not for this trigger. 
       * 
       * This is an Incident Transition report
       */
      
      if (self->hierarchy->dev->mark > ENTSTATE_TESTING) i_incident_transition (self, met->op_inc, ENTITY(active_trigger));
    }
    else if (!met->op_inc)
    {
      /* No incident has been reported off this trigger.
       *
       * This is a new Incident Report
       */
      if (self->hierarchy->dev->mark > ENTSTATE_TESTING && active_trigger->opstate > ENTSTATE_NORMAL) 
      {
        met->op_inc = i_incident_report (self, INCTYPE_OPSTATE, ENTITY(active_trigger));
      }
    }
  }
  else 
  {
    if (met->op_inc)
    {
      /* No trigger active but an incident has been reported, clear it 
       *
       * NOTE: The fate of the incident struct is now handed over
       *       to i_incident_clear. The incident struct should not
       *       be freed here
       */
      if (self->hierarchy->dev->mark > ENTSTATE_TESTING) i_incident_clear (self, ENTITY(met), met->op_inc); 
      met->op_inc = NULL;
    }
  
    if (met->opstate != ENTSTATE_NORMAL && i_metric_curval (met)) 
    { i_opstate_change (self, ENTITY(met), ENTSTATE_NORMAL); }
  }

  return 0;
}

/* @} */
