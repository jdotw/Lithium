#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <induction/str.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "index.h"
#include "cache.h"

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_cache_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_cache_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("iftcache", "Cache");
  if (!static_cnt)
  { i_printf (1, "v_cache_enable failed to create container"); v_cache_disable (self); return -1; }
  //static_cnt->macacherm_func = v_cache_cntform;
  //static_cnt->sumform_func = v_cache_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_cache_enable failed to register container"); v_cache_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_cache_enable failed to load and apply container refresh config"); v_cache_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_cache_enable failed to create item_list"); v_cache_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_cache_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_POPULATE;

  /* Triggers */
  i_triggerset *tset = i_triggerset_create ("dirty_pc", "Cache Usage", "dirty_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 98.0, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_cache_enable failed to create master object"); v_cache_disable (self); return -1; }
  //static_obj->macacherm_func = v_cache_objform;

  /* Create cache item */
  v_cache_item *cache = v_cache_item_create ();
  if (!cache)
  { i_printf (1, "v_cache_enable failed to create cache item struct"); v_cache_disable (self); return -1; }
  cache->obj = static_obj;
  static_obj->itemptr = cache;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, cache);
  if (num != 0)
  { i_printf (1, "v_cache_enable failed to enqueue item into static_cnt->item_list"); v_cache_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  i_object *obj = static_obj;

  /* Block Size Index (power of 2)*/
  cache->blocksize_index = l_snmp_metric_create (self, obj, "blocksize_index", "Block Size Index", METRIC_INTEGER, ".1.3.6.1.4.1.1714.1.1.2.2", "0", RECMETHOD_NONE, 0);  
  cache->blocksize = i_metric_create ("blocksize", "Blocksize", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(cache->blocksize));
  i_entity_refreshcb_add (ENTITY(cache->blocksize_index), v_index_refcb, cache->blocksize);

  /* Size (in blocks) */
  cache->total_blocks = l_snmp_metric_create (self, obj, "total_blocks", "Total (in blocks)", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.1.2.3", "0", RECMETHOD_NONE, 0);  
  cache->total_blocks->unit_str = strdup ("blocks");
  cache->dirty_blocks = l_snmp_metric_create (self, obj, "dirty_blocks", "Dirty (in blocks)", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.1.2.4", "0", RECMETHOD_NONE, 0);  
  cache->dirty_blocks->unit_str = strdup ("blocks");
  
  /* Size */
  cache->total = l_snmp_metric_create (self, obj, "total", "Total", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.1.2.3", "0", RECMETHOD_NONE, 0);
  cache->total->alloc_unit_met = cache->blocksize;
  cache->total->valstr_func = i_string_volume_metric;
  cache->total->unit_str = strdup ("byte");
  cache->total->kbase = 1024;
  cache->dirty = l_snmp_metric_create (self, obj, "dirty", "Dirty", METRIC_GAUGE, ".1.3.6.1.4.1.1714.1.1.2.4", "0", RECMETHOD_NONE, 0);
  cache->dirty->alloc_unit_met = cache->blocksize;
  cache->dirty->valstr_func = i_string_volume_metric;
  cache->dirty->unit_str = strdup ("byte");
  cache->dirty->kbase = 1024;

  /* Percent */
  cache->dirty_pc = i_metric_acpcent_create (self, obj, "dirty_pc", "Dirty Percent", RECMETHOD_RRD, cache->dirty, cache->total, ACPCENT_REFCB_GAUGE);
  cache->dirty_pc->record_defaultflag = 1;

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_cache_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

