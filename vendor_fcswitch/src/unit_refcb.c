#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/snmp.h>

#include "port.h"
#include "unit.h"

/* Port Refresh Callbacks */

int v_unit_switchname_refcb (i_resource *self, i_entity *ent, void *passdata)
{
  i_metric *name_met = (i_metric *) ent;
  v_unit_item *unit = passdata;
  
  /* Get name */
  char *name_val = i_metric_valstr (name_met, NULL);
  if (!name_val) return 0;

  /* Loop through interfaces and set names */
  i_container *cnt = v_port_cnt ();
  i_object *obj;
  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    v_port_item *port = obj->itemptr;
    if (!port->desc_updated && port->switch_domain == unit->switch_domain)
    {
      /* Update desc */
      if (obj->desc_str) free (obj->desc_str);
      asprintf (&obj->desc_str, "Switch %s Port %i", name_val, port->switch_port);
      port->desc_updated = 1;
    }
  }

  return 0;  
}
