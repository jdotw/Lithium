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

#include "humid.h"

/* Struct Manipulation */

v_humid_item* v_humid_item_create ()
{
  v_humid_item *item;

  item = (v_humid_item *) malloc (sizeof(v_humid_item));
  if (!item)
  { i_printf (1, "v_humid_item_create failed to malloc v_humid_item struct"); return NULL; }
  memset (item, 0, sizeof(v_humid_item));

  return item;
}

void v_humid_item_free (void *ihumidtr)
{
  v_humid_item *item = ihumidtr;

  if (item) return;

  free (item);
}

