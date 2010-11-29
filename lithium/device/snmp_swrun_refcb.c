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
#include <induction/postgresql.h>
#include <induction/adminstate.h>

#include "snmp.h"
#include "snmp_swrun.h"
#include "procpro.h"

int l_snmp_swrun_cnt_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when the swrun container has been refreshed.
   *
   * At this point all process data should be populated and a procpro refreh
   * can happen
   *
   * Always return 0
   */

  i_container *procpro_cnt = l_procpro_cnt ();
  if (procpro_cnt) 
  {
    /* DEBUG */
    i_printf(0, "l_snmp_swrun_cnt_refcb about to refresh procpro container with swrun container showing refresh result %i and child count of %i", 
      ent->refresh_result, ent->child_list ? ent->child_list->size : 0);
    /* END DEBUG */

    i_entity_refresh (self, ENTITY(procpro_cnt), REFFLAG_AUTO, NULL, NULL);
  }
  
  return 0;
}
