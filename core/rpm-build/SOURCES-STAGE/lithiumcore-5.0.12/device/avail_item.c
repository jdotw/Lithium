#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "avail.h"

/* Availability Items */

/* Struct Manipulation */

l_avail_item* l_avail_item_create ()
{
  l_avail_item *item;

  item = (l_avail_item *) malloc (sizeof(l_avail_item));
  if (!item)
  { i_printf (1, "l_avail_item_create failed to malloc l_avail_item struct"); return NULL; }
  memset (item, 0, sizeof(l_avail_item));

  item->rt_list = i_list_create ();
  if (!item->rt_list)
  { i_printf (1, "l_avail_item_create failed to create rt_list"); l_avail_item_free (item); return NULL; }
  i_list_set_destructor (item->rt_list, free);

  return item;
}

void l_avail_item_free (void *itemptr)
{
  l_avail_item *item = itemptr;

  if (item) return;

  if (item->rt_list) i_list_free (item->rt_list);
  if (item->avail_cg) i_metric_cgraph_free (item->avail_cg);

  free (item);
}

