#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "output.h"

int v_output_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Looks over the boolean status OIDs returned by a 
   * MGE UPS and construct a single status metric
   */

  i_object *obj = (i_object *) ent;
  v_output_item *output = obj->itemptr;
  i_metric_value *val = i_metric_value_create ();
  val->integer = 0;

  if (obj->refresh_result != REFRESULT_OK || !i_metric_curval(output->onbattery))
  { return 0; }

  if (i_metric_valflt(output->onbattery, NULL) == 1)
  { val->integer = 3; }
  else if (i_metric_valflt(output->onboost, NULL) == 1)
  { val->integer = 4; }
  else if (i_metric_valflt(output->utilityoff, NULL) == 1)
  { val->integer = 7; }
  else if (i_metric_valflt(output->onbypass, NULL) == 1)
  { val->integer = 9; }
  else if (i_metric_valflt(output->ontrim, NULL) == 1)
  { val->integer = 12; }
  else 
  { val->integer = 2; }

  i_metric_value_enqueue (self, output->status, val);
  output->status->refresh_result = REFRESULT_OK;

  return 0;
}
