#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "xsanvisdisk.h"

/* Item Struct Manipulation */

v_xsanvisdisk_item* v_xsanvisdisk_item_create ()
{
  v_xsanvisdisk_item *item;

  item = (v_xsanvisdisk_item *) malloc (sizeof(v_xsanvisdisk_item));
  if (!item)
  { i_printf (1, "v_xsanvisdisk_item_create failed to malloc v_xsanvisdisk_item struct"); return NULL; }
  memset (item, 0, sizeof(v_xsanvisdisk_item));

  return item;
}

void v_xsanvisdisk_item_free (void *itemptr)
{
  v_xsanvisdisk_item *item = itemptr;

  if (!item) return;

  free (item);
}
