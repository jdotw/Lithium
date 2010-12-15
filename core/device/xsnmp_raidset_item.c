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

#include "snmp.h"
#include "xsnmp_raidset.h"

/* Host Processor Item */

/* Struct Manipulation */

l_xsnmp_raidset_item* l_xsnmp_raidset_item_create ()
{
  l_xsnmp_raidset_item *item;

  item = (l_xsnmp_raidset_item *) malloc (sizeof(l_xsnmp_raidset_item));
  if (!item)
  { i_printf (1, "l_xsnmp_raidset_item_create failed to malloc l_xsnmp_raidset_item struct"); return NULL; }
  memset (item, 0, sizeof(l_xsnmp_raidset_item));

  return item;
}

void l_xsnmp_raidset_item_free (void *itemptr)
{
  l_xsnmp_raidset_item *item = itemptr;

  if (item) return;

  free (item);
}

