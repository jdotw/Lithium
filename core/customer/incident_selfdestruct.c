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
#include <induction/timer.h>
#include <induction/list.h>
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

/* Incident Self-Destruct */

int l_incident_selfdestruct (i_resource *self, i_timer *timer, void *passdata)
{
  i_incident *inc = (i_incident *) passdata;

  /* The incident has reached the recurrence hold-down period (4 hours)
   * with no further recurrence. 
   *
   * Clear the incident 
   */
  
  /* Retrieve table/list */
  i_hashtable *cleared_table = l_incident_cleared_table ();
  i_list *cleared_list = l_incident_cleared_list ();

  /* Run Cleared Action for Recurring Incidents */
  if (inc->occurrences > 1) l_action_incident_clear (self, inc);

  /* Remove from list */
  cleared_list->p = inc->cleared_listp;
  int num = i_list_delete (cleared_list);
  if (num != 0)
  { i_printf (1, "l_incident_selfdestruct warning, failed to remove incident %li from list", inc->id); }
  
  /* Remove from hash table */
  char *met_entaddr_str = i_entity_address_string (NULL, (i_entity_address *) inc->met);
  i_hashtable_key *key = i_hashtable_create_key_string (met_entaddr_str, cleared_table->size);
  free (met_entaddr_str);
  num = i_hashtable_remove (cleared_table, key);
  if (num != 0)
  { i_printf (1, "l_incident_selfdestruct warning, failed to remove incident %li from table", inc->id); }
  i_hashtable_key_free (key);
    
  /* Free incident */
  i_incident_free (inc);
  
  return 0;
}

