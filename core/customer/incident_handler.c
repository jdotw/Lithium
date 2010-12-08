#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/data.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/opstate.h>
#include <induction/incident.h>

#include "case.h"
#include "action.h"
#include "incident.h"

/* Incident Report Handler */

int l_incident_handler_report (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Handle an inbound incident report
   *
   * ALWAYS RETURN 0 to keep handler active
   */
  int num;
  long msgid;
  i_incident *inc;
  i_list *inc_list;
  i_list *inc_uhlist;
  i_list *cleared_list;
  i_site *site;
  i_hashtable_key *key;
  i_hashtable *inc_table;
  i_hashtable *cleared_table;

  /* Convert msg data to incident */
  inc = i_incident_struct (msg->data, msg->datasize);
  if (!inc) 
  { 
    i_printf (1, "l_incident_handler_report failed to convert msg data to incident report from %s:%s:%i:%i:%s",
      msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str);
    return 0;
  }

  /* Retrieve table/list */
  inc_table = l_incident_table ();
  cleared_table = l_incident_cleared_table ();
  inc_list = l_incident_list ();
  cleared_list = l_incident_cleared_list ();
  inc_uhlist = l_incident_uhlist ();

  /* Check for a recently cleared incident */
  char *ent_addr_str = i_entity_address_string (NULL, (i_entity_address *) inc->met);
  key = i_hashtable_create_key_string (ent_addr_str, cleared_table->size);
  free (ent_addr_str);
  i_incident *cleared_inc = i_hashtable_get (cleared_table, key);
  if (cleared_inc)
  {
    /* A recently-cleared incident matches this incident.
     *
     * Revive the cleared incident 
     */

    /* Update local incident */
    cleared_inc->end_tv.tv_sec = 0;
    cleared_inc->end_tv.tv_usec = 0;
    cleared_inc->state = INCSTATE_ACTIVE;
    if (cleared_inc->prev_ent) i_entity_descriptor_free (cleared_inc->prev_ent);
    cleared_inc->prev_ent = cleared_inc->ent ? i_entity_descriptor_duplicate (cleared_inc->ent) : NULL;
    if (cleared_inc->ent) i_entity_descriptor_free (cleared_inc->ent);
    cleared_inc->ent = inc->ent ? i_entity_descriptor_duplicate (inc->ent) : NULL;
    cleared_inc->trg_type = inc->trg_type;
    if (cleared_inc->trg_xval_str) free (cleared_inc->trg_xval_str);
    cleared_inc->trg_xval_str = inc->trg_xval_str ? strdup (inc->trg_xval_str) : NULL;
    if (cleared_inc->trg_yval_str) free (cleared_inc->trg_yval_str);
    cleared_inc->trg_yval_str = inc->trg_yval_str ? strdup (inc->trg_yval_str) : NULL;
    if (cleared_inc->cleared_valstr) free (cleared_inc->cleared_valstr);
    cleared_inc->cleared_valstr = NULL;
    gettimeofday (&cleared_inc->last_transition_tv, NULL);
    gettimeofday (&cleared_inc->last_occurrence_tv, NULL);
    cleared_inc->occurrences++;

    /* Update SQL */
    num = l_incident_sql_update (self, cleared_inc);
    if (num != 0)
    { i_printf (1, "l_incident_handler_report warning, failed to update SQL databse for recurring incident %li", cleared_inc->id); }

    /* Remove from recently cleared list/table */
    cleared_list->p = cleared_inc->cleared_listp;
    num = i_list_delete (cleared_list);
    if (num != 0)
    { i_printf (1, "l_incident_handler_report warning, failed to remove recurring incident %li from recently cleared list", cleared_inc->id); }
    num = i_hashtable_remove (cleared_table, key);
    if (num != 0)
    { i_printf (1, "l_incident_handler_report warning, failed to remove recurring incident %li from recently cleared hash table", cleared_inc->id); }

    /* Remove self-destruct timer */
    i_timer_remove (cleared_inc->destruct_timer);
    cleared_inc->destruct_timer = NULL;

    /* Free the recently created incident */
    i_incident_free (inc);

    /* Switch to using the cleared/revived incident */
    inc = cleared_inc;
  }
  else
  {
    /* No recently-cleared incident match.
     *
     * Stick with the new incident
     */

    inc->occurrences = 1;

    /* Add incident to SQL database */
    num = l_incident_sql_insert (self, inc);
    if (num != 0)
    {
      i_printf (1, "l_incident_handler_report failed to insert incident report from %s:%s:%i:%i:%s to SQL database",
        msg->src->plexus, msg->src->node, msg->src->type, msg->src->ident_int, msg->src->ident_str);
      i_incident_free (inc);
      return 0;
    }

    /* Assign ID */
    inc->id = l_incident_id_assign ();
  }
  i_hashtable_key_free (key);

  /* Add incident to table */
  key = i_hashtable_create_key_long (inc->id, inc_table->size);
  num = i_hashtable_put (inc_table, key, inc);
  i_hashtable_key_free (key);
  if (num != 0)
  { i_printf (1, "l_incident_handler_report warning, failed to add incident to hashtable"); }

  /* Add to incident lists */
  num = i_list_enqueue (inc_list, inc);
  if (num != 0)
  { i_printf (1, "l_incident_handler_report warning, failed to enqueue incident to incident list"); }
  if (inc->caseid == 0)
  {
    num = i_list_enqueue (inc_uhlist, inc);
    if (num != 0)
    { i_printf (1, "l_incident_handler_report warning, failed to enqueue incident to unhandled incident list"); }
  }

  /* Store incident list pointer */
  i_list_move_last (inc_list);
  inc->listp = inc_list->p;

  /* Response with incidnt ID */
  msgid = i_message_send (self, MSG_INC_REPORT, &inc->id, sizeof(long), msg->src, MSG_FLAG_RESP, msg->msgid);
  if (msgid == -1)
  { i_printf (1, "l_incident_handler_report warning, failed to send incident ID to reporting resource"); }

  /* Increment Counters */
  switch (inc->ent->opstate)
  {
    case ENTSTATE_FAILED: l_incident_count_failed_inc (); 
                          break;
    case ENTSTATE_IMPAIRED: l_incident_count_impaired_inc (); 
                          break;
    case ENTSTATE_ATRISK: l_incident_count_atrisk_inc (); 
                          break;
  }

  /* Get site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), inc->ent->site_name);
  if (site)
  {
    i_device *dev;

    /* Get device */
    dev = (i_device *) i_entity_child_get (ENTITY(site), inc->ent->dev_name);
    if (dev)
    {
      /* Attempt to find full entity match */
      i_entity *local_entity = i_entity_local_get (self, (i_entity_address *) inc->ent);
      if (local_entity)
      { 
        /* Update synced entity (will stop at device level) */
        i_opstate_change (self, local_entity, inc->ent->opstate); 
      }
      /* Update device status */
      i_opstate_change (self, ENTITY(dev), inc->ent->dev_opstate); 

      /* Update incidents entity descriptor 
       *
       * This is done because the incidents entity descriptor
       * is what comes from the device. The device doesnt 
       * know the status of its site and customer, hence with
       * our knowledge as the customer we update the descriptor
       */
      inc->ent->site_opstate = site->opstate;
      inc->ent->cust_opstate = site->cust->opstate;
      inc->ent->site_adminstate = site->adminstate;
      inc->ent->cust_adminstate = site->cust->adminstate;

    }
    else
    { i_printf (1, "l_incident_handler_report received incident for unknown device %s at %s", inc->ent->dev_name, inc->ent->site_name); }
  }
  else
  { i_printf (1, "l_incident_handler_report received incident for unknown site %s", inc->ent->site_name); }

  /* If incident is new, bind incident to case */
  if (!cleared_inc)
  {
    num = l_incident_bind_incident (self, inc);
    if (num != 0)
    { i_printf (1, "l_incident_handler_report warning, failed to bind incident to cases"); }
  }

  /* Actions 
   *
   * The l_action_incident_report will handle the event
   * as a 'report' or 'recurrence' based on the action 
   * runcount and incident occurrence count
   */
  l_action_incident_report (self, inc);    
  
  return 0;
}

