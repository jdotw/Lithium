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

#include "outphase.h"

/* Item Struct Manipulation */

v_outphase_item* v_outphase_item_create ()
{
  v_outphase_item *item;

  item = (v_outphase_item *) malloc (sizeof(v_outphase_item));
  if (!item)
  { i_printf (1, "v_outphase_item_create failed to malloc v_outphase_item struct"); return NULL; }
  memset (item, 0, sizeof(v_outphase_item));

  return item;
}

void v_outphase_item_free (void *itemptr)
{
  v_outphase_item *item = itemptr;

  if (!item) return;

  free (item);
}
