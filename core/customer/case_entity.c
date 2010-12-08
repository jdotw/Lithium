#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/socket.h>
#include <induction/message.h>
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

/* Entity Manipulation */

int l_case_entity_add (i_resource *self, long caseid, i_entity_descriptor *ent)
{
  int num;

  /* Insert into SQL */
  num = l_case_entity_sql_insert (self, caseid, ent);
  if (num != 0) return -1;

  /* Bind incidents to case */
  num = l_incident_bind_case_entity (self, caseid, ent);
  if (num != 0) { i_printf (1, "l_case_entity_add failed to bind incidents"); }

  return 0;
}

int l_case_entity_remove (i_resource *self, long caseid, i_entity_descriptor *ent)
{
  int num;
  
  /* Delete from SQL */
  num = l_case_entity_sql_delete (self, caseid, ent);
  if (num != 0) return -1;

  /* Unbind incidents */
  num = l_incident_unbind_case_entity (self, caseid, ent);
  if (num != 0) { i_printf (1, "l_case_entity_remove failed to unbind incidents"); }

  return 0;
}
