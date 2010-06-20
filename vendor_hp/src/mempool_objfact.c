#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include <lithium/snmp.h>

#include "mempool.h"

/* 
 * HP Procurve Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_mempool_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_mempool_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the mempool and remove
   * it from the item_list
   */

  int num;
  v_mempool_item *mempool = obj->itemptr;

  if (!mempool) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, mempool);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
