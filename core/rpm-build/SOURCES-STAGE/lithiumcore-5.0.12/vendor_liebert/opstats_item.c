#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"

#include "opstats.h"

/* Struct Manipulation */

v_opstats_item* v_opstats_item_create ()
{
  v_opstats_item *item;

  item = (v_opstats_item *) malloc (sizeof(v_opstats_item));
  if (!item)
  { i_printf (1, "v_opstats_item_create failed to malloc v_opstats_item struct"); return NULL; }
  memset (item, 0, sizeof(v_opstats_item));

  return item;
}

void v_opstats_item_free (void *iopstatstr)
{
  v_opstats_item *item = iopstatstr;

  if (item) return;

  free (item);
}

