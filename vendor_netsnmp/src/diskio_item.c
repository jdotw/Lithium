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

#include "diskio.h"

/* Item Struct Manipulation */

l_snmp_nsdiskio_item* l_snmp_nsdiskio_item_create ()
{
  l_snmp_nsdiskio_item *item;

  item = (l_snmp_nsdiskio_item *) malloc (sizeof(l_snmp_nsdiskio_item));
  if (!item)
  { i_printf (1, "l_snmp_nsdiskio_item_create failed to malloc l_snmp_nsdiskio_item struct"); return NULL; }
  memset (item, 0, sizeof(l_snmp_nsdiskio_item));

  return item;
}

void l_snmp_nsdiskio_item_free (void *itemptr)
{
  l_snmp_nsdiskio_item *item = itemptr;

  if (!item) return;

  free (item);
}
