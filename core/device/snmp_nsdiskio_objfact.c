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

#include "snmp.h"
#include "snmp_nsdiskio.h"

/* 
 * Cisco Memory Pools - Object Factory Functions 
 */

/* Object Factory Fabrication */

int l_snmp_nsdiskio_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  l_snmp_nsdiskio_item *diskio;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  //obj->mainform_func = l_snmp_nsdiskio_objform;
  //obj->histform_func = l_snmp_nsdiskio_objform_hist;
  
  /* Check name */
  if (strstr(obj->desc_str, "ram") || strstr(obj->desc_str, "loop"))
  { return -1; }

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "l_snmp_nsdiskio_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create diskio item struct */
  diskio = l_snmp_nsdiskio_item_create ();
  if (!diskio)
  { i_printf (1, "l_snmp_nsdiskio_objfact_fab failed to create diskio item for object %s", obj->name_str); return -1; }
  diskio->obj = obj;
  obj->itemptr = diskio;
  diskio->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Byte Counts */
  diskio->read_bytes = l_snmp_metric_create (self, obj, "read_bytes", "Bytes Read", METRIC_COUNT, ".1.3.6.1.4.1.2021.13.15.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  diskio->write_bytes = l_snmp_metric_create (self, obj, "write_bytes", "Bytes Written", METRIC_COUNT, ".1.3.6.1.4.1.2021.13.15.1.1.4", index_oidstr, RECMETHOD_NONE, 0);

  /* Byte Throughput */
  diskio->read_tput = i_metric_acrate_create (self, obj, "read_tput", "Read Throughput", "byte/sec", RECMETHOD_RRD, diskio->read_bytes, 0);
  diskio->read_tput->record_defaultflag = 1;
  diskio->write_tput = i_metric_acrate_create (self, obj, "write_tput", "Write Throughput", "byte/sec", RECMETHOD_RRD, diskio->write_bytes, 0);
  diskio->write_tput->record_defaultflag = 1;

  /* Operation Counts */
  diskio->read_count = l_snmp_metric_create (self, obj, "read_count", "Read Operation Count", METRIC_COUNT, ".1.3.6.1.4.1.2021.13.15.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  diskio->write_count = l_snmp_metric_create (self, obj, "write_count", "write Operation Count", METRIC_COUNT, ".1.3.6.1.4.1.2021.13.15.1.1.6", index_oidstr, RECMETHOD_NONE, 0);

  /* Operation Rate */
  diskio->read_rate = i_metric_acrate_create (self, obj, "read_rate", "Read Operations Per Second", "reads/sec", RECMETHOD_RRD, diskio->read_count, 0);
  diskio->read_rate->record_defaultflag = 1;
  diskio->read_rate->record_priority = 2;
  diskio->write_rate = i_metric_acrate_create (self, obj, "write_rate", "Write Operations Per Second", "writes/sec", RECMETHOD_RRD, diskio->write_count, 0);
  diskio->write_rate->record_defaultflag = 1;
  diskio->write_rate->record_priority = 2;
  
  /* Enqueue the diskio item */
  num = i_list_enqueue (cnt->item_list, diskio);
  if (num != 0)
  { i_printf (1, "l_snmp_nsdiskio_objfact_fab failed to enqueue diskio for object %s", obj->name_str); l_snmp_nsdiskio_item_free (diskio); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int l_snmp_nsdiskio_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int l_snmp_nsdiskio_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the diskio and remove
   * it from the item_list
   */

  int num;
  l_snmp_nsdiskio_item *diskio = obj->itemptr;

  if (!diskio) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, diskio);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
