#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"

#include "device/snmp.h"

#include "fibreport.h"

/* 
 * fibreport Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_fibreport_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_fibreport_item *fibreport;

  /* Object setup */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_fibreport_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create fibreport item struct */
  fibreport = v_fibreport_item_create ();
  fibreport->obj = obj;
  obj->itemptr = fibreport;
  fibreport->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Loop ID */
  fibreport->loopid = l_snmp_metric_create (self, obj, "loopid", "Loop ID", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.6.1.1.2", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fibreport->loopid, 255, "Auto");

  /* Mode */
  fibreport->mode = l_snmp_metric_create (self, obj, "mode", "Mode", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.6.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fibreport->mode, 0, "Unknown");
  i_metric_enumstr_add (fibreport->mode, 1, "Fabric");
  i_metric_enumstr_add (fibreport->mode, 2, "Peer-to-Peer");
  i_metric_enumstr_add (fibreport->mode, 3, "Private Loop");
  i_metric_enumstr_add (fibreport->mode, 4, "Public Loop");
  i_metric_enumstr_add (fibreport->mode, 255, "Auto");

  /* Rate */
  fibreport->rate = l_snmp_metric_create (self, obj, "rate", "Rate", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.6.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (fibreport->rate, 0, "Unknown");
  i_metric_enumstr_add (fibreport->rate, 1, "1Gbps");
  i_metric_enumstr_add (fibreport->rate, 2, "2Gbps");
  i_metric_enumstr_add (fibreport->rate, 4, "4Gbps");
  i_metric_enumstr_add (fibreport->rate, 255, "Auto");

  /* WWN */
  fibreport->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_HEXSTRING, ".1.3.6.1.4.1.31165.1.1.6.1.1.5", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the fibreport item */
  num = i_list_enqueue (cnt->item_list, fibreport);
  if (num != 0)
  { i_printf (1, "v_fibreport_objfact_fab failed to enqueue fibreport for object %s", obj->name_str); v_fibreport_item_free (fibreport); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_fibreport_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_fibreport_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the fibreport and remove
   * it from the item_list
   */

  int num;
  v_fibreport_item *fibreport = obj->itemptr;

  if (!fibreport) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, fibreport);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
