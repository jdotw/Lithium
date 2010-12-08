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
#include "device/snmp.h"

#include "mainboard.h"

/* Item Struct Manipulation */

v_mainboard_item* v_mainboard_item_create ()
{
  v_mainboard_item *item;

  item = (v_mainboard_item *) malloc (sizeof(v_mainboard_item));
  if (!item)
  { i_printf (1, "v_mainboard_item_create failed to malloc v_mainboard_item struct"); return NULL; }
  memset (item, 0, sizeof(v_mainboard_item));

  return item;
}

void v_mainboard_item_free (void *itemptr)
{
  v_mainboard_item *item = itemptr;

  if (!item) return;

  free (item);
}
