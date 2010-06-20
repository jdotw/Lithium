#include <stdlib.h>

#include <induction.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <induction/hierarchy.h>
#include <induction/list.h>

#include <lithium/snmp.h>

#include "input.h"

int v_input_level_refcb (i_resource *self, i_metric *met, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  v_input_item *item = passdata;

  float curval = i_metric_valflt (item->current_level, NULL);
  if (curval >= 0)
  {
    i_entity_refresh (self, ENTITY(item->remaining_pc), REFFLAG_AUTO, NULL, NULL);
  }

  return 0;
}

