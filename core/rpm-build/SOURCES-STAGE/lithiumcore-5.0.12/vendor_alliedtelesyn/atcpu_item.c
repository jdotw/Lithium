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

#include "atcpu.h"

/* Struct Manipulation */

v_atcpu_item* v_atcpu_item_create ()
{
  v_atcpu_item *item;

  item = (v_atcpu_item *) malloc (sizeof(v_atcpu_item));
  if (!item)
  { i_printf (1, "v_atcpu_item_create failed to malloc v_atcpu_item struct"); return NULL; }
  memset (item, 0, sizeof(v_atcpu_item));

  return item;
}

void v_atcpu_item_free (void *itemptr)
{
  v_atcpu_item *item = itemptr;

  if (item) return;

  free (item);
}

