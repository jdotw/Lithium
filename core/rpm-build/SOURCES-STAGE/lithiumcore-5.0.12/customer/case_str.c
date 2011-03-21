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

char* l_case_statestr (unsigned short state)
{
  switch (state)
  {
    case CASE_STATE_OPEN: return "Open";
    case CASE_STATE_CLOSED: return "Closed";
  }

  return "Unknown";
}
