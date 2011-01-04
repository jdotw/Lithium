#include <stdlib.h>

#include "induction.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"
#include "induction/hierarchy.h"
#include "induction/list.h"

#include "device/snmp.h"

#include "port.h"
#include "unit.h"
#include "sensor.h"

/* 
 * Physical Unit - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_unit_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_unit_item *unit;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu(pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_unit_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create unit item struct */
  unit = v_unit_item_create ();
  if (!unit)
  { i_printf (1, "v_unit_objfact_fab failed to create unit item for object %s", obj->name_str); return -1; }
  unit->obj = obj;
  obj->itemptr = unit;

  /* 
   * Metric Creation 
   */

  i_entity_refresh_config refconfig;
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* FIX Add Unit Triggers */
            
  /* Enqueue the unit item */
  num = i_list_enqueue (cnt->item_list, unit);
  if (num != 0)
  { i_printf (1, "v_unit_objfact_fab failed to enqueue unit for object %s", obj->name_str); v_unit_item_free (unit); return -1; }

  /* Create port container */
  unit->port_cnt = v_port_enable (self, obj->name_str, obj->desc_str, index_oidstr);
  unit->sensor_cnt = v_sensor_enable (self, obj->name_str, obj->desc_str, index_oidstr);

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_unit_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_unit_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the unit and remove
   * it from the item_list
   */

  int num;
  v_unit_item *unit = obj->itemptr;

  if (!unit) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, unit);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
