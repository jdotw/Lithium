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

#include "revision.h"

/* Cisco CPU Resource Sub-System */

/* Item Struct Manipulation */

v_revision_item* v_revision_item_create ()
{
  v_revision_item *item;

  item = (v_revision_item *) malloc (sizeof(v_revision_item));
  if (!item)
  { i_printf (1, "v_revision_item_create failed to malloc v_revision_item struct"); return NULL; }
  memset (item, 0, sizeof(v_revision_item));

  return item;
}

void v_revision_item_free (void *itemptr)
{
  v_revision_item *item = itemptr;

  if (!item) return;

  free (item);
}
