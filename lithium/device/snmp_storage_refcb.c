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
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/adminstate.h>

#include "snmp.h"
#include "snmp_storage.h"

/* snmp_storage - SNMP Storage Resources Sub-System */

/* Metric-specific refresh callback */

int l_snmp_storage_typeoid_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the type metric has been refresh to:
   *
   * 1) Update the type state metric
   * 2) Analyse the type and enable/disable metrics
   *
   * Always return 0
   */

  int num;
  char *oid;
  i_metric_value *val;
  i_metric_value *new_val;
  i_metric *met = (i_metric *) ent;
  i_metric *child_met;
  l_snmp_storage_item *store = (l_snmp_storage_item *) met->obj->itemptr;

  /* Get current value */
  val = i_metric_curval (met);
  if (!val) return 0;
  oid = val->oid;

  /* Create/enqueue type value */
  new_val = i_metric_value_create ();
  new_val->integer = oid[val->oid_len-1];
  num = i_metric_value_enqueue (self, store->type, new_val);
  if (num != 0) 
  { i_metric_value_free (new_val); }
  store->type->refresh_result = REFRESULT_OK;

  /* Disable metrics depending on type */
  if ((int) oid[val->oid_len-1] == 1 || (int) oid[val->oid_len-1] == 7)
  {
    for (i_list_move_head(met->obj->met_list); (child_met=i_list_restore(met->obj->met_list))!=NULL; i_list_move_next(met->obj->met_list))
    {
      i_adminstate_change (self, ENTITY(child_met), ENTADMIN_DISABLED);
    }
  }
  
  return 0;
}

int l_snmp_storage_writeable_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the writeable metric has been refresh to enable/disable metrics
   *
   * Always return 0
   */

  i_metric_value *val;
  i_metric *met = (i_metric *) ent;

  /* Get current value */
  val = i_metric_curval (met);
  if (!val) return 0;

  /* Enable metrics depending on type */
  if (val->integer == 1)
  {
    /* Volume is writeable, apply used_pc triggerset */
    l_snmp_storage_apply_usedpc_tset (self, met->obj);
  }
  
  return 0;
}
