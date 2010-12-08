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

#include "fans.h"

/* Fans Sub-System */

static i_container *static_cnt = NULL;
static l_snmp_objfact *static_objfact = NULL;

/* Variable Retrieval */

i_container* v_fans_cnt ()
{ return static_cnt; }

/* Enable */

int v_fans_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  static_cnt = i_container_create ("asfans", "Fans");

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_fans_enable failed to register container");return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_fans_enable failed to load and apply container refresh config"); return -1; }

  /*
   * Trigger Sets
   */

  /* Operational Status */
  tset = i_triggerset_create ("status", "Status", "status");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_EQUAL, 2, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "error", "Error", VALTYPE_INTEGER, TRGTYPE_EQUAL, 3, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /*
   * Items and Object Factory
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  i_list_set_destructor (static_cnt->item_list, v_fans_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Create object factory */
  static_objfact = l_snmp_objfact_create (self, "fans", "Fans");
  static_objfact->dev = self->hierarchy->dev;
  static_objfact->cnt = static_cnt;
  static_objfact->name_oid_str = strdup (".1.3.6.1.4.1.31165.1.1.1.1.1.5");
  static_objfact->fabfunc = v_fans_objfact_fab;
  static_objfact->ctrlfunc = v_fans_objfact_ctrl;
  static_objfact->cleanfunc = v_fans_objfact_clean;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, static_objfact);
  if (num != 0)
  { 
    i_printf (1, "v_fans_enable failed to call l_snmp_objfact_start to start the object factory");
    return -1;
  }         

  return 0;
}


