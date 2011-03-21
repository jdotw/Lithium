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

#include "fantray.h"

/* 3Com fantray Sub-System */

/* Item Struct Manipulation */

v_fantray_item* v_fantray_item_create ()
{
  v_fantray_item *item;

  item = (v_fantray_item *) malloc (sizeof(v_fantray_item));
  if (!item)
  { i_printf (1, "v_fantray_item_create failed to malloc v_fantray_item struct"); return NULL; }
  memset (item, 0, sizeof(v_fantray_item));

  return item;
}

void v_fantray_item_free (void *itemptr)
{
  v_fantray_item *item = itemptr;

  if (!item) return;

  free (item);
}
