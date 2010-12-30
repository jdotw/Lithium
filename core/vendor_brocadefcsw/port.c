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

/* Brocade FC Switch Ports 
 *
 * This sub-system walks the port list, and if it find a port
 * with a "slot/port" specified then it will attempt to 
 * enable a slot sub-system just for that slot.
 *
 * Otherwise, the intrface will be added to this container. 
 */

static i_container *static_cnt = NULL;
static l_snmp_objfact *static_objfact = NULL;

/* Variable Retrieval */

i_container* v_port_cnt ()
{ return static_cnt; }

/* Enable */

int v_port_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("fcport", "Other Ports");
  if (!static_cnt)
  { i_printf (1, "v_port_enable failed to create container"); return -1; }

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_port_enable failed to register container"); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_port_enable failed to load and apply container refresh config"); return -1; }

  /*
   * Trigger Sets 
   */

  /*
   * Items and Object Factory
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_port_enable failed to create item_list"); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_port_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  static_objfact = l_snmp_objfact_create (self, "fcport", "Other Ports");
  if (!static_objfact)
  { 
    i_printf (1, "v_port_enable failed to call l_snmp_objfact_create to create objfact"); 
    return -1; 
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup (".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.37");
  static_objfact->fabfunc = v_port_objfact_fab;
  static_objfact->ctrlfunc = v_port_objfact_ctrl;
  static_objfact->cleanfunc = v_port_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_port_enable failed to call l_snmp_objfact_start to start the object factory");
    return -1;
  }         

  return 0;
}

