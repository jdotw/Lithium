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

#include "intenv.h"

/* 
 * UPS Devices - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_intenv_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_intenv_item *intenv;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_intenv_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create intenv item struct */
  intenv = v_intenv_item_create ();
  if (!intenv)
  { i_printf (1, "v_intenv_objfact_fab failed to create intenv item for object %s", obj->name_str); return -1; }
  intenv->obj = obj;
  obj->itemptr = intenv;

  /* 
   * Metric Creation 
   */

  /* Temperature */
  intenv->temp = l_snmp_metric_create (self, obj, "temp", "Temperature", METRIC_GAUGE, "enterprises.318.1.1.10.2.3.2.1.4", index_oidstr, RECMETHOD_RRD, 0);
  intenv->temp->record_defaultflag = 1;

  /* Units */
  intenv->units = l_snmp_metric_create (self, obj, "units", "Units", METRIC_INTEGER, "enterprises.318.1.1.10.2.3.2.1.5", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (intenv->units, 1, "deg.C");
  i_metric_enumstr_add (intenv->units, 2, "deg.F");

  /* Enqueue the intenv item */
  num = i_list_enqueue (cnt->item_list, intenv);
  if (num != 0)
  { i_printf (1, "v_intenv_objfact_fab failed to enqueue intenv for object %s", obj->name_str); v_intenv_item_free (intenv); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_intenv_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_intenv_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the intenv and remove
   * it from the item_list
   */

  int num;
  v_intenv_item *intenv = obj->itemptr;

  if (!intenv) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, intenv);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
