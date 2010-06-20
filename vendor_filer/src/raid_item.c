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

#include "raid.h"

/* Item Manipulation */

v_raid_item* v_raid_item_create ()
{
  v_raid_item *raid;

  raid = (v_raid_item *) malloc (sizeof(v_raid_item));
  if (!raid)
  { i_printf (1, "v_raid_item_create failed to malloc raid"); return NULL; }
  memset (raid, 0, sizeof(v_raid_item));

  return raid;
}

void v_raid_item_free (void *itemptr)
{
  v_raid_item *raid = itemptr;

  free (raid);
}
