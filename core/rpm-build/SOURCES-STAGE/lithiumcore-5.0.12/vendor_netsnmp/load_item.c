#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/value.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/snmp.h"

#include "load.h"

/* Item Manipulation */

l_snmp_nsload_item* l_snmp_nsload_item_create ()
{
  l_snmp_nsload_item *load;

  load = (l_snmp_nsload_item *) malloc (sizeof(l_snmp_nsload_item));
  if (!load)
  { i_printf (1, "l_snmp_nsload_item_create failed to malloc load"); return NULL; }
  memset (load, 0, sizeof(l_snmp_nsload_item));

  return load;
}

void l_snmp_nsload_item_free (void *itemptr)
{
  l_snmp_nsload_item *load = itemptr;

  free (load);
}
