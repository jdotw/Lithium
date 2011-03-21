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

#include "inputphase.h"

/* Item Struct Manipulation */

v_inputphase_item* v_inputphase_item_create ()
{
  v_inputphase_item *item;

  item = (v_inputphase_item *) malloc (sizeof(v_inputphase_item));
  if (!item)
  { i_printf (1, "v_inputphase_item_create failed to malloc v_inputphase_item struct"); return NULL; }
  memset (item, 0, sizeof(v_inputphase_item));

  return item;
}

void v_inputphase_item_free (void *itemptr)
{
  v_inputphase_item *item = itemptr;

  if (!item) return;

  free (item);
}
