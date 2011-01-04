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

#include "revision.h"

/* connUnitSensor Sub-System */

i_container* v_revision_enable (i_resource *self, char *unit_name, char *unit_desc, char *unit_oid_suffix)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  char *name_str;
  char *desc_str;
  asprintf(&name_str, "fcrevision_%s", unit_name);
  asprintf(&desc_str, "%s Revisions", unit_desc);
  i_container *cnt = i_container_create (name_str, desc_str);
  free (unit_name);
  free (unit_desc);

  /* Register entity */
  i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);

  /*
   * Trigger Sets 
   */

  /*
   * Items and Object Factory
   */

  /* Create item list */
  cnt->item_list = i_list_create ();
  if (!cnt->item_list)
  { i_printf (1, "v_revision_enable failed to create item_list"); v_revision_disable (self); return -1; }
  i_list_set_destructor (cnt->item_list, v_revision_item_free);
  cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  l_snmp_objfact *objfact = l_snmp_objfact_create (self, "revision", "Revisions");
  objfact->dev = self->hierarchy->dev;
  objfact->cnt = cnt;
  asprintf(&objfact->name_oid_str, ".1.3.6.1.3.94.1.7.1.4.%s", unit_oid_suffix);
  objfact->fabfunc = v_revision_objfact_fab;
  objfact->ctrlfunc = v_revision_objfact_ctrl;
  objfact->cleanfunc = v_revision_objfact_clean;
  objfact->passdata = strdup(unit_oid_suffix);

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_revision_enable failed to call l_snmp_objfact_start to start the object factory");
    return NULL;
  }         

  return cnt;
}


