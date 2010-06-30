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

#include "snmp.h"
#include "snmp_hrfilesys.h"

/* snmp_hrfilesys - SNMP Host File System Sub-System */

static int static_hrfilesys_enabled = 0;

int l_snmp_hrfilesys_enabled()
{ return static_hrfilesys_enabled; }

/* Sub-System Enable */

int l_snmp_hrfilesys_enable (i_resource *self)
{
  int num;
  i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  i_container *cnt = i_container_create ("hrfilesys", "Storage Resources");
  cnt->hidden = 1;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_hrfilesys_enable failed to register container"); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_hrfilesys_enable failed to load and apply container refresh config"); return -1; }

  /* Create item list */
  cnt->item_list = i_list_create ();
  cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create the object factory */
  l_snmp_objfact *objfact = l_snmp_objfact_create (self, cnt->name_str, cnt->desc_str);
  if (!objfact)
  {
    i_printf (1, "l_snmp_hrfilesys_enable failed to call l_snmp_objfact_create to create the objfact"); 
    return -1;
  }
  objfact->dev = self->hierarchy->dev;
  objfact->cnt = cnt;
  objfact->name_oid_str = strdup (".1.3.6.1.2.1.25.3.8.1.7");
  objfact->fabfunc = l_snmp_hrfilesys_objfact_fab;
  objfact->ctrlfunc = l_snmp_hrfilesys_objfact_ctrl;
  objfact->cleanfunc = l_snmp_hrfilesys_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, objfact);
  if (num != 0)
  { 
    i_printf (1, "l_snmp_hrfilesys_enable failed to call l_snmp_objfact_start to start the object factory"); 
    return -1; 
  }

  static_hrfilesys_enabled = 1;

  return 0;  
}


