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

#include "drive.h"

/* 
 * drive Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_drive_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_drive_item *drive;

  /* Object setup */
  char *str = l_snmp_get_string_from_pdu (pdu);
  asprintf (&obj->desc_str, "Slot %s", str);
  free (str);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_drive_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create drive item struct */
  drive = v_drive_item_create ();
  drive->obj = obj;
  obj->itemptr = drive;
  drive->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  drive->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.7.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (drive->status, 0, "Online");
  i_metric_enumstr_add (drive->status, 1, "Foreign");
  i_metric_enumstr_add (drive->status, 2, "Conflict");
  i_metric_enumstr_add (drive->status, 3, "Remove");
  i_metric_enumstr_add (drive->status, 4, "Perm. Removed");
  i_metric_enumstr_add (drive->status, 5, "Faulty");
  i_metric_enumstr_add (drive->status, 6, "Initializing");
  i_metric_enumstr_add (drive->status, 7, "Unknown");
  i_metric_enumstr_add (drive->status, 8, "Empty");

  /* Speed */
  drive->firmware = l_snmp_metric_create (self, obj, "firmware", "Firmware", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.7.1.1.9", index_oidstr, RECMETHOD_NONE, 0);

  /* Available Size */
  drive->availsize = l_snmp_metric_create (self, obj, "availsize", "Available Size", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.7.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  drive->availsize->summary_flag = 1;
  drive->availsize->alloc_unit = (1024 * 1024);
  drive->availsize->unit_str = strdup ("MB");

  /* Total Size */
  drive->totalsize = l_snmp_metric_create (self, obj, "totalsize", "Total Size", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.7.1.1.7", index_oidstr, RECMETHOD_NONE, 0);
  drive->totalsize->unit_str = strdup ("MB");
  drive->totalsize->summary_flag = 1;

  /* Usage */
  drive->usage = l_snmp_metric_create (self, obj, "usage", "Usage", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.7.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  drive->usage->summary_flag = 1;
  i_metric_enumstr_add (drive->usage, -1, "None");
  i_metric_enumstr_add (drive->usage, 0, "Unused");
  i_metric_enumstr_add (drive->usage, 1, "JBOD");
  i_metric_enumstr_add (drive->usage, 3, "RAID Member");
  i_metric_enumstr_add (drive->usage, 4, "Local Spare");
  i_metric_enumstr_add (drive->usage, 5, "Global Spare");
  i_metric_enumstr_add (drive->usage, 6, "Clone");
  i_metric_enumstr_add (drive->usage, 65536, "Invalid");

  /* Target */
  drive->targettype = l_snmp_metric_create (self, obj, "targettype", "Target Type", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.7.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  drive->targettype->summary_flag = 1;
  i_metric_enumstr_add (drive->targettype, -2, "Unused");
  i_metric_enumstr_add (drive->targettype, 1, "JBOD");
  i_metric_enumstr_add (drive->targettype, 2, "RAID Set");

  /* Speed */
  drive->speed = l_snmp_metric_create (self, obj, "speed", "Speed", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.7.1.1.8", index_oidstr, RECMETHOD_NONE, 0);
  drive->speed->summary_flag = 1;

  /* Serial */
  drive->serial = l_snmp_metric_create (self, obj, "serial", "Serial", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.7.1.1.10", index_oidstr, RECMETHOD_NONE, 0);
  drive->serial->summary_flag = 1;

  /* STandard */
  drive->standard = l_snmp_metric_create (self, obj, "standard", "standard", METRIC_STRING, ".1.3.6.1.4.1.31165.1.1.7.1.1.11", index_oidstr, RECMETHOD_NONE, 0);

  /* Enqueue the drive item */
  num = i_list_enqueue (cnt->item_list, drive);
  if (num != 0)
  { i_printf (1, "v_drive_objfact_fab failed to enqueue drive for object %s", obj->name_str); v_drive_item_free (drive); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_drive_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_drive_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the drive and remove
   * it from the item_list
   */

  int num;
  v_drive_item *drive = obj->itemptr;

  if (!drive) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, drive);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
