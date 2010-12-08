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

#include "case.h"
#include "incident.h"

/*
 * Case System
 */

/* Open the case */

int l_case_open (i_resource *self, l_case *cas, i_list *ent_list)
{
  /* Opens the specified case. */
  int num;
  i_entity_descriptor *ent;

  /* Insert case into SQL */
  num = l_case_sql_insert (self, cas);
  if (num != 0) return -1; 

  /* Insert entity list */
  for (i_list_move_head(ent_list); (ent=i_list_restore(ent_list))!=NULL; i_list_move_next(ent_list))
  {
    /* Add entity to case */
    l_case_entity_add (self, cas->id, ent);
  }

  /* Set case state */
  l_case_entity_sql_update_casestate (self, cas->id, cas->state);
  l_case_logentry_sql_update_casestate (self, cas->id, cas->state);

  return 0;
}

