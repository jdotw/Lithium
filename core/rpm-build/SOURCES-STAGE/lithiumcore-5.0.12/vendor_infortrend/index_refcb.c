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
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "ldrive.h"

/* Index-style metric refresh callback */

int v_index_refcb (i_resource *self, i_metric *index, void *passdata)
{
  /* Called when an "index" metric is refreshed to 
   * set the value of actual to 2 ^ (index)
   *
   * ALWAYS RETURN 0
   */

  i_metric *actual = passdata;
  int index_int;
  i_metric_value *val;

  /* Get index */
  val = i_metric_curval (index);
  if (!val) 
  {
    return 0;
  }
  index_int = (int) i_metric_valflt (index, val);

  /* Set value */
  val = i_metric_value_create ();
  val->gauge = 1 << index_int;
  i_printf (0, "v_index_refcb set gauge to %li (%li)", 2 ^ index_int, val->gauge);
  i_metric_value_enqueue (self, actual, val);
  actual->refresh_result = REFRESULT_OK;

  return 0;
}
