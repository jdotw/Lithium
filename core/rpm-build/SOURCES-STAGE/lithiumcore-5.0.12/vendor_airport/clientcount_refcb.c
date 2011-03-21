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
#include "device/record.h"
#include "device/snmp.h"

#include "clientcount.h"

int v_clientcount_clientlist_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the client list object factory has been refreshed */
  l_snmp_objfact *objfact = passdata;
  v_clientcount_item *item = v_clientcount_static_item ();

  /* Set value */
  if (ent->refresh_result == REFRESULT_OK)
  {
    i_metric_value *val = i_metric_value_create ();
    if (objfact->cnt->obj_list)
    { val->gauge = objfact->cnt->obj_list->size; }
    else
    { val->gauge = 0; }
    i_metric_value_enqueue (self, item->count, val);
    item->count->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(item->count));
  }
  else
  {
    item->count->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(item->count));
  }

  return 0;
}
