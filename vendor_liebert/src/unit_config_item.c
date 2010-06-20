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

#include "unit_config.h"

/* Struct Manipulation */

v_config_item* v_config_item_create ()
{
  v_config_item *item;

  item = (v_config_item *) malloc (sizeof(v_config_item));
  if (!item)
  { i_printf (1, "v_config_item_create failed to malloc v_config_item struct"); return NULL; }
  memset (item, 0, sizeof(v_config_item));

  return item;
}

void v_config_item_free (void *iconfigtr)
{
  v_config_item *item = iconfigtr;

  if (item) return;

  free (item);
}

