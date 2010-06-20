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
 * ctrl Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_ctrl_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_ctrl_item *ctrl;

  /* Object setup */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_ctrl_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create ctrl item struct */
  ctrl = v_ctrl_item_create ();
  ctrl->obj = obj;
  obj->itemptr = ctrl;
  ctrl->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  ctrl->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.11.1.1.14", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (ctrl->status, 0, "Unknown");
  i_metric_enumstr_add (ctrl->status, 1, "Booting");
  i_metric_enumstr_add (ctrl->status, 2, "OK");
  i_metric_enumstr_add (ctrl->status, 3, "Failed");
  i_metric_enumstr_add (ctrl->status, 4, "Failover");
  i_metric_enumstr_add (ctrl->status, 5, "Fallback");

  /* Role */
  ctrl->role = l_snmp_metric_create (self, obj, "role", "Role", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.11.1.1.13", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->role->summary_flag = 1;
  i_metric_enumstr_add (ctrl->role, 0, "Unknown");
  i_metric_enumstr_add (ctrl->role, 1, "Primary");
  i_metric_enumstr_add (ctrl->role, 2, "Secondary");

  /* RAM */
  ctrl->ramsize = l_snmp_metric_create (self, obj, "ramsize", "RAM Size", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.11.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->ramsize->summary_flag = 1;
  ctrl->ramsize->unit_str = strdup ("MB");
  ctrl->ramtype = l_snmp_metric_create (self, obj, "ramtype", "RAM Type", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.11.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (ctrl->ramtype, 0, "Non-ECC");
  i_metric_enumstr_add (ctrl->ramtype, 1, "ECC");

  /* Serial, Model, Product */
  ctrl->serial = l_snmp_metric_create (self, obj, "serial", "Serial", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.11.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->model = l_snmp_metric_create (self, obj, "model", "Model", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.11.1.1.7", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->vendor = l_snmp_metric_create (self, obj, "vendor", "Vendor", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.11.1.1.11", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->product = l_snmp_metric_create (self, obj, "product", "Product", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.11.1.1.12", index_oidstr, RECMETHOD_NONE, 0);

  /* Versions */
  ctrl->firmware = l_snmp_metric_create (self, obj, "firmware", "Firmware", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.11.1.1.8", index_oidstr, RECMETHOD_NONE, 0);
  ctrl->firmware->summary_flag = 1;
  ctrl->bootcode = l_snmp_metric_create (self, obj, "bootcode", "Bootcode", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.11.1.1.9", index_oidstr, RECMETHOD_NONE, 0);

  /* Iface type */
  ctrl->ifacetype = l_snmp_metric_create (self, obj, "ifacetype", "Interface Type", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.11.1.1.10", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (ctrl->ifacetype, 0, "SCSI");
  i_metric_enumstr_add (ctrl->ifacetype, 1, "SATA");
  i_metric_enumstr_add (ctrl->ifacetype, 2, "SAS");
  i_metric_enumstr_add (ctrl->ifacetype, 3, "Fibre");

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
