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
#include "snmp_hrfilesys.h"

/* snmp_storage - SNMP Storage Resources Sub-System */

/* Metric-specific refresh callback */

int l_snmp_storage_obj_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the storage object has been refreshed to 
   * determine whether or not to apply the used_pc trigger
   * to the object. 
   *
   * Always return 0 to keep callback alive
   */

  i_object *obj = (i_object *) ent;
  l_snmp_storage_item *store = (l_snmp_storage_item *) obj->itemptr;
  int apply_usedpc_trigger = 0;

  if (l_snmp_xsnmp_enabled())
  {
    /* Use the Xsnmp 'writeable' parameter to determine whether or
     * not to apply the used_pc trigger. Only apply the used_pc trigger
     * if the volume is writeable 
     */
    
    /* Get current value */
    i_metric_value *val = i_metric_curval (store->writeable);
    if (!val) return 0;

    /* Enable used_pc trigger depending on whether or not the store is writable*/
    if (val->integer == 1 && !store->usedpc_trigger_applied)
    {
      apply_usedpc_trigger = 1;
      i_printf(0, "DEBUG: l_snmp_storage_obj_refcb(%s): setting apply_usedpc_trigger to 1", obj->desc_str);
    }
  }
  else if (l_snmp_hrfilesys_enabled())
  {
    /* Use the 'access' parameter of the hrfilesys extensions to 
     * determine whether or not to apply the trigger. Only apply the
     * used_pc trigger if the volume is read/write
     */

    i_metric_value *access_val = i_metric_curval(store->access);
    if (!access_val) return 0;

    /* Enable used_pc trigger if access is 1 (readWrite) */
    if (access_val->integer == 1 && !store->usedpc_trigger_applied)
    {
      apply_usedpc_trigger = 1;
      i_printf(0, "DEBUG: l_snmp_storage_obj_refcb(%s): setting apply_usedpc_trigger to 1", obj->desc_str);
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
    if (!val || val->oid_len < 10) return 0;
    oid = val->oid;

    /* Create/enqueue type value */
    new_val = i_metric_value_create ();
    new_val->integer = oid[9];
    num = i_metric_value_enqueue (self, store->type, new_val);
    if (num != 0) 
    { i_metric_value_free (new_val); }
    store->type->refresh_result = REFRESULT_OK;

    /* Disable metrics depending on type */
    if ((int) oid[9] == 1 || (int) oid[9] == 7)
    {
      for (i_list_move_head(obj->met_list); (child_met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
      {
        i_adminstate_change (self, ENTITY(child_met), ENTADMIN_DISABLED);
      }
      i_printf(0, "DEBUG: l_snmp_storage_obj_refcb(%s): disabled all metrics", obj->desc_str);
    }
    else
    { 
      apply_usedpc_trigger = 1; 
      i_printf(0, "DEBUG: l_snmp_storage_obj_refcb(%s): setting apply_usedpc_trigger to 1", obj->desc_str);
    }
  } 

  if (apply_usedpc_trigger == 1 && !store->usedpc_trigger_applied)
  {
    /* Volume is writeable, apply used_pc and used triggersets */
    i_triggerset *tset;
   
    i_printf(0, "DEBUG: l_snmp_storage_obj_refcb(%s): applied percent used and used triggersets", obj->desc_str);
    
    tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 97, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 97, NULL, 99, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "critical", "Critical", VALTYPE_FLOAT, TRGTYPE_GT, 99, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
    i_triggerset_assign_obj (self, obj, tset);
    
    tset = i_triggerset_create ("used", "Used", "used");
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 0, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 0, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "critical", "Critical", VALTYPE_FLOAT, TRGTYPE_GT, 0, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
    tset->default_applyflag = 0;  // Not enabled by default
    i_triggerset_assign_obj (self, obj, tset);
    
    tset = i_triggerset_create ("free", "Free", "free");
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 0, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 0, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "critical", "Critical", VALTYPE_FLOAT, TRGTYPE_LT, 0, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
    tset->default_applyflag = 0;  // Not enabled by default
    i_triggerset_assign_obj (self, obj, tset);
    
    store->usedpc_trigger_applied = 1;
    i_triggerset_evalapprules_allsets (self, obj);
  }

  return 0;
}

