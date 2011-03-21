#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/message.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/site.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>
#include <induction/incident.h>

#include "action.h"

/* Incident Interaction */

/* Reporting an incident */

int l_action_incident_report (i_resource *self, i_incident *inc)
{
  /* Confirm minimum severity is met */
  i_site *site = (i_site *) i_entity_child_get(ENTITY(self->hierarchy->cust), inc->met->site_name);
  if (!site) { i_printf (1, "l_action_incident_report failed to find site %s", inc->met->site_name); return 0; }
  i_device *dev = (i_device *) i_entity_child_get(ENTITY(site), inc->met->dev_name);
  if (!dev) { i_printf (1, "l_action_incident_report failed to find device %s", inc->met->dev_name); return 0; }
  if (inc->met->opstate < dev->minimum_action_severity)
  {
    i_printf (1, "l_action_incident_report suppressed action for incident %li as the severity %i is lower than the devices (%s) minimum_action_severity (%i)", 
      inc->id, inc->met->opstate, dev->desc_str,
      dev->minimum_action_severity);
    return 0;
  }

  /* Load list of candidates */
  if (inc->action_list)
  {
    /* Action list already exists */
    l_action_incident_candidatecb (self, inc->action_list, inc);
  }
  else
  {
    /* Load the action candidates */
    inc->action_cb = l_action_sql_load_candidates (self, inc, l_action_incident_candidatecb, inc);
    if (!inc->action_cb)
    { i_printf (1, "l_action_incident_report failed to load list of candidate actions"); return -1; }
  }

  return 0;
}

