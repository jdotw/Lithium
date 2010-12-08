#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>

#include "xsaninfo.h"

v_xsaninfo_item* v_xsaninfo_item_create ()
{
  v_xsaninfo_item *item;

  item = (v_xsaninfo_item *) malloc (sizeof(v_xsaninfo_item));
  if (!item)
  { i_printf (1, "v_xsaninfo_item_create failed to malloc v_xsaninfo_item struct"); return NULL; }
  memset (item, 0, sizeof(v_xsaninfo_item));

  return item;
}

void v_xsaninfo_item_free (void *itemptr)
{
  v_xsaninfo_item *item = itemptr;

  if (item) return;

  free (item);
}

