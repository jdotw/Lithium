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

#include "load.h"

/* Item Manipulation */

v_load_item* v_load_item_create ()
{
  v_load_item *load;

  load = (v_load_item *) malloc (sizeof(v_load_item));
  if (!load)
  { i_printf (1, "v_load_item_create failed to malloc load"); return NULL; }
  memset (load, 0, sizeof(v_load_item));

  return load;
}

void v_load_item_free (void *itemptr)
{
  v_load_item *load = itemptr;

  free (load);
}
