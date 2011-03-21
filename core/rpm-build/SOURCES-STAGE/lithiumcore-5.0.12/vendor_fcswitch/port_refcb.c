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
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "induction/adminstate.h"
#include "device/snmp.h"

#include "unit.h"
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

/* Admin State Refcb */

int v_port_adminstate_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when a refresh of the adminstate metric is complete
   *
   * Check admin state and disable other metrics as required
   *   * Always return 0 to keep callback alive
   */

  i_metric_value *val;
  i_metric *child_met;
  i_metric *met = (i_metric *) ent;
  v_port_item *port = met->obj->itemptr;

  val = i_metric_curval (met);
  if (!val) return 0;

  if (val->integer != 2 && port->disabled_metrics == 0)  {
    /* Admin status of the interface is NOT Online
     * Disable all other metric in this object
     */
    for (i_list_move_head(met->obj->met_list); (child_met=i_list_restore(met->obj->met_list))!=NULL; i_list_move_next(met->obj->
met_list))
    {
      if (child_met != met)
      { i_adminstate_change (self, ENTITY(child_met), ENTADMIN_DISABLED); }
    }

    port->disabled_metrics = 1;
  }
  else if (val->integer == 2 && port->disabled_metrics == 1)
  {
    /* Iface is online, enable all other metrics */
    for (i_list_move_head(met->obj->met_list); (child_met=i_list_restore(met->obj->met_list))!=NULL; i_list_move_next(met->obj->
met_list))
    {
      if (child_met != met)
      { i_adminstate_change (self, ENTITY(child_met), ENTADMIN_ENABLED); }
    }

    port->disabled_metrics = 0;
  }

  return 0;
}

/* Operational State Refcb */

int v_port_status_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when a refresh of the operational state metric is complete
   *
   * Some switches, qlogic included, report the Op Status as "Unused" which
   * according to the MIB shoudln't happen and is misleading. If the value
   * is Unused, hide the metric and instead show the txtype in its place
   *
   *   * Always return 0 to keep callback alive
   */

  i_metric_value *val;
  i_metric *met = (i_metric *) ent;
  v_port_item *port = met->obj->itemptr;

  val = i_metric_curval (met);
  if (!val) return 0;

  if (val->integer == 2 && port->status->hidden != 1)  
  {
    /* Switch uses 'Unused' for status, hide it */
    port->status->hidden = 1;
    port->txtype->summary_flag = 1; // Shows Media in place of status 
  }
  else if (val->integer != 2 && port->status->hidden == 1)
  {
    /* Switch status is sane */
    port->status->hidden = 0;
    port->txtype->summary_flag = 0; // Media would have been shown in place of status
  }

  return 0;
}

