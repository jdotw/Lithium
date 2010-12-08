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
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/data.h>
#include <induction/callback.h>
#include <induction/incident.h>

#include "case.h"
#include "incident.h"

/*
 * Incident Sub-System - Incident/Case Unbinding 
 */

/* Unbind based on a case */

int l_incident_unbind_case (i_resource *self, long caseid)
{
  i_list *inc_list;
  i_list *inc_uhlist;
  i_incident *inc;

  if (caseid < 1) return -1;

  /* Loop through active incidents */
  inc_list = l_incident_list ();
  inc_uhlist = l_incident_uhlist ();
  for (i_list_move_head(inc_list); (inc=i_list_restore(inc_list))!=NULL; i_list_move_next(inc_list))
  {
    /* Check binding */
    if (inc->caseid > 0 && inc->caseid == caseid)
    {
      /* Match found, unbind */
      inc->caseid = 0;
      i_list_enqueue (inc_uhlist, inc);
      l_incident_sql_update (self, inc);
    }
  }

  return 0;
}

/* Unbind based on a case entity */

int l_incident_unbind_case_entity (i_resource *self, long caseid, i_entity_descriptor *ent)
{
  int num;
  i_incident *inc;
  i_list *inc_list;
  i_list *inc_uhlist;

  /* Search for matching incident */
  inc_list = l_incident_list (); 
  inc_uhlist = l_incident_uhlist (); 
  for (i_list_move_head(inc_list); (inc=i_list_restore(inc_list))!=NULL; i_list_move_next(inc_list))
  { 
    if (inc->caseid != caseid) continue;   /* Skip unrelated incident */

    num = l_incident_bind_entitymatch (ent, inc->ent);
    if (num == 0)
    { 
      /* Match found unbind */
      inc->caseid = 0;
      i_list_enqueue (inc_uhlist, inc);
      l_incident_sql_update (self, inc);
    }
  }
  
  return 0;
}

