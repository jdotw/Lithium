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

/* CLose the case */

int l_case_close (i_resource *self, long caseid)
{
  /* Closes the specified case. */
  char *str;
  i_callback *cb;

  /* Load specified case */
  asprintf (&str, "%li", caseid);
  cb = l_case_sql_list (self, str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_close_casecb, NULL);
  free (str);
  if (!cb) return -1; 

  return 0;
}

int l_case_close_casecb (i_resource *self, i_list *list, void *passdata)
{
  int num;
  l_case *cas;

  /* Retrieve case */
  i_list_move_head (list);
  cas = i_list_restore (list);
  if (!cas)
  { i_printf (1, "l_case_close_casecb specified case not found"); return -1; }

  /* Set variables */
  gettimeofday (&cas->end, NULL);
  cas->state = CASE_STATE_CLOSED;

  /* Update case in SQL */
  num = l_case_sql_update (self, cas);
  if (num != 0)
  { i_printf (1, "l_case_close_casecb failed to call l_case_sql_update"); return -1; }

  /* Set case state */
  l_case_entity_sql_update_casestate (self, cas->id, cas->state);
  l_case_logentry_sql_update_casestate (self, cas->id, cas->state);

  /* Unbind incidents */
  l_incident_unbind_case (self, cas->id);
  
  return -1;  /* Dont keep list */
}
