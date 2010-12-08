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

#include "case.h"

/*
 * Case System
 */

/* Log Entries */

l_case_logentry* l_case_logentry_create ()
{
  l_case_logentry *log;

  log = (l_case_logentry *) malloc (sizeof(l_case_logentry));
  if (!log) return NULL;
  memset (log, 0, sizeof(l_case_logentry));

  return log;
}

void l_case_logentry_free (void *logptr)
{
  l_case_logentry *log = logptr;

  if (!log) return;

  if (log->author_str) free (log->author_str);
  if (log->entry_str) free (log->entry_str);
  
  free (log);
}
