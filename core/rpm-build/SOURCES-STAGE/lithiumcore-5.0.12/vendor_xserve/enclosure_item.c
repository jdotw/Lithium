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

#include "enclosure.h"

/* Item Struct Manipulation */

v_enclosure_item* v_enclosure_item_create ()
{
  v_enclosure_item *item;

  item = (v_enclosure_item *) malloc (sizeof(v_enclosure_item));
  if (!item)
  { i_printf (1, "v_enclosure_item_create failed to malloc v_enclosure_item struct"); return NULL; }
  memset (item, 0, sizeof(v_enclosure_item));

  return item;
}

void v_enclosure_item_free (void *itemptr)
{
  v_enclosure_item *item = itemptr;

  if (!item) return;

  free (item);
}
