#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/value.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <lithium/snmp.h>

#include "rserv.h"
#include "rport.h"

/* Item Manipulation */

v_rport_item* v_rport_item_create ()
{
  v_rport_item *rport;

  rport = (v_rport_item *) malloc (sizeof(v_rport_item));
  if (!rport)
  { i_printf (1, "v_rport_item_create failed to malloc rport"); return NULL; }
  memset (rport, 0, sizeof(v_rport_item));

  return rport;
}

void v_rport_item_free (void *itemptr)
{
  v_rport_item *rport = itemptr;

  free (rport);
}
