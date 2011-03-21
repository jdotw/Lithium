#include <stdlib.h>

#include "induction.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/hierarchy.h"
#include "induction/list.h"
#include "induction/str.h"

#include "device/snmp.h"

#include "pixconn.h"

/* 
 * Cisco PIX Connections - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_pixconn_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  char *str;
  v_pixconn_item *pixconn;

  /* Check index to stop duplictes */
  if (pdu->variables->name[pdu->variables->name_length-1] != 6) return -1;

  /* Object Configuration */
  obj->mainform_func = v_pixconn_objform;
  obj->histform_func = v_pixconn_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_pixconn_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create pixconn item struct */
  pixconn = v_pixconn_item_create ();
  if (!pixconn)
  { i_printf (1, "v_pixconn_objfact_fab failed to create pixconn item for object %s", obj->name_str); return -1; }
  pixconn->obj = obj;
  obj->itemptr = pixconn;
  pixconn->service = (unsigned long) pdu->variables->name[pdu->variables->name_length-2];

  /* Set obj description */
  switch (pixconn->service)
  {
    case 40: obj->desc_str = strdup ("IP");
             break;
    default: asprintf (&obj->desc_str, "Unknown (%lu)", pixconn->service);
  }
  
  /* 
   * Metric Creation 
   */

  asprintf (&str, "%lu.6", pixconn->service);
  pixconn->curconn = l_snmp_metric_create (self, obj, "curconn", "Current Connections", METRIC_GAUGE, "enterprises.9.9.147.1.2.2.2.1.5", str, RECMETHOD_RRD, 0);
  free (str);
  pixconn->curconn->record_defaultflag = 1;

  asprintf (&str, "%lu.7", pixconn->service);
  pixconn->highconn = l_snmp_metric_create (self, obj, "highconn", "Highest Connection Count", METRIC_GAUGE, "enterprises.9.9.147.1.2.2.2.1.5", str, RECMETHOD_NONE, 0);
  free (str);
  
  /* Enqueue the pixconn item */
  num = i_list_enqueue (cnt->item_list, pixconn);
  if (num != 0)
  { i_printf (1, "v_pixconn_objfact_fab failed to enqueue pixconn for object %s", obj->name_str); v_pixconn_item_free (pixconn); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_pixconn_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_pixconn_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the pixconn and remove
   * it from the item_list
   */

  int num;
  v_pixconn_item *pixconn = obj->itemptr;

  if (!pixconn) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, pixconn);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
