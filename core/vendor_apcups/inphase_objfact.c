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

#include "inphase.h"

/* 
 * UPS Output Phase - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_inphase_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_inphase_item *inphase;

	/* Check that the pdu size is OK */
	if (!pdu || !pdu->variables || pdu->variables->name_length < 2) return -1;

  /* Object Configuration */
  asprintf (&obj->desc_str, "Input Phase %lu.%lu", pdu->variables->name[pdu->variables->name_length-2], pdu->variables->name[pdu->variables->name_length-1]);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_inphase_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create inphase item struct */
  inphase = v_inphase_item_create ();
  if (!inphase)
  { i_printf (1, "v_inphase_objfact_fab failed to create inphase item for object %s", obj->name_str); return -1; }
  inphase->obj = obj;
  obj->itemptr = inphase;

  /* 
   * Metric Creation 
   */

  inphase->voltage = l_snmp_metric_create (self, obj, "voltage", "Voltage", METRIC_GAUGE, "enterprises.318.1.1.1.9.2.3.1.3.1.1", index_oidstr, RECMETHOD_RRD, 0);
  inphase->voltage->record_defaultflag = 1;
  inphase->voltage->unit_str = strdup ("VAC");

  /* Enqueue the inphase item */
  num = i_list_enqueue (cnt->item_list, inphase);
  if (num != 0)
  { i_printf (1, "v_inphase_objfact_fab failed to enqueue inphase for object %s", obj->name_str); v_inphase_item_free (inphase); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_inphase_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_inphase_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the inphase and remove
   * it from the item_list
   */

  int num;
  v_inphase_item *inphase = obj->itemptr;

  if (!inphase) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, inphase);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
