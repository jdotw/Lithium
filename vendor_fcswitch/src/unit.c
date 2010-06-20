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
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/snmp.h>

#include "unit.h"

/* Switch Physical Unit Sub-System */

static i_container *static_cnt = NULL;
static l_snmp_objfact *static_objfact = NULL;

/* Variable Retrieval */

i_container* v_unit_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_unit_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("fcunit", "Physical Unit");
  if (!static_cnt)
  { i_printf (1, "v_unit_enable failed to create container"); v_unit_disable (self); return -1; }
//  static_cnt->mainform_func = v_unit_cntform;
//  static_cnt->sumform_func = v_unit_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_unit_enable failed to register container"); v_unit_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_unit_enable failed to load and apply container refresh config"); v_unit_disable (self); return -1; }

  /*
   * Trigger Sets 
   */

  /*
   * Items and Object Factory
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_unit_enable failed to create item_list"); v_unit_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_unit_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  static_objfact = l_snmp_objfact_create (self, "unit", "Physical Unit");
  if (!static_objfact)
  { 
    i_printf (1, "v_unit_enable failed to call l_snmp_objfact_create to create objfact"); 
    v_unit_disable (self);
    return -1; 
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup ("experimental.94.1.6.1.11");
  static_objfact->namesource = OBJFACT_NAME_HEXSTRING;
  static_objfact->fabfunc = v_unit_objfact_fab;
  static_objfact->ctrlfunc = v_unit_objfact_ctrl;
  static_objfact->cleanfunc = v_unit_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_unit_enable failed to call l_snmp_objfact_start to start the object factory");
    v_unit_disable (self);
    return -1;
  }         

  return 0;
}

int v_unit_disable (i_resource *self)
{
  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

