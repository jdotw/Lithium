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

#include "xsancomputer.h"

/* Item Struct Manipulation */

v_xsancomputer_item* v_xsancomputer_item_create ()
{
  v_xsancomputer_item *item;

  item = (v_xsancomputer_item *) malloc (sizeof(v_xsancomputer_item));
  if (!item)
  { i_printf (1, "v_xsancomputer_item_create failed to malloc v_xsancomputer_item struct"); return NULL; }
  memset (item, 0, sizeof(v_xsancomputer_item));

  return item;
}

void v_xsancomputer_item_free (void *itemptr)
{
  v_xsancomputer_item *item = itemptr;

  if (!item) return;

  free (item);
}
