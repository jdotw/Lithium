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

#include "psu.h"

/* VTrak Power Supply Unit Sub-System */

static i_container *static_cnt = NULL;
static l_snmp_objfact *static_objfact = NULL;

/* Variable Retrieval */

i_container* v_psu_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_psu_enable (i_resource *self, int enclosure, char *oidindex_str)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  char *name_str;
  char *desc_str;
  asprintf (&name_str, "vtpsu_%i", enclosure);
  asprintf (&desc_str, "Enclosure %i Power", enclosure);
  static_cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  if (!static_cnt)
  { i_printf (1, "v_psu_enable failed to create container"); v_psu_disable (self); return -1; }

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_psu_enable failed to register container"); v_psu_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_psu_enable failed to load and apply container refresh config"); v_psu_disable (self); return -1; }

  /*
   * Trigger Sets
   */

  /* Operational Status */
  tset = i_triggerset_create ("opstate", "Operational Status", "opstate");
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "Powered On and Functional", 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /*
   * Items and Object Factory
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_psu_enable failed to create item_list"); v_psu_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_psu_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  asprintf (&name_str, "vtpsu_%i", enclosure);
  asprintf (&desc_str, "Enclosure %i Power", enclosure);
  static_objfact = l_snmp_objfact_create (self, name_str, desc_str);
  free (name_str);
  free (desc_str);
  if (!static_objfact)
  { 
    i_printf (1, "v_psu_enable failed to call l_snmp_objfact_create to create objfact"); 
    v_psu_disable (self);
    return -1; 
  }
  char *name_oid_str;
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  asprintf (&name_oid_str, ".1.3.6.1.4.1.7933.1.20.1.12.1.1.%s", oidindex_str);
  static_objfact->name_oid_str = strdup (name_oid_str);
  free (name_oid_str);
  static_objfact->fabfunc = v_psu_objfact_fab;
  static_objfact->ctrlfunc = v_psu_objfact_ctrl;
  static_objfact->cleanfunc = v_psu_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_psu_enable failed to call l_snmp_objfact_start to start the object factory");
    v_psu_disable (self);
    return -1;
  }         

  return 0;
}

int v_psu_disable (i_resource *self)
{
  /* Terminate the object factory */
  if (static_objfact)
  { l_snmp_objfact_stop (self, static_objfact); static_objfact = NULL; }

  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

