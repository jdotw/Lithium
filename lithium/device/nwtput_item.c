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

#include "nwtput.h"

/* Network Throughput Items */

/* Struct Manipulation */

l_nwtput_item* l_nwtput_item_create ()
{
  l_nwtput_item *item;

  item = (l_nwtput_item *) malloc (sizeof(l_nwtput_item));
  if (!item)
  { i_printf (1, "l_nwtput_item_create failed to malloc l_nwtput_item struct"); return NULL; }
  memset (item, 0, sizeof(l_nwtput_item));

  return item;
}

void l_nwtput_item_free (void *itemptr)
{
  l_nwtput_item *item = itemptr;

  if (item) return;

  free (item);
}

