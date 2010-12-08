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

#include "logdrv.h"

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_logdrv_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_logdrv_item *logdrv;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_logdrv_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create logdrv item struct */
  logdrv = v_logdrv_item_create ();
  if (!logdrv)
  { i_printf (1, "v_logdrv_objfact_fab failed to create logdrv item for object %s", obj->name_str); return -1; }
  logdrv->obj = obj;
  obj->itemptr = logdrv;
  logdrv->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Drive Info */
  logdrv->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.5.1.4", index_oidstr, RECMETHOD_NONE, 0);

  /* State */
  logdrv->opstate = l_snmp_metric_create (self, obj, "opstate", "Operational Status", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.5.1.5", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->condition = l_snmp_metric_create (self, obj, "condition", "Condition", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.5.1.6", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->operation = l_snmp_metric_create (self, obj, "operation", "Operation", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.2.5.1.7", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->synchronized = l_snmp_metric_create (self, obj, "synchronized", "Synchronized", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.5.1.8", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (logdrv->synchronized, 1, "Yes");
  i_metric_enumstr_add (logdrv->synchronized, 0, "No");

  /* Config */
  logdrv->raidlevel = l_snmp_metric_create (self, obj, "raidlevel", "RAID Level", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.5.1.9", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (logdrv->raidlevel, 0, "RAID-0");
  i_metric_enumstr_add (logdrv->raidlevel, 1, "RAID-1");
  i_metric_enumstr_add (logdrv->raidlevel, 3, "RAID-3");
  i_metric_enumstr_add (logdrv->raidlevel, 5, "RAID-5");
  i_metric_enumstr_add (logdrv->raidlevel, 6, "RAID-6");
  i_metric_enumstr_add (logdrv->raidlevel, 10, "JBOD");
  i_metric_enumstr_add (logdrv->raidlevel, 80, "RAID-50");
  i_metric_enumstr_add (logdrv->raidlevel, 81, "RAID-51");
  i_metric_enumstr_add (logdrv->raidlevel, 85, "RAID-55");
  i_metric_enumstr_add (logdrv->raidlevel, 95, "RAID-1E");
  i_metric_enumstr_add (logdrv->raidlevel, 96, "RAID-60");
  logdrv->arrayid = l_snmp_metric_create (self, obj, "arrayid", "Array ID", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.2.5.1.12", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->axelcount = l_snmp_metric_create (self, obj, "axelcount", "Axel Count", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.2.5.1.13", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->usedpd = l_snmp_metric_create (self, obj, "usedpd", "Used PD", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.2.5.1.14", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->sectorsize = l_snmp_metric_create (self, obj, "sectorsize", "Sector Size", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.2.5.1.15", index_oidstr, RECMETHOD_NONE, 0);

  /* Errors */
  logdrv->errors = l_snmp_metric_create (self, obj, "errors", "Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.8.1.4", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->eps = i_metric_acrate_create (self, obj, "eps", "Errors Rate", "err/sec", RECMETHOD_RRD, logdrv->errors, 0);
  logdrv->eps->record_defaultflag = 1;
  logdrv->nonrw_errors = l_snmp_metric_create (self, obj, "nonrw_errors", "Non-RW Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.8.1.5", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->read_errors = l_snmp_metric_create (self, obj, "read_errors", "Read Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.8.1.6", index_oidstr, RECMETHOD_NONE, 0);
  logdrv->write_errors = l_snmp_metric_create (self, obj, "write_errors", "Write Error", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.2.8.1.7", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the logdrv item */
  num = i_list_enqueue (cnt->item_list, logdrv);
  if (num != 0)
  { i_printf (1, "v_logdrv_objfact_fab failed to enqueue logdrv for object %s", obj->name_str); v_logdrv_item_free (logdrv); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_logdrv_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_logdrv_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the logdrv and remove
   * it from the item_list
   */

  int num;
  v_logdrv_item *logdrv = obj->itemptr;

  if (!logdrv) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, logdrv);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
