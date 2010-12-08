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

#include "port.h"

/* Port Refresh Callbacks */

int v_port_speed_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  i_metric *speed_met = (i_metric *) ent;
  i_metric *bps_met = (i_metric *) passdata;
  double speedflt;

  i_metric_value *val = i_metric_value_create ();
  speedflt = i_metric_valflt (speed_met, NULL);
  if (speedflt == 106250)
  { val->flt = 1000000000.0; }
  else if (speedflt == 212500)
  { val->flt = 2000000000.0; }
  else if (speedflt == 425000)
  { val->flt = 4000000000.0; }
  else if (speedflt == 1062500)
  { val->flt = 10000000000.0; }
  i_metric_value_enqueue (self, bps_met, val);
  bps_met->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(bps_met));

  return 0;  
}
