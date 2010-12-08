#include <stdlib.h>

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
#include <induction/incident.h>

#include "case.h"
#include "incident.h"

/* Incident ID Functions */

static long static_currentid = 0;

/* Assign ID */

long l_incident_id_assign ()
{ static_currentid++; return static_currentid; }

int l_incident_id_setcurrent (long currentid)
{ static_currentid = currentid; return 0; }

