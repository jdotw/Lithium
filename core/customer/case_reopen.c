#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>
#include <induction/callback.h>

#include "case.h"
#include "incident.h"

/*
 * Case System
 */

/* Re-open a case */

int l_case_reopen (i_resource *self, l_case *cas, char *owner_str)
{
  int num;
  char *str;

  /* Set variables */
  gettimeofday (&cas->start, NULL);
  cas->end.tv_sec = 0;
  cas->end.tv_usec = 0;
  cas->state = CASE_STATE_OPEN;
  if (cas->owner_str) free (cas->owner_str);
  cas->owner_str = strdup (owner_str);

  /* Update case in SQL */
  num = l_case_sql_update (self, cas);
  if (num != 0)
  { i_printf (1, "l_case_close_casecb failed to call l_case_sql_update"); return -1; }

  /* Set case state */
  l_case_entity_sql_update_casestate (self, cas->id, cas->state);
  l_case_logentry_sql_update_casestate (self, cas->id, cas->state);

  /* Load entities to bind */
  asprintf (&str, "%li", cas->id);
  l_case_entity_sql_list (self, str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_reopen_entitycb, NULL);
  free (str);
  
  return 0;
}

int l_case_reopen_entitycb (i_resource *self, i_list *list, void *passdata)
{
  l_case *cas;

  /* Loop through cases */
  for (i_list_move_head(list); (cas=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    l_incident_bind_case (self, cas);
  }

  return -1; /* Dont keep the list */
}