/* Incident Transition Handler */

int l_incident_handler_transition (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep handler alive */

  int num;
  i_incident *local_inc;                /* Local incident */
  i_incident *msg_inc;                  /* Incident in message */
  i_list *inc_list;
  i_list *inc_uhlist;
  i_hashtable *inc_table;
  i_hashtable_key *key;

  /* Retrieve incident from message */
  msg_inc = i_incident_struct (msg->data, msg->datasize);
  if (!msg_inc)
  { i_printf (1, "l_incident_handler_transition failed to convert msg->data to msg_inc"); return 0; }
  
  /* Retrieve table/list */
  inc_table = l_incident_table ();
  inc_list = l_incident_list ();
  inc_uhlist = l_incident_uhlist ();

  /* Retrieve incident */
  key = i_hashtable_create_key_long (msg_inc->id, inc_table->size);
  local_inc = i_hashtable_get (inc_table, key);
  if (local_inc)
  {
    /* 
     * A matching local incident was found 
     */
    i_site *site;

    /* Update local incident */
    if (local_inc->prev_ent) i_entity_descriptor_free (local_inc->prev_ent);
    local_inc->prev_ent = local_inc->ent ? i_entity_descriptor_duplicate (local_inc->ent) : NULL;
    if (local_inc->ent) i_entity_descriptor_free (local_inc->ent);
    local_inc->ent = msg_inc->ent ? i_entity_descriptor_duplicate (msg_inc->ent) : NULL;
    local_inc->trg_type = msg_inc->trg_type;
    if (local_inc->trg_xval_str) free (local_inc->trg_xval_str);
    local_inc->trg_xval_str = msg_inc->trg_xval_str ? strdup (msg_inc->trg_xval_str) : NULL;
    if (local_inc->trg_yval_str) free (local_inc->trg_yval_str);
    local_inc->trg_yval_str = msg_inc->trg_yval_str ? strdup (msg_inc->trg_yval_str) : NULL;
    gettimeofday (&local_inc->last_transition_tv, NULL);

    /* Update SQL using the local incident */
    num = l_incident_sql_update (self, local_inc);
    if (num != 0)
    { i_printf (1, "l_incident_handler_transition warning, failed to update SQL databse for incident %li", local_inc->id); }

    /* Get site */
    site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), msg_inc->ent->site_name);
    if (site)
    {
      i_device *dev;

      /* Get device */
      dev = (i_device *) i_entity_child_get (ENTITY(site), msg_inc->ent->dev_name);
      if (dev)
      {
        /* Attempt to find full entity match */
        i_entity *local_entity = i_entity_local_get (self, (i_entity_address *) msg_inc->ent);
        if (local_entity)
        { 
          /* Update synced entity (will stop at device level) */
          i_opstate_change (self, local_entity, msg_inc->ent->opstate); 
        }
        /* Set device state */
        i_opstate_change (self, ENTITY(dev), msg_inc->ent->dev_opstate);
      } 
      else
      { i_printf (1, "l_incident_handler_transition received incident for unknown device %s at %s", msg_inc->ent->dev_name, msg_inc->ent->site_name); }
    } 
    else
    { i_printf (1, "l_incident_handler_transition received incident for unknown site %s", msg_inc->ent->site_name); }
    
    /* Actions */
    l_action_incident_transition (self, local_inc);
    
    /* Update highest/lowest variables */
    if (msg_inc->ent->opstate > local_inc->highest_opstate) local_inc->highest_opstate = msg_inc->ent->opstate;
    if (msg_inc->ent->opstate < local_inc->lowest_opstate) local_inc->lowest_opstate = msg_inc->ent->opstate;
  }
  i_hashtable_key_free (key);


  /* Free received incident */
  i_incident_free (msg_inc);
  
  return 0;
}

