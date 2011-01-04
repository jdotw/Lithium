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
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "port.h"

/* Fibre Channel Alliance - Port Sub-System */

i_container* v_port_enable (i_resource *self, char *unit_name, char *unit_desc, char *unit_oid_suffix)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  char *name_str;
  char *desc_str;
  asprintf(&name_str, "%s_fcport", unit_name);
  asprintf(&desc_str, "%s Ports", unit_desc);
  i_container *cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "v_port_enable failed to register container"); return NULL; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_port_enable failed to load and apply container refresh config"); return NULL; }

  /*
   * Trigger Sets 
   */

  /* FIX Needs trigger sets! */

  /*
   * Items and Object Factory
   */

  /* Create item list */
  cnt->item_list = i_list_create ();
  if (!cnt->item_list)
  { i_printf (1, "v_port_enable failed to create item_list"); return NULL; }
  i_list_set_destructor (cnt->item_list, v_port_item_free);
  cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  l_snmp_objfact *objfact = l_snmp_objfact_create (self, cnt->name_str, cnt->desc_str);
  if (!objfact)
  { 
    i_printf (1, "v_port_enable failed to call l_snmp_objfact_create to create objfact"); 
    return NULL; 
  }
  objfact->dev = self->hierarchy->dev;
  objfact->cnt = cnt;
  asprintf (&objfact->name_oid_str, ".1.3.6.1.3.94.1.10.1.18.%s", unit_oid_suffix);
  objfact->fabfunc = v_port_objfact_fab;
  objfact->ctrlfunc = v_port_objfact_ctrl;
  objfact->cleanfunc = v_port_objfact_clean;
  objfact->passdata = strdup(unit_oid_suffix);

  /* Start the object factory */
  num = l_snmp_objfact_start (self, objfact);
  if (num != 0)
  { 
    i_printf (1, "v_port_enable failed to call l_snmp_objfact_start to start the object factory");
    return NULL;
  }         

  return cnt;
}

v_port_item* v_port_item_create ()
{
  v_port_item *item;

  item = (v_port_item *) malloc (sizeof(v_port_item));
  if (!item)
  { i_printf (1, "v_port_item_create failed to malloc v_port_item struct"); return NULL; }
  memset (item, 0, sizeof(v_port_item));

  return item;
}

void v_port_item_free (void *itemptr)
{
  v_port_item *item = itemptr;

  if (!item) return;

  free (item);
}

