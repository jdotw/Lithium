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

/* Storage/FileSystem Access Callback */

int l_snmp_hrfilesys_access_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the 'access' metric has been refreshed
   * to disble triggers for read-only resources
   *
   * Always return 0
   */

  i_metric_value *val;
  i_metric *met = (i_metric *) ent;
  l_snmp_storage_item *store = (l_snmp_storage_item *) met->obj->itemptr;

  /* Get current value */
  val = i_metric_curval (met);
  if (!val) return 0;

  /* Enable/Disable metrics depending on access */
  if (val->integer == 1 && !store->usedpc_trigger_applied)
  {
    /* Resource is read/write, apply used_pc trigger set */
    i_triggerset *tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
    i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 97, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 97, NULL, 99, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
    i_triggerset_addtrg (self, tset, "critical", "Critical", VALTYPE_FLOAT, TRGTYPE_GT, 99, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
    i_triggerset_assign_obj (self, met->obj, tset);
    store->usedpc_trigger_applied = 1;
    i_triggerset_evalapprules_allsets (self, met->obj);
  }
  
  return 0;
}
