#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/callback.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>

#include "snmp.h"
#include "icmp.h"

/* icmp - ICMP Sub-System */

/* Item Struct Manipulation */

l_icmp_item* l_icmp_item_create ()
{
  l_icmp_item *item;

  item = (l_icmp_item *) malloc (sizeof(l_icmp_item));
  if (!item)
  { i_printf (1, "l_icmp_item failed to malloc l_icmp_item struct"); return NULL; }
  memset (item, 0, sizeof(l_icmp_item));

  return item;
}

void l_icmp_item_free (void *itemptr)
{
  l_icmp_item *item = itemptr;

  if (!item) return;

  free (item);
}

