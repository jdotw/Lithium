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

#include "cache.h"

/* Item Struct Manipulation */

v_cache_item* v_cache_item_create ()
{
  v_cache_item *item;

  item = (v_cache_item *) malloc (sizeof(v_cache_item));
  if (!item)
  { i_printf (1, "v_cache_item_create failed to malloc v_cache_item struct"); return NULL; }
  memset (item, 0, sizeof(v_cache_item));

  return item;
}

void v_cache_item_free (void *itemptr)
{
  v_cache_item *item = itemptr;

  if (!item) return;

  free (item);
}
