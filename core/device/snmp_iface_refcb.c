#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/value.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/adminstate.h>

#include "snmp.h"
#include "snmp_iface.h"
#include "navtree.h"

/* snmpiface - SNMP Interface Sub-System */

/* Metric-specific refresh callback */

int l_snmp_iface_adminstate_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when a refresh of the adminstate metric is complete
   *
   * Check admin state and disable other metrics as required
   *
   * Always return 0 to keep callback alive
   */

  i_metric_value *val;
  i_metric *child_met;
  i_metric *met = (i_metric *) ent;
  i_interface *iface = met->obj->itemptr;

  val = i_metric_curval (met);
  if (!val) return 0;

  if (val->integer != 1 && iface->disabled_metrics == 0)
  {
    /* Admin status of the interface is NOT up
     * Disable all other metric in this object
     */
    for (i_list_move_head(met->obj->met_list); (child_met=i_list_restore(met->obj->met_list))!=NULL; i_list_move_next(met->obj->met_list))
    {
      if (child_met != met)
      { i_adminstate_change (self, ENTITY(child_met), ENTADMIN_DISABLED); }
    }

    iface->disabled_metrics = 1;
  }
  else if (val->integer == 1 && iface->disabled_metrics == 1)
  {
    /* Iface is up, enable all other metrics */
    for (i_list_move_head(met->obj->met_list); (child_met=i_list_restore(met->obj->met_list))!=NULL; i_list_move_next(met->obj->met_list))
    {
      if (child_met != met)
      { i_adminstate_change (self, ENTITY(child_met), ENTADMIN_ENABLED); }
    }

    iface->disabled_metrics = 0; 
  }

  return 0;
}

/* Container refresh callback */

int l_snmp_iface_cnt_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the entire container has been refreshed. 
   * External deps such as nwtput can then be refreshed manually.
   *
   * Always return 0 to keep the callback alive 
   */

  i_container *nwtput_cnt;

  /* Get nwtput cnt */
  nwtput_cnt = (i_container *) i_entity_child_get (ENTITY(self->hierarchy->dev), "nwtput");
  if (nwtput_cnt)
  {
    i_entity_refresh_children (self, ENTITY(nwtput_cnt), REFOP_REFRESH);
    i_entity_refresh_children (self, ENTITY(nwtput_cnt), REFOP_TERMINATE);
  }

  return 0;
}
