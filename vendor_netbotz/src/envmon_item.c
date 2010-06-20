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

#include "envmon.h"

/* Host Processor Item */

/* Struct Manipulation */

v_envmon_item* v_envmon_item_create ()
{
  v_envmon_item *item;

  item = (v_envmon_item *) malloc (sizeof(v_envmon_item));
  if (!item)
  { i_printf (1, "v_envmon_item_create failed to malloc v_envmon_item struct"); return NULL; }
  memset (item, 0, sizeof(v_envmon_item));

  return item;
}

void v_envmon_item_free (void *itemptr)
{
  v_envmon_item *item = itemptr;

  if (item) return;

  free (item);
}

