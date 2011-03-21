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
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "unit.h"
#include "sensor.h"

/* connUnitSensor Sub-System */

i_container* v_sensor_enable (i_resource *self, v_unit_item *unit)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  char *name_str;
  char *desc_str;
  asprintf(&name_str, "fcsensor_%s", unit->obj->name_str);
  asprintf(&desc_str, "%s Sensors", unit->obj->desc_str);
  i_container *cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);

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

  /* Status */

  tset = i_triggerset_create ("status", "Status", "status");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_EQUAL, 4, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_INTEGER, TRGTYPE_EQUAL, 5, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);

  /*
   * Items and Object Factory
   */

  /* Create item list */
  cnt->item_list = i_list_create ();
  i_list_set_destructor (cnt->item_list, v_sensor_item_free);
  cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  l_snmp_objfact *objfact = l_snmp_objfact_create (self, cnt->name_str, cnt->desc_str);
  if (!objfact)
  { 
    i_printf (1, "v_sensor_enable failed to call l_snmp_objfact_create to create objfact"); 
    return NULL; 
  }
  objfact->dev = self->hierarchy->dev;
  objfact->cnt = cnt;
  asprintf(&objfact->name_oid_str, ".1.3.6.1.3.94.1.8.1.3.%s", unit->oid_suffix);
  objfact->fabfunc = v_sensor_objfact_fab;
  objfact->ctrlfunc = v_sensor_objfact_ctrl;
  objfact->cleanfunc = v_sensor_objfact_clean;
  objfact->passdata = unit;

  /* Start the object factory */
  num = l_snmp_objfact_start (self, objfact);
  if (num != 0)
  { 
    i_printf (1, "v_sensor_enable failed to call l_snmp_objfact_start to start the object factory");
    return NULL;
  }         

  return cnt;
}

v_sensor_item* v_sensor_item_create ()
{
  v_sensor_item *item;

  item = (v_sensor_item *) malloc (sizeof(v_sensor_item));
  if (!item)
  { i_printf (1, "v_sensor_item_create failed to malloc v_sensor_item struct"); return NULL; }
  memset (item, 0, sizeof(v_sensor_item));

  return item;
}

void v_sensor_item_free (void *itemptr)
{
  v_sensor_item *item = itemptr;

  if (!item) return;

  free (item);
}

