#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/interface.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/adminstate.h>
#include <induction/triggerset.h>
#include <induction/str.h>
#include <induction/name.h>
#include "device/snmp.h"

#include "xsanvol.h"
#include "xsannode.h"

/* Object Factory Fabrication (Xsan Node/LUN) */

int v_xsannode_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_xsannode_item *node;
  v_xsanvol_item *vol = (v_xsanvol_item *) passdata;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_xsannode_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create storage item struct */
  node = v_xsannode_item_create ();
  if (!node)
  { i_printf (1, "v_xsannode_objfact_fab failed to create node item for object %s", obj->name_str); return -1; }
  obj->itemptr = node;
  node->obj = obj;
  node->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];
  
  /* Adjust the index_oidstr to include the implied volume index */
  asprintf(&index_oidstr, "%i.%s", vol->index, index_oidstr);

  /* 
   * Metric Creation 
   */

  node->wwn = l_snmp_metric_create (self, obj, "wwn", "WWN", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.1.3.1.6", index_oidstr, RECMETHOD_NONE, 0);
  node->blockdev = l_snmp_metric_create (self, obj, "blockdev", "Block Device", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.1.3.1.8", index_oidstr, RECMETHOD_NONE, 0);
  node->hostinquiry = l_snmp_metric_create (self, obj, "hostinquiry", "Host", METRIC_STRING, ".1.3.6.1.4.1.20038.2.1.1.3.1.9", index_oidstr, RECMETHOD_NONE, 0);

  node->sectorsize = l_snmp_metric_create (self, obj, "sectorsize", "Sector Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.3.1.11", index_oidstr, RECMETHOD_NONE, 0);
  node->sectorsize->unit_str = strdup("byte");
  node->sectorsize->valstr_func = i_string_volume_metric;

  node->sectors = l_snmp_metric_create (self, obj, "sectors", "Sectors", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.3.1.14", index_oidstr, RECMETHOD_NONE, 0);
  node->sectors->alloc_unit = 1000;
  node->maxsectors = l_snmp_metric_create (self, obj, "maxsectors", "Max Sectors", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.3.1.15", index_oidstr, RECMETHOD_NONE, 0);
  node->maxsectors->alloc_unit = 1000;

  node->visible = l_snmp_metric_create (self, obj, "visible", "Visible", METRIC_INTEGER, ".1.3.6.1.4.1.20038.2.1.1.3.1.16", index_oidstr, RECMETHOD_NONE, 0);
  node->visible->summary_flag = 1;
  i_metric_enumstr_add (node->visible, 1, "Yes");
  i_metric_enumstr_add (node->visible, 0, "No");

  node->size = l_snmp_metric_create (self, obj, "size", "Size", METRIC_GAUGE, ".1.3.6.1.4.1.20038.2.1.1.3.1.17", index_oidstr, RECMETHOD_NONE, 0);
  node->size->alloc_unit = 1000 * 1000;
  node->size->valstr_func = i_string_volume_metric;
  node->size->unit_str = strdup ("byte");
  node->size->kbase = 1000;
  node->size->summary_flag = 1;
  
  /* Enqueue the storage item */
  num = i_list_enqueue (cnt->item_list, node);
  if (num != 0)
  { i_printf (1, "v_xsannode_objfact_fab failed to enqueue storage item for object %s", obj->name_str); v_xsannode_item_free (node); return -1; }


  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_xsannode_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
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

int v_xsannode_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  int num;
  v_xsannode_item *node = obj->itemptr;
  
  /* Remove from item list */
  num = i_list_search (cnt->item_list, node);
  if (num == 0) 
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;

  return 0;
}
