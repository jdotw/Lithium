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

int l_snmp_storage_obj_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the storage object has been refresh to:
   *
   * 1) Update the type state metric
   * 2) Analyse the type and enable/disable metrics
   *
   * Always return 0
   */

  i_object *obj = (i_object *) ent;
  l_snmp_storage_item *store = (l_snmp_storage_item *) obj->itemptr;

  if (l_snmp_xsnmp_enabled())
  {
    i_metric_value *val;

    /* Get current value */
    val = i_metric_curval (store->writeable);
    i_printf (1, "l_snmp_storage_obj_refcb val=%p val->integer=%i usedpc_trigger_applied=%i", val, val ? val->integer : -1, store->usedpc_trigger_applied);
    if (!val) return 0;

    /* Enable metrics depending on type */
    if (val->integer == 1 && !store->usedpc_trigger_applied)
    {
      /* Volume is writeable, apply used_pc triggerset */
      i_triggerset *tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
      i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 97, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
      i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 97, NULL, 99, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
      i_triggerset_addtrg (self, tset, "critical", "Critical", VALTYPE_FLOAT, TRGTYPE_GT, 99, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
      i_triggerset_assign_obj (self, obj, tset);
      store->usedpc_trigger_applied = 1;
      i_triggerset_evalapprules_allsets (self, obj);
      i_printf (1, "*** Applying");
    }
  }
  else
  {
    int num;
    char *oid;
    i_metric_value *val;
    i_metric_value *new_val;
    i_metric *child_met;

    /* Get current type value */
    val = i_metric_curval (store->typeoid);
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
      for (i_list_move_head(obj->met_list); (child_met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
      {
        i_adminstate_change (self, ENTITY(child_met), ENTADMIN_DISABLED);
      }
    }
  } 

  return 0;
}

