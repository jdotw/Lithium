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

#include "ldrive.h"

/* Cisco Memory Pool Sub-System */

/* Item Struct Manipulation */

v_ldrive_item* v_ldrive_item_create ()
{
  v_ldrive_item *item;

  item = (v_ldrive_item *) malloc (sizeof(v_ldrive_item));
  if (!item)
  { i_printf (1, "v_ldrive_item_create failed to malloc v_ldrive_item struct"); return NULL; }
  memset (item, 0, sizeof(v_ldrive_item));

  return item;
}

void v_ldrive_item_free (void *itemptr)
{
  v_ldrive_item *item = itemptr;

  if (!item) return;

  free (item);
}
