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

#include "vserv.h"
#include "vport.h"

/* Item Manipulation */

v_vport_item* v_vport_item_create ()
{
  v_vport_item *vport;

  vport = (v_vport_item *) malloc (sizeof(v_vport_item));
  if (!vport)
  { i_printf (1, "v_vport_item_create failed to malloc vport"); return NULL; }
  memset (vport, 0, sizeof(v_vport_item));

  return vport;
}

void v_vport_item_free (void *itemptr)
{
  v_vport_item *vport = itemptr;

  free (vport);
}
