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

#include "logdrv.h"

/* Item Struct Manipulation */

v_logdrv_item* v_logdrv_item_create ()
{
  v_logdrv_item *item;

  item = (v_logdrv_item *) malloc (sizeof(v_logdrv_item));
  if (!item)
  { i_printf (1, "v_logdrv_item_create failed to malloc v_logdrv_item struct"); return NULL; }
  memset (item, 0, sizeof(v_logdrv_item));

  return item;
}

void v_logdrv_item_free (void *itemptr)
{
  v_logdrv_item *item = itemptr;

  if (!item) return;

  free (item);
}
