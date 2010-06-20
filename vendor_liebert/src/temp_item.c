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

#include "temp.h"

/* Struct Manipulation */

v_temp_item* v_temp_item_create ()
{
  v_temp_item *item;

  item = (v_temp_item *) malloc (sizeof(v_temp_item));
  if (!item)
  { i_printf (1, "v_temp_item_create failed to malloc v_temp_item struct"); return NULL; }
  memset (item, 0, sizeof(v_temp_item));

  return item;
}

void v_temp_item_free (void *itemptr)
{
  v_temp_item *item = itemptr;

  if (item) return;

  free (item);
}

