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

#include "service.h"

/* Script Struct Manipulation */

l_service_log* l_service_log_create ()
{
  l_service_log *log;
  log = (l_service_log *) malloc (sizeof(l_service_log));
  memset (log, 0, sizeof(l_service_log));
  return log;
}

void l_service_log_free (void *logptr)
{
  l_service_log *log = logptr;
  if (log->comment_str) free (log->comment_str);
  free (log);
}


