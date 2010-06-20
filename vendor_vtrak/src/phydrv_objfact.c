#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include <lithium/snmp.h>

#include "phydrv.h"

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_phydrv_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_phydrv_item *phydrv;

  /* Object Configuration */
  char *desc_index_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Drive %s", desc_index_str);
  free (desc_index_str);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_phydrv_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create phydrv item struct */
  phydrv = v_phydrv_item_create ();
  if (!phydrv)
  { i_printf (1, "v_phydrv_objfact_fab failed to create phydrv item for object %s", obj->name_str); return -1; }
  phydrv->obj = obj;
  obj->itemptr = phydrv;
  phydrv->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Drive Info */
  phydrv->interface = l_snmp_metric_create (self, obj, "interface", "Interface", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.2", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->alias = l_snmp_metric_create (self, obj, "alias", "Alias", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->model = l_snmp_metric_create (self, obj, "model", "Model", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->serial = l_snmp_metric_create (self, obj, "serial", "Serial Number", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->firmware = l_snmp_metric_create (self, obj, "firmware", "Firmware", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->blocksize = l_snmp_metric_create (self, obj, "blocksize", "Block Size", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.1.1.16", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->drivetype = l_snmp_metric_create (self, obj, "drivetype", "Drive Type", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.20", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (phydrv->drivetype, 1, "SATA");
  i_metric_enumstr_add (phydrv->drivetype, 1, "SAS");

  /* State*/
  phydrv->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational Status", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.8", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->condition = l_snmp_metric_create (self, obj, "condition", "Condition", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.9", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->operation = l_snmp_metric_create (self, obj, "operation", "Operation", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.1.1.10", index_oidstr, RECMETHOD_NONE, 0);

  /* Config */
  phydrv->arrayid = l_snmp_metric_create (self, obj, "arrayid", "Array Index", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.1.1.12", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->seqnum = l_snmp_metric_create (self, obj, "seqnum", "Sequence Number", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.1.1.13", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->enclosure = l_snmp_metric_create (self, obj, "enclosure", "Enclosure", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.1.1.14", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->slot = l_snmp_metric_create (self, obj, "slot", "Slot", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.1.1.15", index_oidstr, RECMETHOD_NONE, 0);

  /* Errors */
  phydrv->errors = l_snmp_metric_create (self, obj, "errors", "Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.7.1.4", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->eps = i_metric_acrate_create (self, obj, "eps", "Errors Rate", "err/sec", RECMETHOD_RRD, phydrv->errors, 0);
  phydrv->eps->record_defaultflag = 1;
  phydrv->nonrw_errors = l_snmp_metric_create (self, obj, "nonrw_errors", "Non-RW Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.7.1.5", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->read_errors = l_snmp_metric_create (self, obj, "read_errors", "Read Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.7.1.6", index_oidstr, RECMETHOD_NONE, 0);
  phydrv->write_errors = l_snmp_metric_create (self, obj, "write_errors", "Write Error", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.7.1.7", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the phydrv item */
  num = i_list_enqueue (cnt->item_list, phydrv);
  if (num != 0)
  { i_printf (1, "v_phydrv_objfact_fab failed to enqueue phydrv for object %s", obj->name_str); v_phydrv_item_free (phydrv); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_phydrv_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_phydrv_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the phydrv and remove
   * it from the item_list
   */

  int num;
  v_phydrv_item *phydrv = obj->itemptr;

  if (!phydrv) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, phydrv);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
