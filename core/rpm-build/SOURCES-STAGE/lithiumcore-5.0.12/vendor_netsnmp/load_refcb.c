#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/interface.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/adminstate.h"

#include "device/snmp.h"

#include "load.h"

/* load - Load Average Sub-System */

/* Metric-specific refresh callback */

int l_snmp_nsload_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the raw (integer) load has been refreshed.
   * A float value is created for the real load metric
   * Always return 0
   */

  int num;
  i_metric_value *val;
  i_metric_value *new_val;
  i_metric *met = (i_metric *) ent;
  i_metric *load_met = NULL;
  l_snmp_nsload_item *load = (l_snmp_nsload_item *) met->obj->itemptr;

  /* Get current value */
  val = i_metric_curval (met);
  if (!val) return 0;

  /* Create/enqueue type value */
  new_val = i_metric_value_create ();
  new_val->flt = ((float) val->integer) * 0.01f;
  if (strcmp(met->name_str, "one_min_raw") == 0)
  { load_met = load->one_min; }
  else if (strcmp(met->name_str, "five_min_raw") == 0)
  { load_met = load->five_min; }
  else if (strcmp(met->name_str, "fifteen_min_raw") == 0)
  { load_met = load->fifteen_min; }
  num = i_metric_value_enqueue (self, load_met, new_val);
  if (num != 0) 
  { i_metric_value_free (new_val); }
  load_met->refresh_result = REFRESULT_OK;

  return 0;
}
