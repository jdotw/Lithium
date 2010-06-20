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
#include <lithium/record.h>
#include <lithium/snmp.h>

#include "mempool.h"

/* HP Procurve Memory  Sub-System */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_mempool_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_mempool_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  static_cnt = i_container_create ("hpmempool", "Memory");
  if (!static_cnt)
  { i_printf (1, "v_mempool_enable failed to create container"); v_mempool_disable (self); return -1; }
  static_cnt->mainform_func = v_mempool_cntform;
  static_cnt->sumform_func = v_mempool_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_mempool_enable failed to register container"); v_mempool_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_mempool_enable failed to load and apply container refresh config"); v_mempool_disable (self); return -1; }

  /*
   * Trigger Sets 
   */

  /* Used Percent */
  tset = i_triggerset_create("local_used_pc", "Local Percent Used", "local_used_pc");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_FLOAT, TRGTYPE_GT, 97, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 90, NULL, 97, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 90, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
    
  tset = i_triggerset_create("global_used_pc", "Global Percent Used", "global_used_pc");
  i_triggerset_addtrg (self, tset, "failed", "Failed", VALTYPE_FLOAT, TRGTYPE_GT, 97, NULL, 0, NULL, 0, ENTSTATE_FAILED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_RANGE, 90, NULL, 97, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 90, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_mempool_enable failed to create item_list"); v_mempool_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_mempool_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("slot1", "Slot 1");
  if (!static_obj)
  { i_printf (1, "v_mempool_enable failed to create slot1  object"); v_mempool_disable (self); return -1; }
  static_obj->mainform_func = v_mempool_objform;

  /* Create battery item */
  v_mempool_item *mempool = v_mempool_item_create ();
  if (!mempool)
  { i_printf (1, "v_mempool_enable failed to create slot1 item struct"); v_mempool_disable (self); return -1; }
  mempool->obj = static_obj;
  static_obj->itemptr = mempool;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, mempool);
  if (num != 0)
  { i_printf (1, "v_mempool_enable failed to enqueue item into static_cnt->item_list"); v_mempool_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Local */
  mempool->local_slab = l_snmp_metric_create (self, static_obj, "local_slab", "Local Slab Count", METRIC_COUNT, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.2","1", RECMETHOD_NONE, 0);

  mempool->local_freeseg = l_snmp_metric_create (self, static_obj, "local_freeseg", "Local Free Segment Count", METRIC_COUNT, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.3","1", RECMETHOD_NONE, 0);
  
  mempool->local_allocseg = l_snmp_metric_create (self, static_obj, "local_allocseg", "Local Allocated Segment Count", METRIC_COUNT, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.4","1", RECMETHOD_NONE, 0);

  mempool->local_size = l_snmp_metric_create (self, static_obj, "local_size", "Local Memory Total Bytes", METRIC_GAUGE, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.5","1", RECMETHOD_NONE, 0);
  mempool->local_size->valstr_func = i_string_volume_metric;
  mempool->local_size->unit_str = strdup ("byte");
  mempool->local_size->kbase = 1024;
        
  mempool->local_free = l_snmp_metric_create (self, static_obj, "local_free", "Local Memory Free Bytes", METRIC_GAUGE, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.6","1", RECMETHOD_NONE, 0);
  mempool->local_free->valstr_func = i_string_volume_metric;
  mempool->local_free->unit_str = strdup ("byte");
  mempool->local_free->kbase = 1024;
  
  mempool->local_used = l_snmp_metric_create (self, static_obj, "local_used", "Local Memory Allocated Bytes", METRIC_GAUGE, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.7","1", RECMETHOD_NONE, 0);
  mempool->local_used->valstr_func = i_string_volume_metric;
  mempool->local_used->unit_str = strdup ("byte");
  mempool->local_used->kbase = 1024;
  
  mempool->local_used_pc = i_metric_acpcent_create (self, static_obj, "local_used_pc", "Local Memory Used Percent", RECMETHOD_RRD, mempool->local_used, mempool->local_size, ACPCENT_REFCB_GAUGE);
  mempool->local_used_pc->record_defaultflag = 1;
   
  /* Global */
  mempool->global_slab = l_snmp_metric_create (self, static_obj, "global_slab", "Global Slab Count", METRIC_COUNT, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.2","1", RECMETHOD_NONE, 0);

  mempool->global_freeseg = l_snmp_metric_create (self, static_obj, "global_freeseg", "Global Free Segment Count", METRIC_COUNT, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.3","1", RECMETHOD_NONE, 0);
  
  mempool->global_allocseg = l_snmp_metric_create (self, static_obj, "global_allocseg", "Global Allocated Segment Count", METRIC_COUNT, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.4","1", RECMETHOD_NONE, 0);

  mempool->global_size = l_snmp_metric_create (self, static_obj, "global_size", "Global Memory Total Bytes", METRIC_GAUGE, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.5","1", RECMETHOD_NONE, 0);
  mempool->global_size->valstr_func = i_string_volume_metric;
  mempool->global_size->unit_str = strdup ("byte");
  mempool->global_size->kbase = 1024;

  mempool->global_free = l_snmp_metric_create (self, static_obj, "global_free", "Global Memory Free Bytes", METRIC_GAUGE, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.6","1", RECMETHOD_NONE, 0);
  mempool->global_free->valstr_func = i_string_volume_metric;
  mempool->global_free->unit_str = strdup ("byte");
  mempool->global_free->kbase = 1024;
  
  mempool->global_used = l_snmp_metric_create (self, static_obj, "global_used", "Global Memory Allocated Bytes", METRIC_GAUGE, ".1.3.6.1.4.1.11.2.14.11.5.1.1.2.1.1.1.7","1", RECMETHOD_NONE, 0);
  mempool->global_used->valstr_func = i_string_volume_metric;
  mempool->global_used->unit_str = strdup ("byte");
  mempool->global_used->kbase = 1024;
  
  mempool->global_used_pc = i_metric_acpcent_create (self, static_obj, "global_used_pc", "Global Memory Used Percent", RECMETHOD_RRD, mempool->global_used, mempool->global_size, ACPCENT_REFCB_GAUGE);
  mempool->global_used_pc->record_defaultflag = 1;
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_mempool_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

