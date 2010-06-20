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

#include "ctrl.h"

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_ctrl_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_ctrl_item *ctrl;

  /* Object Configuration */
  char *desc_index_str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Controller %s", desc_index_str);
  free (desc_index_str);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_ctrl_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create ctrl item struct */
  ctrl = v_ctrl_item_create ();
  if (!ctrl)
  { i_printf (1, "v_ctrl_objfact_fab failed to create ctrl item for object %s", obj->name_str); return -1; }
  ctrl->obj = obj;
  obj->itemptr = ctrl;
  ctrl->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Alias */
  ctrl->alias = l_snmp_metric_create (self, obj, "alias", "Alias", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.2", index_oidstr, RECMETHOD_NONE, 0);

  /* WWN */
  ctrl->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.3", index_oidstr, RECMETHOD_NONE, 0);

  /* Part */
  ctrl->part = l_snmp_metric_create (self, obj, "part", "Part Number", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.6", index_oidstr, RECMETHOD_NONE, 0);

  /* Serial */
  ctrl->serial = l_snmp_metric_create (self, obj, "serial", "Serial", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.7", index_oidstr, RECMETHOD_NONE, 0);

  /* Revision */
  ctrl->revision = l_snmp_metric_create (self, obj, "revision", "Revision", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.8", index_oidstr, RECMETHOD_NONE, 0);

  /* Software */
  ctrl->software = l_snmp_metric_create (self, obj, "software", "Software", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.14", index_oidstr, RECMETHOD_NONE, 0);

  /* Firmware */
  ctrl->firmware = l_snmp_metric_create (self, obj, "firmware", "Firmware", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.13", index_oidstr, RECMETHOD_NONE, 0);

  /* Role */
  ctrl->role = l_snmp_metric_create (self, obj, "role", "Role", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.1.3.1.16", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (ctrl->role, 1, "Primary");
  i_metric_enumstr_add (ctrl->role, 0, "Secondary");

  /* Readiness */
  ctrl->readiness = l_snmp_metric_create (self, obj, "readiness", "Readiness", METRIC_STRING, ".1.3.6.1.4.1.7933.1.20.1.3.1.17", index_oidstr, RECMETHOD_NONE, 0);

  /* Array Count */
  ctrl->arraycount = l_snmp_metric_create (self, obj, "arraycount", "Array Count", METRIC_INTEGER, ".1.3.6.1.4.1.7933.1.20.1.3.1.18", index_oidstr, RECMETHOD_NONE, 0);

  /* Physical Drives */
  ctrl->phydrvcount = l_snmp_metric_create (self, obj, "phydrvcount", "Physical Drives Present", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.19", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->phydrvonline = l_snmp_metric_create (self, obj, "phydrvonline", "Physical Drives Online", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.20", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->phydrvoffline = l_snmp_metric_create (self, obj, "phydrvoffline", "Physical Drives Offline", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.21", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->phydrvfpa = l_snmp_metric_create (self, obj, "phydrvfpa", "Physical Drives with FPA", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.22", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->phydrvrebuild = l_snmp_metric_create (self, obj, "phydrvrebuild", "Physical Drives Rebuilding", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.23", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->phydrvmissing = l_snmp_metric_create (self, obj, "phydrvmissing", "Physical Drives Missing", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.24", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->phydrvunconfig = l_snmp_metric_create (self, obj, "phydrvunconfig", "Physical Drives Unconfigured", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.25", index_oidstr, RECMETHOD_NONE, 0);

  /* Logical Drives */
  ctrl->logdrvcount = l_snmp_metric_create (self, obj, "logdrvcount", "Logical Drives Present", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.26", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->logdrvonline = l_snmp_metric_create (self, obj, "logdrvonline", "Logical Drives Online", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.27", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->logdrvoffline = l_snmp_metric_create (self, obj, "logdrvoffline", "Logical Drives Offline", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.28", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->logdrvcritical = l_snmp_metric_create (self, obj, "logdrvcritical", "Logical Drives Critical", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.29", index_oidstr, RECMETHOD_NONE, 0);

  /* Spares */
  ctrl->globalspares = l_snmp_metric_create (self, obj, "globalspares", "Global Spares", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.30", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->dedspares = l_snmp_metric_create (self, obj, "dedspares", "Dedicated Spares", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.31", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->rvtglobalspares = l_snmp_metric_create (self, obj, "rvtglobalspares", "Revertible Global Spares", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.32", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->rvtglobalspares_used = l_snmp_metric_create (self, obj, "rvtglobalspares_used", "Revertible Global Spares Used", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.33", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->rvtdedspares = l_snmp_metric_create (self, obj, "rvtdedspares", "Revrtible Dedicated Spares", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.34", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->rvtdedspares_used = l_snmp_metric_create (self, obj, "rvtdedspares_used", "Revrtible Dedicated Spares Used", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.35", index_oidstr, RECMETHOD_NONE, 0);

  /* BGA */
  ctrl->bgacount = l_snmp_metric_create (self, obj, "bgacount", "BGA Count", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.36", index_oidstr, RECMETHOD_NONE, 0);

  /* Cache */
  ctrl->dirtycache = l_snmp_metric_create (self, obj, "dirtycache", "Dirty Cache", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.38", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->dirtycache->unit_str = strdup ("%");
  ctrl->cacheused = l_snmp_metric_create (self, obj, "cacheused", "Cache Used", METRIC_GAUGE, ".1.3.6.1.4.1.7933.1.20.1.3.1.39", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->cacheused->unit_str = strdup ("%");

  /* Errors */
  ctrl->errors = l_snmp_metric_create (self, obj, "errors", "Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.1.4.1.4", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->eps = i_metric_acrate_create (self, obj, "eps", "Errors Rate", "err/sec", RECMETHOD_RRD, ctrl->errors, 0);
  ctrl->eps->record_defaultflag = 1;
  ctrl->nonrw_errors = l_snmp_metric_create (self, obj, "nonrw_errors", "Non-RW Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.1.4.1.5", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->read_errors = l_snmp_metric_create (self, obj, "read_errors", "Read Errors", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.1.4.1.6", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->write_errors = l_snmp_metric_create (self, obj, "write_errors", "Write Error", METRIC_COUNT, ".1.3.6.1.4.1.7933.1.20.1.4.1.7", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the ctrl item */
  num = i_list_enqueue (cnt->item_list, ctrl);
  if (num != 0)
  { i_printf (1, "v_ctrl_objfact_fab failed to enqueue ctrl for object %s", obj->name_str); v_ctrl_item_free (ctrl); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_ctrl_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_ctrl_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the ctrl and remove
   * it from the item_list
   */

  int num;
  v_ctrl_item *ctrl = obj->itemptr;

  if (!ctrl) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, ctrl);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
