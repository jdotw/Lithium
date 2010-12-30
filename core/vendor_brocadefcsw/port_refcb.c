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
#include "induction/adminstate.h"
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
#include "device/snmp.h"

#include "port.h"

/* Port Refresh Callbacks */

int v_port_speed_enum_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  i_metric *enum_met = (i_metric *) ent;
  i_metric *bps_met = (i_metric *) passdata;

  i_metric_value *enum_val = i_metric_curval(enum_met);
  if (enum_val)
  {
    i_metric_value *val = i_metric_value_create ();
    switch (enum_val->integer)
    {
      case 1:
        val->flt = 1.0 * 1000.0 * 1000.0 * 1000.0;
        break;
      case 2:
        val->flt = 2.0 * 1000.0 * 1000.0 * 1000.0;
        break;
      case 4:
        val->flt = 4.0 * 1000.0 * 1000.0 * 1000.0;
        break;
      case 5:
        val->flt = 8.0 * 1000.0 * 1000.0 * 1000.0;
        break;
      case 6:
        val->flt = 10.0 * 1000.0 * 1000.0 * 1000.0;
        break;
      default:
        i_metric_value_free(val);
        val = NULL;
        break;
    }
    if (val)
    {
      i_metric_value_enqueue (self, bps_met, val);
      bps_met->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(bps_met));
    }
  }

  return 0;  
}

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

  if (val->integer != 1 && port->disabled_metrics == 0)  {
    /* Admin status of the interface is NOT up
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
  else if (val->integer == 1 && port->disabled_metrics == 1)
  {
    /* Iface is up, enable all other metrics */
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