/* Incident Clear Handler */

int l_incident_handler_clear (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep handler alive */

  int num;
  i_incident *local_inc;                /* Local incident */
  i_incident *msg_inc;                  /* Incident in message */
  i_list *inc_list;
  i_list *inc_uhlist;
  i_list *cleared_list;
  i_hashtable *inc_table;
  i_hashtable *cleared_table;
  i_hashtable_key *key;

  /* Retrieve incident from message */
  msg_inc = i_incident_struct (msg->data, msg->datasize);
  if (!msg_inc)
  { i_printf (1, "l_incident_handler_clear failed to convert msg->data to msg_inc"); return 0; }

  /* Retrieve table/list */
  inc_table = l_incident_table ();
  cleared_table = l_incident_cleared_table ();
  inc_list = l_incident_list ();
  cleared_list = l_incident_cleared_list ();
  inc_uhlist = l_incident_uhlist ();

  /* Retrieve incident */
  key = i_hashtable_create_key_long (msg_inc->id, inc_table->size);
  local_inc = i_hashtable_get (inc_table, key);
  if (local_inc)
  {
    /* 
     * A matching local incident was found 
     */
    i_site *site;

    /* Update local incident 
     *
     * The incident entity is NOT updated because the clear 
     * comes with the metric specified, not the trigger
     *
     */
    local_inc->end_tv.tv_sec = msg_inc->end_tv.tv_sec;
    local_inc->end_tv.tv_usec = msg_inc->end_tv.tv_usec;
    local_inc->state = INCSTATE_INACTIVE;

    /* Update SQL using the local incident */
    num = l_incident_sql_update (self, local_inc);
    if (num != 0)
    { i_printf (1, "l_incident_handler_clear warning, failed to update SQL databse for incident %li", local_inc->id); }

    /* Decrement Counters */
    switch (local_inc->ent->opstate)
    {
      case ENTSTATE_FAILED: l_incident_count_failed_dec ();
                            break;
      case ENTSTATE_IMPAIRED: l_incident_count_impaired_dec ();
                              break;
      case ENTSTATE_ATRISK: l_incident_count_atrisk_dec ();
                            break;
    }                       

    /* Get site */
    site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), msg_inc->ent->site_name);
    if (site)
    {
      i_device *dev;

      /* Get device */
      dev = (i_device *) i_entity_child_get (ENTITY(site), msg_inc->ent->dev_name);
      if (dev)
      {
        /* Attempt to find full entity match */
        i_entity *local_entity = i_entity_local_get (self, (i_entity_address *) msg_inc->ent);
        if (local_entity)
        { 
          /* Update synced entity (will stop at device level) */
          i_opstate_change (self, local_entity, msg_inc->ent->opstate);
        }
   
        /* Set device state */
        i_opstate_change (self, ENTITY(dev), msg_inc->ent->dev_opstate);
      } 
      else
      { i_printf (1, "l_incident_handler_clear received incident for unknown device %s at %s", msg_inc->ent->dev_name, msg_inc->ent->site_name); }
    } 
    else
    { i_printf (1, "l_incident_handlerclear received incident for unknown site %s", msg_inc->ent->site_name); }
    
    /* Actions */
    if (local_inc->occurrences == 1)
    {
      /* This is a single-run incident, send the 
       * clear action. Recurring incidents have
       * the clear action performed when the 
       * self-destruct timer fires 
       *
       * The local_inc MUST be used as the incident passed to
       * l_action_incident_clear to ensure the occurrence and runcount
       * is correct
       */
      l_action_incident_clear (self, local_inc);
    }

    /* Remove from list and table */
    inc_list->p = local_inc->listp;
    num = i_list_delete (inc_list);
    if (num != 0)
    { i_printf (1, "l_incident_handler_clear warning, failed to remove incident %li from list", msg_inc->id); }
    if (i_list_search (inc_uhlist, local_inc) == 0) i_list_delete (inc_uhlist);
    local_inc->listp = NULL;

    /* Remove from hash table */
    num = i_hashtable_remove (inc_table, key);
    if (num != 0)
    { i_printf (1, "l_incident_handler_clear warning, failed to remove incident %li from table", msg_inc->id); }
    i_hashtable_key_free (key);
    
    /* Add to cleared list and table */
    char *met_entaddr_str = i_entity_address_string (NULL, (i_entity_address *) local_inc->met);
    key = i_hashtable_create_key_string (met_entaddr_str, cleared_table->size);
    free (met_entaddr_str);
    num = i_hashtable_put (cleared_table, key, local_inc);
    i_hashtable_key_free (key);
    if (num != 0)
    { i_printf (1, "l_incident_handler_clear warning, failed to add incident to cleared hashtable"); }
  
    /* Add to incident lists */
    num = i_list_enqueue (cleared_list, local_inc);
    if (num != 0)
    { i_printf (1, "l_incident_handler_clear warning, failed to enqueue incident to cleared incident list"); }

    /* Create self-destruct timer */
    local_inc->destruct_timer = i_timer_add (self, (60 * 60 * 4), 0, l_incident_selfdestruct, local_inc);
    
    /* Store incident list pointer */
    i_list_move_last (cleared_list);
    local_inc->cleared_listp = cleared_list->p;
  }
  else
  {
    i_printf (1, "l_incident_handler_clear warning, incident %li not found in incident table", msg_inc->id);
    i_hashtable_key_free (key); 
  }

  /* Free received incident */
  i_incident_free (msg_inc);
  
  return 0;
}

