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
#include "induction/value.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/snmp.h"
#include "device/record.h"

#include "cpu.h"

int l_snmp_nscpu_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when a Raw value (ticks) has been 
   * refreshed. This function takes the delta
   * ticks, examines the time passed and calculates
   * a percentage
   */

  i_metric *raw = (i_metric *) ent;
  i_metric *pc = passdata;

  i_list_move_head (raw->val_list);
  i_metric_value *last = i_list_restore (raw->val_list);
  i_list_move_next (raw->val_list);
  i_metric_value *before = i_list_restore (raw->val_list);

  if (!last || !before) 
  {
    pc->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(pc));
    return 0;
  }

  time_t delta_time = last->tstamp.tv_sec - before->tstamp.tv_sec;
  if (delta_time > (5 * self->hierarchy->dev->refresh_interval) || last->count < before->count) 
  {
    pc->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(pc));
    return 0;
  }
  
  unsigned long delta_ticks = last->count - before->count;
  float usage = (float) (((float) delta_ticks / (float) (delta_time * 100)) * 100.0f); 

  i_metric_value *val = i_metric_value_create ();
  val->flt = usage;

  i_metric_value_enqueue (self, pc, val);
  pc->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(pc));

  return 0;
}
