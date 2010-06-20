#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/path.h>
#include <lithium/snmp.h>

#include "volume.h"

/* Item Struct Manipulation */

v_volume_item* v_volume_item_create ()
{
  v_volume_item *item;

  item = (v_volume_item *) malloc (sizeof(v_volume_item));
  if (!item)
  { i_printf (1, "v_volume_item_create failed to malloc v_volume_item struct"); return NULL; }
  memset (item, 0, sizeof(v_volume_item));

  return item;
}

void v_volume_item_free (void *itemptr)
{
  v_volume_item *item = itemptr;

  if (!item) return;

  free (item);
}
