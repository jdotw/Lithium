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
#include "slot.h"

/* Brocade FC Switch Ports (Slot-Specific)
 *
 * This sub-system is enabled by the port sub-system when a
 * port is found that uses a slot/port specifier (name). 
 *
 * The prefix supplied is the "slot/" prefix of the slot 
 *
 * Otherwise, the intrface will be added to this container. 
 */

/* Enable */

int v_slot_enable (i_resource *self, char *prefix_str)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_container *cnt;
  l_snmp_objfact *objfact;

  /* Create/Config Container */
  char *name_str;   // Freed at end of function, used for objfact too
  char *desc_str;
  asprintf(&name_str, "fcslot_%s", prefix_str);
  asprintf(&desc_str, "Slot %s", prefix_str);
  cnt = i_container_create (name_str, desc_str);
  if (!cnt)
  { i_printf (1, "v_slot_enable failed to create container"); return -1; }

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "v_slot_enable failed to register container"); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_slot_enable failed to load and apply container refresh config"); return -1; }

  /*
   * Trigger Sets 
   */

  /*
   * Items and Object Factory
   */

  /* Create item list */
  cnt->item_list = i_list_create ();
  i_list_set_destructor (cnt->item_list, v_port_item_free);
  cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  objfact = l_snmp_objfact_create (self, name_str, desc_str);
  if (!objfact)
  { 
    i_printf (1, "v_slot_enable failed to call l_snmp_objfact_create to create objfact"); 
    return -1; 
  }
  objfact->dev = self->hierarchy->dev;
  objfact->cnt = cnt;
  objfact->name_oid_str = strdup (".1.3.6.1.4.1.1588.2.1.1.1.6.2.1.37");
  objfact->fabfunc = v_slot_objfact_fab;
  objfact->ctrlfunc = v_slot_objfact_ctrl;
  objfact->cleanfunc = v_slot_objfact_clean;
  objfact->passdata = strdup(prefix_str);

  /* Start the object factory */
  num = l_snmp_objfact_start (self, objfact);
  if (num != 0)
  { 
    i_printf (1, "v_slot_enable failed to call l_snmp_objfact_start to start the object factory");
    return -1;
  }         

  /* Cleanup */
  free (name_str);
  free (desc_str);
  
  return 0;
}

