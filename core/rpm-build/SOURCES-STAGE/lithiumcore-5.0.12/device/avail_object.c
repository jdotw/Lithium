#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/navtree.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "record.h"
#include "avail.h"

/* Availability Object Manipulations.
 *
 * Sub-Systems that are able to reflect the
 * availability and/or response time of the
 * device can create an object under the
 * l_avail sub-systen. This object contains
 * a uniform set of availability and 
 * response time related metrics.
 */

/* Add an object */

i_object* l_avail_object_add (i_resource *self, char *name_str, char *desc_str)
{
  int num;
  i_object *obj;
  i_container *cnt;
  l_avail_item *item;
  static i_entity_refresh_config refconfig;

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  if (!obj)
  { i_printf (1, "l_avail_object_add failed to create object"); return NULL; }
  obj->cnt = l_avail_cnt ();
  obj->mainform_func = l_avail_objform;
  obj->histform_func = l_avail_objform_hist;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create item */
  item = l_avail_item_create ();
  if (!item)
  { i_printf (1, "l_avail_object_add failed to create item"); i_entity_free (obj); return NULL; }
  item->obj = obj;
  obj->itemptr = item;

  /* Register object */
  cnt = l_avail_cnt ();
  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  /* Setup refresh configuration default
   * to default to a REFMETHOD_PARENT 
   * refresh method. This default is applied
   * to all metrics and to the object
   * itself
   */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  
  /* Operation count metrics */
  item->ok_ops = i_metric_create ("ok_ops", "Successful Operations", METRIC_COUNT);
  i_entity_register (self, ENTITY(obj), ENTITY(item->ok_ops));
  i_entity_refresh_config_loadapply (self, ENTITY(item->ok_ops), &refconfig);
  
  item->fail_ops = i_metric_create ("fail_ops", "Failed Operations", METRIC_COUNT);
  i_entity_register (self, ENTITY(obj), ENTITY(item->fail_ops));
  i_entity_refresh_config_loadapply (self, ENTITY(item->fail_ops), &refconfig);
  
  item->total_ops = i_metric_acsum_create (self, obj, "total_ops", "Total Operations", METRIC_COUNT, RECMETHOD_NONE, item->ok_ops, item->fail_ops, ACSUM_REFCB_YMET);

  /* Add RRD recorded metrics for master item */
  item->ok_pc = i_metric_acpcent_create (self, obj, "ok_pc", "Success Rate", RECMETHOD_RRD, item->ok_ops, item->total_ops, ACPCENT_REFCB_MAX);
  item->ok_pc->record_priority = 2;
  item->fail_pc = i_metric_acpcent_create (self, obj, "fail_pc", "Failure Rate", RECMETHOD_RRD, item->fail_ops, item->total_ops, ACPCENT_REFCB_MAX);
  item->fail_pc->record_priority = 2;
  
  /* Response time metric */
  item->resptime = i_metric_create ("resptime", "Response Time", METRIC_FLOAT);
  item->resptime->unit_str = strdup ("ms");
  item->resptime->record_method = RECMETHOD_RRD;
  item->resptime->record_priority = 2;
  i_entity_register (self, ENTITY(obj), ENTITY(item->resptime));
  i_entity_refresh_config_loadapply (self, ENTITY(item->resptime), &refconfig);

  /* Availability CGraph */
  item->avail_cg = i_metric_cgraph_create (obj, "avail_cg", "Availability");
  item->avail_cg->width_small = METRIC_CGRAPH_WIDTH_SMALL;
  item->avail_cg->height_small = METRIC_CGRAPH_HEIGHT_SMALL;
  item->avail_cg->upper_limit = 100;
  item->avail_cg->upper_limit_set = 1;
  item->avail_cg->lower_limit = 0;
  item->avail_cg->lower_limit_set = 1;
  item->avail_cg->rigid = 1;
  asprintf (&item->avail_cg->title_str, "%s Availability", obj->desc_str);
  asprintf (&item->avail_cg->render_str, "\"AREA:met_%s_ok_pc_avg#00AA00:Successful Operations\" \"AREA:met_%s_fail_pc_avg#AA0000:Failed Operations\" \"GPRINT:met_%s_ok_pc_avg:AVERAGE:          Average Availability\\: %%.2lf %%%%\"",
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (item->avail_cg->met_list, item->ok_pc);
  i_list_enqueue (item->avail_cg->met_list, item->fail_pc);

  /* Load/Apply refresh configuration for the object */
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_PARENT;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), &refconfig);
  if (num != 0)
  { 
    i_printf (1, "l_avail_object_add failed to load and apply object refresh config for object %s", name_str);
    l_avail_object_remove (self, obj);
    return NULL;
  }
  
  /* Trigger Sets */
  obj->tset_list = i_list_create ();
  obj->tset_ht = i_hashtable_create (30);
  i_triggerset *tset = i_triggerset_create ("avail_pc", "Availability", "ok_pc");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_FLOAT, TRGTYPE_EQUAL, 0, NULL, 0, NULL, self->hierarchy->dev->refresh_interval * 2, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 1, NULL, 61, NULL, 600, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 61, NULL, 90, NULL, 600, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 0;
  i_triggerset_assign_obj (self, obj, tset);

  /* Evaluate all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);

  /* Enqueue the avail item */
  i_list_enqueue (cnt->item_list, item);

  return obj;
}

/* Remove an object */

int l_avail_object_remove (i_resource *self, i_object *obj)
{
  int num;
  l_avail_item *item = obj->itemptr;

  /* Remove from item list */
  if (obj->cnt)
  {
    num = i_list_search (obj->cnt->item_list, item);
    if (num == 0)
    { i_list_delete (obj->cnt->item_list); }
  }

  /* Free the item */
  if (item)
  { l_avail_item_free (item); obj->itemptr = NULL; }

  /* Deregister the object */
  i_entity_deregister (self, ENTITY(obj));

  /* Free the object */
  i_entity_free (ENTITY(obj));

  return 0;
}

