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

#include "volume.h"

/* Volume Sub-System */

static i_container *static_cnt = NULL;
static l_snmp_objfact *static_objfact = NULL;

/* Variable Retrieval */

i_container* v_volume_cnt ()
{ return static_cnt; }

/* Enable */

int v_volume_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  static_cnt = i_container_create ("volume", "Volume");
  if (!static_cnt)
  { i_printf (1, "v_volume_enable failed to create container"); return -1; }

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_volume_enable failed to register container"); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_volume_enable failed to load and apply container refresh config"); return -1; }

  /*
   * Trigger Sets
   */

  /* Operational Status */
  tset = i_triggerset_create ("status", "Status", "status");
  i_triggerset_addtrg (self, tset, "degraded", "Degraded", VALTYPE_INTEGER, TRGTYPE_EQUAL, 6, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "reconfig_degraded", "Reconfig. Degraded", VALTYPE_INTEGER, TRGTYPE_EQUAL, 8, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "faulty", "Faulty", VALTYPE_INTEGER, TRGTYPE_EQUAL, 12, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "init_faulty", "Init. Faulty", VALTYPE_INTEGER, TRGTYPE_EQUAL, 13, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /*
   * Items and Object Factory
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_volume_enable failed to create item_list"); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_volume_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  static_objfact = l_snmp_objfact_create (self, "volume", "volume");
  if (!static_objfact)
  { 
    i_printf (1, "v_volume_enable failed to call l_snmp_objfact_create to create objfact"); 
    return -1; 
  }
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup (".1.3.6.1.4.1.31165.1.1.9.1.1.8");
  static_objfact->fabfunc = v_volume_objfact_fab;
  static_objfact->ctrlfunc = v_volume_objfact_ctrl;
  static_objfact->cleanfunc = v_volume_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_volume_enable failed to call l_snmp_objfact_start to start the object factory");
    return -1;
  }         

  return 0;
}

