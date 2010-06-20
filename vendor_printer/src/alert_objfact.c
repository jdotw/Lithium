#include <stdlib.h>

#include <induction.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <induction/hierarchy.h>
#include <induction/list.h>

#include <lithium/snmp.h>

#include "alert.h"

/* 
 * EMS Probes - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_alert_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_alert_item *alert;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_alert_objform;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_alert_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create alert item struct */
  alert = v_alert_item_create ();
  if (!alert)
  { i_printf (1, "v_alert_objfact_fab failed to create alert item for object %s", obj->name_str); return -1; }
  alert->obj = obj;
  obj->itemptr = alert;
  alert->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Severity */
  alert->severity = l_snmp_metric_create (self, obj, "severity", "Severity", METRIC_INTEGER, ".1.3.6.1.2.1.43.18.1.1.2.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (alert->severity, 1, "Other");
  i_metric_enumstr_add (alert->severity, 3, "Critical (Binary)");
  i_metric_enumstr_add (alert->severity, 4, "Warning (Unary)");
  i_metric_enumstr_add (alert->severity, 5, "Warning (Unary)");

  /* Group */
  alert->group = l_snmp_metric_create (self, obj, "group", "Group", METRIC_INTEGER, ".1.3.6.1.2.1.43.18.1.1.4.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (alert->group, 1, "Other");
  i_metric_enumstr_add (alert->group, 3, "Storage Resource");
  i_metric_enumstr_add (alert->group, 4, "Device");
  i_metric_enumstr_add (alert->group, 5, "Printer");
  i_metric_enumstr_add (alert->group, 6, "Cover");
  i_metric_enumstr_add (alert->group, 7, "Localization");
  i_metric_enumstr_add (alert->group, 8, "Input");
  i_metric_enumstr_add (alert->group, 9, "Output");
  i_metric_enumstr_add (alert->group, 10, "Marker");
  i_metric_enumstr_add (alert->group, 11, "Marker Supplies");
  i_metric_enumstr_add (alert->group, 12, "Marker Colorant");
  i_metric_enumstr_add (alert->group, 13, "Media Path");
  i_metric_enumstr_add (alert->group, 14, "Channel");
  i_metric_enumstr_add (alert->group, 15, "Interpreter");
  i_metric_enumstr_add (alert->group, 16, "Console Display");
  i_metric_enumstr_add (alert->group, 17, "Console Lights");
  i_metric_enumstr_add (alert->group, 18, "Alert");
  i_metric_enumstr_add (alert->group, 30, "Finisher Device");
  i_metric_enumstr_add (alert->group, 31, "Finisher Supply");
  i_metric_enumstr_add (alert->group, 32, "Finisher Input");
  i_metric_enumstr_add (alert->group, 33, "Finisher Attribute");

  /* Code */
  alert->code = l_snmp_metric_create (self, obj, "code", "Code", METRIC_INTEGER, ".1.3.6.1.2.1.43.18.1.1.7.1", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (alert->code, 1, "Other");
  i_metric_enumstr_add (alert->code, 2, "Unknown");
  i_metric_enumstr_add (alert->code, 3, "Cover Open");
  i_metric_enumstr_add (alert->code, 4, "Cover Closed");
  i_metric_enumstr_add (alert->code, 5, "Interlock Open");
  i_metric_enumstr_add (alert->code, 6, "Interlock Closed");
  i_metric_enumstr_add (alert->code, 7, "Config Changed");
  i_metric_enumstr_add (alert->code, 8, "Jammed");
  i_metric_enumstr_add (alert->code, 9, "Subunit Missing");
  i_metric_enumstr_add (alert->code, 10, "Subunit Life Almost Over");
  i_metric_enumstr_add (alert->code, 11, "Subunit Life Over");
  i_metric_enumstr_add (alert->code, 12, "Subunit Life Almost Empty");
  i_metric_enumstr_add (alert->code, 13, "Subunit Life Empty");
  i_metric_enumstr_add (alert->code, 14, "Subunit Almost Full");
  i_metric_enumstr_add (alert->code, 15, "Subunit Full");
  i_metric_enumstr_add (alert->code, 16, "Subunit Near Limit");
  i_metric_enumstr_add (alert->code, 17, "Subunit At Limit");
  i_metric_enumstr_add (alert->code, 18, "Subunit Open");
  i_metric_enumstr_add (alert->code, 19, "Subunit Close");
  i_metric_enumstr_add (alert->code, 20, "Subunit Turned Off");
  i_metric_enumstr_add (alert->code, 21, "Subunit Turned On");
  i_metric_enumstr_add (alert->code, 22, "Subunit Offline");
  i_metric_enumstr_add (alert->code, 23, "Subunit Power Save");
  i_metric_enumstr_add (alert->code, 24, "Subunit Warming Up");
  i_metric_enumstr_add (alert->code, 25, "Subunit Added");
  i_metric_enumstr_add (alert->code, 26, "Subunit Removed");
  i_metric_enumstr_add (alert->code, 27, "Subunit Resource Added");
  i_metric_enumstr_add (alert->code, 28, "Subunit Resource Removed");
  i_metric_enumstr_add (alert->code, 29, "Subunit Recoverable Failure");
  i_metric_enumstr_add (alert->code, 30, "Subunit Unrecoverable Failure");
  i_metric_enumstr_add (alert->code, 31, "Subunit Recoverable Storage Error");
  i_metric_enumstr_add (alert->code, 32, "Subunit Unrecoverable Storage Error");
  i_metric_enumstr_add (alert->code, 33, "Subunit Motor Failure");
  i_metric_enumstr_add (alert->code, 34, "Subunit Memory Exhausted");
  i_metric_enumstr_add (alert->code, 35, "Subunit Under Temperature");
  i_metric_enumstr_add (alert->code, 36, "Subunit Over Temperature");
  i_metric_enumstr_add (alert->code, 37, "Subunit Timing Failure");
  i_metric_enumstr_add (alert->code, 38, "Subunit Thermistor Failure");
  i_metric_enumstr_add (alert->code, 501, "Door Open");
  i_metric_enumstr_add (alert->code, 502, "Door Closed");
  i_metric_enumstr_add (alert->code, 503, "Powered Up");
  i_metric_enumstr_add (alert->code, 504, "Powered Down");
  i_metric_enumstr_add (alert->code, 505, "NMS Reset");
  i_metric_enumstr_add (alert->code, 506, "Manual Reset");
  i_metric_enumstr_add (alert->code, 507, "Ready to print");
  i_metric_enumstr_add (alert->code, 801, "Input Media Tray Missing");
  i_metric_enumstr_add (alert->code, 802, "Input Media Size Changed");
  i_metric_enumstr_add (alert->code, 803, "Input Media Weight Changed");
  i_metric_enumstr_add (alert->code, 804, "Input Media Type Changed");
  i_metric_enumstr_add (alert->code, 805, "Input Media Color Changed");
  i_metric_enumstr_add (alert->code, 806, "Input Media Form Parts Changed");
  i_metric_enumstr_add (alert->code, 807, "Input Media Supply Low");
  i_metric_enumstr_add (alert->code, 808, "Input Media Supply Empty");
  i_metric_enumstr_add (alert->code, 809, "Input Media Change Request");
  i_metric_enumstr_add (alert->code, 810, "Input Media Input Request");
  i_metric_enumstr_add (alert->code, 811, "Input Media Failed to Position");
  i_metric_enumstr_add (alert->code, 812, "Input Media Tray Elevation Failure");
  i_metric_enumstr_add (alert->code, 813, "Input Media Can Not Feed Size Selected");
  i_metric_enumstr_add (alert->code, 901, "Output Media Tray Missing");
  i_metric_enumstr_add (alert->code, 902, "Output Media Tray Almost Full");
  i_metric_enumstr_add (alert->code, 903, "Output Media Tray Full");
  i_metric_enumstr_add (alert->code, 904, "Output Media Mailbox Select Failure");
  i_metric_enumstr_add (alert->code, 1001, "Marker Fuser Under Temperature");
  i_metric_enumstr_add (alert->code, 1002, "Marker Fuser Over Temperature");
  i_metric_enumstr_add (alert->code, 1003, "Marker Fuser Timing Temperature");
  i_metric_enumstr_add (alert->code, 1004, "Marker Fuser Thermistor Temperature");
  i_metric_enumstr_add (alert->code, 1005, "Marker Fuser Adjusting Print Quality Temperature");
  i_metric_enumstr_add (alert->code, 1101, "Marker Toner Empty");
  i_metric_enumstr_add (alert->code, 1102, "Marker Ink Empty");
  i_metric_enumstr_add (alert->code, 1103, "Marker Print Ribbon Empty");
  i_metric_enumstr_add (alert->code, 1104, "Marker Toner Almost Empty");
  i_metric_enumstr_add (alert->code, 1105, "Marker Ink Almost Empty");
  i_metric_enumstr_add (alert->code, 1106, "Marker Print Ribbon Almost Empty");
  i_metric_enumstr_add (alert->code, 1107, "Marker Waste Toner Receptacle Almost Full");
  i_metric_enumstr_add (alert->code, 1108, "Marker Waste Ink Receptacle Almost Full");
  i_metric_enumstr_add (alert->code, 1109, "Marker Waste Toner Receptacle Full");
  i_metric_enumstr_add (alert->code, 1110, "Marker Waste Ink Receptacle Full");
  i_metric_enumstr_add (alert->code, 1111, "Marker Waste OPS Life Almost Over");
  i_metric_enumstr_add (alert->code, 1112, "Marker Waste OPS Life Over");
  i_metric_enumstr_add (alert->code, 1113, "Marker Developer Almost Empty");
  i_metric_enumstr_add (alert->code, 1114, "Marker Developer Empty");
  i_metric_enumstr_add (alert->code, 1115, "Marker Toner Cartridge Missing");
  i_metric_enumstr_add (alert->code, 1301, "Media Path Tray Missing");
  i_metric_enumstr_add (alert->code, 1302, "Media Path Tray Almost Full");
  i_metric_enumstr_add (alert->code, 1303, "Media Path Tray Full");
  i_metric_enumstr_add (alert->code, 1501, "Interpreter Memory Increased");
  i_metric_enumstr_add (alert->code, 1502, "Interpreter Memory Decreased");
  i_metric_enumstr_add (alert->code, 1503, "Interpreter Cartridge Added");
  i_metric_enumstr_add (alert->code, 1504, "Interpreter Cartridge Deleted");
  i_metric_enumstr_add (alert->code, 1505, "Interpreter Resource Added");
  i_metric_enumstr_add (alert->code, 1506, "Interpreter Resource Deleted");
  i_metric_enumstr_add (alert->code, 1507, "Interpreter Resource Unavailable");
  i_metric_enumstr_add (alert->code, 1509, "Interpreter Complex Page Encountered");
  i_metric_enumstr_add (alert->code, 1801, "Removal of Binary Change Entry");
  
  /* Enqueue the alert item */
  num = i_list_enqueue (cnt->item_list, alert);
  if (num != 0)
  { i_printf (1, "v_alert_objfact_fab failed to enqueue alert for object %s", obj->name_str); v_alert_item_free (alert); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_alert_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_alert_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the alert and remove
   * it from the item_list
   */

  int num;
  v_alert_item *alert = obj->itemptr;

  if (!alert) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, alert);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
