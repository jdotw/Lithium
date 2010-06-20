#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/path.h>
#include <lithium/snmp.h>

#include "device.h"

/* Cisco CPU Resource Sub-System */

/* Item Struct Manipulation */

v_device_item* v_device_item_create ()
{
  v_device_item *item;

  item = (v_device_item *) malloc (sizeof(v_device_item));
  if (!item)
  { i_printf (1, "v_device_item_create failed to malloc v_device_item struct"); return NULL; }
  memset (item, 0, sizeof(v_device_item));

  return item;
}

void v_device_item_free (void *itemptr)
{
  v_device_item *item = itemptr;

  if (!item) return;

  free (item);
}