int l_action_incident_candidatecb (i_resource *self, i_list *list, void *passdata)
{
  i_incident *inc = passdata;

  /* Set list */
  inc->action_list = list;
  inc->action_cb = NULL;

  /* Go through each action */
  l_action *action;
  for (i_list_move_head(list); (action=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    /* Set incident */
    action->inc = inc;

    /* Check activation */
    if (action->activation == 0)
    { break; }

    /* Set flag */
    action->runstate = 1;       /* About to be run/re-run */
    
    /* Check delay */
    if (action->delay == 0)
    {
      /* Execute action immediately */
      l_action_incident_run_action (self, action);
    }
    else
    {
      /* Install delay timer */
      action->delay_timer = i_timer_add (self, action->delay, 0, l_action_incident_delaytimercb, action);
      if (!action->delay_timer)
      { i_printf (1, "l_action_incident_candidatecb failed to install delay_timer"); }
    }
  }
  
  return 0;   /* Keep the list!! */
}

int l_action_incident_delaytimercb (i_resource *self, i_timer *timer, void *passdata)
{
  l_action *action = passdata;

  /* NULL pointer */
  action->delay_timer = NULL;

  /* Run action */
  l_action_incident_run_action (self, action);

  return -1;    /* Always return -1 to kill off the delay timer */
}

int l_action_incident_run_action (i_resource *self, l_action *action)
{
  /* Remove delay timer if necessary */
  if (action->delay_timer)
  {
    i_timer_remove (action->delay_timer);
    action->delay_timer = NULL;
  }

  /* Execute action */
  if (action->inc->occurrences == 1 || action->run_count == 0)
  { l_action_exec (self, action->script_file, "report", action, action->inc, NULL, NULL); }
  else
  { l_action_exec (self, action->script_file, "report_recurring", action, action->inc, NULL, NULL); }

  /* Check if a re-run timer is needed */
  if (action->rerun_timer) 
  { 
    i_timer_remove (action->rerun_timer);
    action->rerun_timer = NULL;
    action->rerun_count = 0;
  }
  if (action->rerun == 1)
  {
    action->runstate = 1;
    action->rerun_timer = i_timer_add (self, action->rerun_delay * 60, 0, l_action_incident_reruntimercb, action); 
    if (!action->rerun_timer) 
    { i_printf (1, "l_action_incident_run_action failed to install rerun_timer"); }
  }
  else
  {
    /* Will not be rerun */
    action->runstate = 0;
  }

  return 0;
}

int l_action_incident_reruntimercb (i_resource *self, i_timer *timer, void *passdata)
{
  l_action *action = passdata;

  /* Update counter */
  action->rerun_count++;

  /* Re-Run an action */
  l_action_exec (self, action->script_file, "remind", action, action->inc, NULL, NULL);

  /* Check run count */
  if (action->rerun_max > 0 && action->rerun_count >= action->rerun_max)
  { 
    action->rerun_count = 0; 
    return -1;  /* Kill timer */
  }

  return 0;   /* Keep re-run timer alive */
}

/* Transitioning an incident */

int l_action_incident_transition (i_resource *self, i_incident *inc)
{
  /* Confirm minimum severity is met */
  i_site *site = (i_site *) i_entity_child_get(ENTITY(self->hierarchy->cust), inc->met->site_name);
  if (!site) { i_printf (1, "l_action_incident_report failed to find site %s", inc->met->site_name); return 0; }
  i_device *dev = (i_device *) i_entity_child_get(ENTITY(site), inc->met->dev_name);
  if (!dev) { i_printf (1, "l_action_incident_report failed to find device %s", inc->met->dev_name); return 0; }
  if (inc->highest_opstate < dev->minimum_action_severity)
  {
    i_printf (1, "l_action_incident_transition suppressed action for incident %li as the highest severity %i is lower than the devices (%s) minimum_action_severity (%i)", 
      inc->id, inc->highest_opstate, dev->desc_str,
      dev->minimum_action_severity);
    return 0;
  }

  /* Process each of the incidents candidate actions */
  l_action *action;
  for (i_list_move_head(inc->action_list); (action=i_list_restore(inc->action_list))!=NULL; i_list_move_next(inc->action_list))
  {
    /* Check run count */
    if (action->run_count > 0)
    {
      /* Script has run atleast once, so we need
       * to run it immediately with a 'transition' command 
       */
    
      l_action_exec (self, action->script_file, "transition", action, inc, NULL, NULL);
    }
  }

  return 0;
}

/* Clearing an incident */

int l_action_incident_clear (i_resource *self, i_incident *inc)
{
  /* Confirm minimum severity is met */
  i_site *site = (i_site *) i_entity_child_get(ENTITY(self->hierarchy->cust), inc->met->site_name);
  if (!site) { i_printf (1, "l_action_incident_report failed to find site %s", inc->met->site_name); return 0; }
  i_device *dev = (i_device *) i_entity_child_get(ENTITY(site), inc->met->dev_name);
  if (!dev) { i_printf (1, "l_action_incident_report failed to find device %s", inc->met->dev_name); return 0; }
  if (inc->highest_opstate < dev->minimum_action_severity)
  {
    i_printf (1, "l_action_incident_clear suppressed action for incident %li as the highest severity %i is lower than the devices (%s) minimum_action_severity (%i)", 
      inc->id, inc->highest_opstate, dev->desc_str,
      dev->minimum_action_severity);
    return 0;
  }

  /* Process each of the incidents candidate actions */
  l_action *action;
  for (i_list_move_head(inc->action_list); (action=i_list_restore(inc->action_list))!=NULL; i_list_move_next(inc->action_list))
  {
    /* Check for a delay timer */
    if (action->delay_timer)
    { i_timer_remove (action->delay_timer); action->delay_timer = NULL; }

    /* Check for a re-run timer */
    if (action->rerun_timer)
    { i_timer_remove (action->rerun_timer); action->rerun_timer = NULL; }

    /* Check run count */
    if (action->run_count > 0)
    {
      /* Script has run atleast once, so we need
       * to run it immediately with a 'clear' command 
       */

      if (inc->occurrences == 1)
      { l_action_exec (self, action->script_file, "clear", action, inc, NULL, NULL); }
      else
      { l_action_exec (self, action->script_file, "clear_recurring", action, inc, NULL, NULL); }
    }
  }

  return 0;
}

/* Case binding */

int l_action_incident_boundtocase (i_resource *self, i_incident *inc)
{
  /* Called when an incident is bound to a case and/or
   * when the list of actions has been loaded for an incident
   * that has been bound to a case already
   */

  l_action *action;
  for (i_list_move_head(inc->action_list); (action=i_list_restore(inc->action_list))!=NULL; i_list_move_next(inc->action_list))
  {
    if (action->casecancels == CASE_CANCELS_ALL)
    {
      /* Cancel all activity */
      action->runstate = 0;
      action->activation = 0;
      if (action->delay_timer)
      { i_timer_remove (action->delay_timer); action->delay_timer = NULL; }
      if (action->rerun_timer)
      { i_timer_remove (action->rerun_timer); action->rerun_timer = NULL; }
    }
    else if (action->casecancels == CASE_CANCELS_RERUN)
    {
      /* Cancel rerun */
      if (action->rerun_timer)
      {
        i_timer_remove (action->rerun_timer);
        action->rerun_timer = NULL;
        action->runstate = 0;
        action->rerun = 0;
      }
    }
  }

  return 0;
}
