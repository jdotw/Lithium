#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "action.h"

/* Script Struct Manipulation */

l_action_log* l_action_log_create ()
{
  l_action_log *log;
  log = (l_action_log *) malloc (sizeof(l_action_log));
  memset (log, 0, sizeof(l_action_log));
  return log;
}

void l_action_log_free (void *logptr)
{
  l_action_log *log = logptr;
  if (log->comment_str) free (log->comment_str);
  if (log->ent) i_entity_descriptor_free (log->ent);
  free (log);
}


