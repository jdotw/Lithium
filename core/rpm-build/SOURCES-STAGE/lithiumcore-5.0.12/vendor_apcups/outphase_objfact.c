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

#include "outphase.h"

/* 
 * UPS Output Phase - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_outphase_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_outphase_item *outphase;

	/* Check that the pdu size is OK */
	if (!pdu || !pdu->variables || pdu->variables->name_length < 2) return -1;

  /* Object Configuration */
  asprintf (&obj->desc_str, "Output Phase %lu.%lu", pdu->variables->name[pdu->variables->name_length-2], pdu->variables->name[pdu->variables->name_length-1]);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_outphase_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create outphase item struct */
  outphase = v_outphase_item_create ();
  if (!outphase)
  { i_printf (1, "v_outphase_objfact_fab failed to create outphase item for object %s", obj->name_str); return -1; }
  outphase->obj = obj;
  obj->itemptr = outphase;

  /* 
   * Metric Creation 
   */

  outphase->voltage = l_snmp_metric_create (self, obj, "voltage", "Voltage", METRIC_GAUGE, "enterprises.318.1.1.1.9.3.3.1.3.1.1", index_oidstr, RECMETHOD_RRD, 0);
  outphase->voltage->record_defaultflag = 1;
  outphase->voltage->unit_str = strdup ("VAC");

  outphase->current = l_snmp_metric_create (self, obj, "current", "Current", METRIC_GAUGE, "enterprises.318.1.1.1.9.3.3.1.4.1.1", index_oidstr, RECMETHOD_RRD, 0);
  outphase->current->record_defaultflag = 1;
  outphase->current->unit_str = strdup ("amp");
  outphase->current->div_by = 10;

  outphase->load_pc = l_snmp_metric_create (self, obj, "load_pc", "Load Percent", METRIC_GAUGE, "enterprises.318.1.1.1.9.3.3.1.10.1.1", index_oidstr, RECMETHOD_RRD, 0);
  outphase->load_pc->record_defaultflag = 1;
  outphase->load_pc->unit_str = strdup ("%");

  outphase->load = l_snmp_metric_create (self, obj, "load", "Load", METRIC_GAUGE, "enterprises.318.1.1.1.9.3.3.1.7.1.1", index_oidstr, RECMETHOD_RRD, 0);
  outphase->load->record_defaultflag = 1;
  outphase->load->unit_str = strdup ("VA");

  outphase->power_pc = l_snmp_metric_create (self, obj, "power_pc", "Power Percent", METRIC_GAUGE, "enterprises.318.1.1.1.9.3.3.1.16.1.1", index_oidstr, RECMETHOD_RRD, 0);
  outphase->power_pc->record_defaultflag = 1;
  outphase->power_pc->unit_str = strdup ("%");

  outphase->power = l_snmp_metric_create (self, obj, "power", "Power", METRIC_GAUGE, "enterprises.318.1.1.1.9.3.3.1.13.1.1", index_oidstr, RECMETHOD_RRD, 0);
  outphase->power->record_defaultflag = 1;
  outphase->power->unit_str = strdup ("W");

  /* Enqueue the outphase item */
  num = i_list_enqueue (cnt->item_list, outphase);
  if (num != 0)
  { i_printf (1, "v_outphase_objfact_fab failed to enqueue outphase for object %s", obj->name_str); v_outphase_item_free (outphase); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_outphase_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_outphase_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the outphase and remove
   * it from the item_list
   */

  int num;
  v_outphase_item *outphase = obj->itemptr;

  if (!outphase) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, outphase);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