/* Incident Clear All Handler */

int l_incident_handler_clearall (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{   
  /* ALWAYS RETURN 0 to keep handler alive */
  i_incident *inc;
  i_list *inc_list;
  char *addrstr = msg->data;

  if (!addrstr) return 0;

  /* Loop through incidents */
  inc_list = l_incident_list();
  for (i_list_move_head(inc_list); (inc=i_list_restore(inc_list))!=NULL; i_list_move_next(inc_list))
  {
    char *incaddrstr = i_resource_address_struct_to_string (inc->ent->resaddr);
    if (!incaddrstr)
    { i_printf (1, "l_incident_handler_clearall failed to convert inc->ent->resaddr to incaddrstr"); continue; }
    if (!strcmp(incaddrstr, addrstr))
    {
      /* 
       * A matching local incident was found 
       */
      int num;
      i_site *site;
      i_list *inc_uhlist = l_incident_uhlist ();
      i_hashtable *inc_table = l_incident_table ();
      i_hashtable_key *key;

      /* Create hashtable key */
      key = i_hashtable_create_key_long (inc->id, inc_table->size);

      /* Decrement Counters */
      switch (inc->ent->opstate)
      {
        case ENTSTATE_FAILED: l_incident_count_failed_dec ();
                              break;
        case ENTSTATE_IMPAIRED: l_incident_count_impaired_dec ();
                                break;
        case ENTSTATE_ATRISK: l_incident_count_atrisk_dec ();
                              break;
      }

      /* Manually update incident */
      inc->state = INCSTATE_INACTIVE;

      /* Update SQL */
      num = l_incident_sql_update (self, inc);
      if (num != 0)
      { i_printf (1, "l_incident_handler_clearall warning, failed to update SQL databse for incident %li", inc->id); }

      /* Get site */
      site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), inc->ent->site_name);
      if (site)
      {
        i_device *dev;

        /* Get device */
        dev = (i_device *) i_entity_child_get (ENTITY(site), inc->ent->dev_name);
        if (dev)
        {
          /* Attempt to find full entity match */
          i_entity *local_entity = i_entity_local_get (self, (i_entity_address *) inc->ent);
          if (local_entity)
          { 
            /* Update synced entity (will stop at device level) */
            i_opstate_change (self, local_entity, inc->ent->opstate); 
          }
          /* Set device state */
          i_opstate_change (self, ENTITY(dev), ENTSTATE_NORMAL);
        }
        else
        { i_printf (1, "l_incident_handler_clearall received incident for unknown device %s at %s", inc->ent->dev_name, inc->ent->site_name); }
      }
      else
      { i_printf (1, "l_incident_handlerclearall received incident for unknown site %s", inc->ent->site_name); }

      /* Actions */
      l_action_incident_clear (self, inc);
  
      /* Remove from lists */
      inc_list->p = inc->listp;
      num = i_list_delete (inc_list);
      if (num != 0)
      { i_printf (1, "l_incident_handler_clear warning, failed to remove incident %li from list", inc->id); }
      if (i_list_search (inc_uhlist, inc) == 0) i_list_delete (inc_uhlist);

      /* Remove from hash table */
      num = i_hashtable_remove (inc_table, key);
      if (num != 0)
      { i_printf (1, "l_incident_handler_clear warning, failed to remove incident %li from table", inc->id); }

      /* Free Incident */
      i_incident_free (inc);
    }
    free (incaddrstr);

    /* End of list iteration */
  }

  return 0;
}
