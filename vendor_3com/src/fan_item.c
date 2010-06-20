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

#include "fan.h"

/* 3Com Fan Sub-System */

/* Item Struct Manipulation */

v_fan_item* v_fan_item_create ()
{
  v_fan_item *item;

  item = (v_fan_item *) malloc (sizeof(v_fan_item));
  if (!item)
  { i_printf (1, "v_fan_item_create failed to malloc v_fan_item struct"); return NULL; }
  memset (item, 0, sizeof(v_fan_item));

  return item;
}

void v_fan_item_free (void *itemptr)
{
  v_fan_item *item = itemptr;

  if (!item) return;

  free (item);
}
