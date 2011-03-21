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

#include "status.h"

/* Item Struct Manipulation */

v_status_item* v_status_item_create ()
{
  v_status_item *item;

  item = (v_status_item *) malloc (sizeof(v_status_item));
  if (!item)
  { i_printf (1, "v_status_item_create failed to malloc v_status_item struct"); return NULL; }
  memset (item, 0, sizeof(v_status_item));

  return item;
}

void v_status_item_free (void *itemptr)
{
  v_status_item *item = itemptr;

  if (!item) return;

  free (item);
}
