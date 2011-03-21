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

#include "group.h"

static long static_currentid = 0;

/* Variable manipulation */

long l_group_id_assign ()
{ 
  static_currentid++; 
  return static_currentid; 
}

void l_group_id_setcurrent (long currentid)
{ 
  static_currentid = currentid; 
}

