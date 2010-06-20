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

#include "xsanvol.h"

/* Item Struct Manipulation */

v_xsanvol_item* v_xsanvol_item_create ()
{
  v_xsanvol_item *item;

  item = (v_xsanvol_item *) malloc (sizeof(v_xsanvol_item));
  if (!item)
  { i_printf (1, "v_xsanvol_item_create failed to malloc v_xsanvol_item struct"); return NULL; }
  memset (item, 0, sizeof(v_xsanvol_item));

  return item;
}

void v_xsanvol_item_free (void *itemptr)
{
  v_xsanvol_item *item = itemptr;

  if (!item) return;

  free (item);
}
