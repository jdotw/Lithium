#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>

#include "snmp.h"
#include "xsnmp_raiddrive.h"

/* 
 * Xsnmp RAID Drives
 */

/* Object Factory Fabrication */

int l_xsnmp_raiddrive_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_xsnmp_raiddrive_item *drive;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_xsnmp_raiddrive_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create hrcpu item struct */
  drive = l_xsnmp_raiddrive_item_create ();
  if (!drive)
  { i_printf (1, "l_xsnmp_raiddrive_objfact_fab failed to create hrcpu item for object %s", obj->name_str); return -1; }
  obj->itemptr = drive;
  drive->obj = obj;
  drive->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  drive->raidset = l_snmp_metric_create (self, obj, "raidset", "RAID Set", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.3.1.3", index_oidstr, RECMETHOD_NONE, 0);
  drive->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.6.3.1.4", index_oidstr, RECMETHOD_NONE, 0);
  drive->size->alloc_unit = (1024 * 1024);
  drive->size->unit_str = strdup("byte");
  drive->status = l_snmp_metric_create (self, obj, "status", "status", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.6.3.1.5", index_oidstr, RECMETHOD_NONE, 0);
  drive->status_message = l_snmp_metric_create (self, obj, "status_message", "Status Message", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.6.3.1.6", index_oidstr, RECMETHOD_NONE, 0);
  
  /*
   * End Metric Creation
   */

  /* Enqueue the hrcpu item */
  num = i_list_enqueue (cnt->item_list, drive);
  if (num != 0)
  { i_printf (1, "l_xsnmp_raiddrive_objfact_fab failed to enqueue hrcpu item for object %s", obj->name_str); l_xsnmp_raiddrive_item_free (drive); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_xsnmp_raiddrive_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_xsnmp_raiddrive_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  l_xsnmp_raiddrive_item *drive = obj->itemptr;

  /* Remove from item list */
  num = i_list_search (cnt->item_list, drive);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
