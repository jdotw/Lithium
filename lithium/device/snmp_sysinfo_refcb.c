#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "record.h"
#include "snmp.h"
#include "snmp_sysinfo.h"

/* SNMP System Information MIB */

int l_snmp_sysinfo_uptime_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  /* Called when uptime has been refreshed. If there is a
   * discontinuity, all SNMP object factories must be refreshed
   * to ensure accuracy
   *
   * Always return 0 to keep the callback alive
   */

  i_metric_value *cur_val;
  double cur_float;
  i_metric_value *prev_val;
  double prev_float;
  i_metric *met = (i_metric *) ent;

  /* Get values */
  i_list_move_head (met->val_list);
  cur_val = i_list_restore (met->val_list);
  if (!cur_val) return 0;
  i_list_move_next (met->val_list);
  prev_val = i_list_restore (met->val_list);
  if (!prev_val) return 0;

  /* Check values */
  cur_float = i_metric_valflt (met, cur_val);
  prev_float = i_metric_valflt (met, prev_val);
  if (cur_float < prev_float)
  {
    /* Uptime discontinuity has occurred. This indicates the 
     * system and/or snmp agent have been restarted. 
     * All object factories must now be restarted
     */
    int num;
    i_container *objfact_cnt = l_snmp_objfact_cnt();
    i_object *obj;

    /* Log */
    i_printf (0, "l_snmp_sysinfo_uptime_refcb refreshing all objects factories after uptime discontinuity (%s)",
      self->hierarchy->dev->name_str);

    /* Look through objfacts and refresh */
    for (i_list_move_head(objfact_cnt->obj_list); (obj=i_list_restore(objfact_cnt->obj_list))!=NULL; i_list_move_next(objfact_cnt->obj_list))
    {
      /* Refresh objfact */
      num = i_entity_refresh (self, ENTITY(obj), REFFLAG_AUTO, NULL, NULL);
      if (num == -1)
      { i_printf (1, "l_snmp_sysinfo_uptime_refcb failed to call refresh on %s object factory after uptime discontinuity", obj->name_str); }
    }
  }

  return 0;
}
