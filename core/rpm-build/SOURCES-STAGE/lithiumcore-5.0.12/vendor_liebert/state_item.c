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

#include "state.h"

/* Struct Manipulation */

v_state_item* v_state_item_create ()
{
  v_state_item *item;

  item = (v_state_item *) malloc (sizeof(v_state_item));
  if (!item)
  { i_printf (1, "v_state_item_create failed to malloc v_state_item struct"); return NULL; }
  memset (item, 0, sizeof(v_state_item));

  return item;
}

void v_state_item_free (void *istatetr)
{
  v_state_item *item = istatetr;

  if (item) return;

  free (item);
}

