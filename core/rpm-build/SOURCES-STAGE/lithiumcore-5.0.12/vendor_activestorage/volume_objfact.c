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

#include "volume.h"

/* 
 * volume Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_volume_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_volume_item *volume;

  /* Object setup */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_volume_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create volume item struct */
  volume = v_volume_item_create ();
  volume->obj = obj;
  obj->itemptr = volume;
  volume->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* State */
  volume->status = l_snmp_metric_create (self, obj, "status", "Status", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.9.1.1.7", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (volume->status, 0, "Initializing");
  i_metric_enumstr_add (volume->status, 1, "Background Initializing");
  i_metric_enumstr_add (volume->status, 2, "Background Init. Degraded");
  i_metric_enumstr_add (volume->status, 3, "Optimal");
  i_metric_enumstr_add (volume->status, 4, "Check");
  i_metric_enumstr_add (volume->status, 5, "Rebuilding");
  i_metric_enumstr_add (volume->status, 6, "Degraded");
  i_metric_enumstr_add (volume->status, 7, "Reconfiguration");
  i_metric_enumstr_add (volume->status, 8, "Reconfig. Degraded");
  i_metric_enumstr_add (volume->status, 9, "Reconfig. Rebuild");
  i_metric_enumstr_add (volume->status, 10, "Clone");
  i_metric_enumstr_add (volume->status, 11, "Clone Rebuild");
  i_metric_enumstr_add (volume->status, 12, "Faulty");
  i_metric_enumstr_add (volume->status, 13, "Init. Faulty");
  i_metric_enumstr_add (volume->status, 65535, "Invalid");

  /* Size */
  volume->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.9.1.1.6", index_oidstr, RECMETHOD_NONE, 0);
  volume->size->unit_str = strdup ("MB");
  volume->size->summary_flag = 1;

  /* RAID Set */
  volume->raidset = l_snmp_metric_create (self, obj, "raidset", "RAID Set", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.9.1.1.1", index_oidstr, RECMETHOD_NONE, 0);
  volume->raidset->summary_flag = 1;

  /* Level */
  volume->level = l_snmp_metric_create (self, obj, "level", "Level", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.9.1.1.3", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (volume->level, 0, "RAID 0");
  i_metric_enumstr_add (volume->level, 1, "RAID 1");
  i_metric_enumstr_add (volume->level, 3, "RAID 3");
  i_metric_enumstr_add (volume->level, 5, "RAID 5");
  i_metric_enumstr_add (volume->level, 6, "RAID 6");
  i_metric_enumstr_add (volume->level, 7, "RAID TP");
  i_metric_enumstr_add (volume->level, 10, "RAID 10");
  i_metric_enumstr_add (volume->level, 30, "RAID 30");
  i_metric_enumstr_add (volume->level, 50, "RAID 50");
  i_metric_enumstr_add (volume->level, 60, "RAID 60");
  i_metric_enumstr_add (volume->level, 200, "NRAID");
  i_metric_enumstr_add (volume->level, 201, "JBOD");
  i_metric_enumstr_add (volume->level, 255, "None");
  volume->level->summary_flag = 1;

  /* Stripe Size */
  volume->stripesize = l_snmp_metric_create (self, obj, "stripesize", "Stripe Size", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.9.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
  volume->stripesize->unit_str = strdup ("KB");

  /* Read Max Time */
  volume->readmaxtime = l_snmp_metric_create (self, obj, "readmaxtime", "Maximum Read Time", METRIC_GAUGE, ".1.3.6.1.4.1.31165.1.1.9.1.1.5", index_oidstr, RECMETHOD_NONE, 0);
  volume->readmaxtime->unit_str = strdup ("msec");

  /* Cache */
  volume->cachepolicy = l_snmp_metric_create (self, obj, "cachepolicy", "Cache Policy", METRIC_INTEGER, ".1.3.6.1.4.1.31165.1.1.9.1.1.9", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (volume->cachepolicy, 0, "Off");
  i_metric_enumstr_add (volume->cachepolicy, 1, "Adaptive");
  i_metric_enumstr_add (volume->cachepolicy, 2, "Always");
  
  /* Enqueue the volume item */
  num = i_list_enqueue (cnt->item_list, volume);
  if (num != 0)
  { i_printf (1, "v_volume_objfact_fab failed to enqueue volume for object %s", obj->name_str); v_volume_item_free (volume); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_volume_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_volume_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the volume and remove
   * it from the item_list
   */

  int num;
  v_volume_item *volume = obj->itemptr;

  if (!volume) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, volume);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
