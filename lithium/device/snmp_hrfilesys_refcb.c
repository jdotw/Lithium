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

  /* Get current value */
  val = i_metric_curval (met);
  if (!val) return 0;

  /* Enable/Disable metrics depending on access */
  if (val->integer == 1)
  {
    /* Resource is read/write, apply used_pc trigger set */
    l_snmp_storage_apply_usedpc_tset (self, met->obj);
  }
  
  return 0;
}
