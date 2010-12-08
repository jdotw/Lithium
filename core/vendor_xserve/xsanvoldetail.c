#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "induction/str.h"
#include "device/snmp.h"
#include "device/record.h"

#include "xsanvoldetail.h"

/* Xraid System Information Sub-System */

static i_list* static_vol_list = NULL;

/* Variable Retrieval */

v_xsanvoldetail_item* v_xsanvoldetail_get (char *name_str)
{
  v_xsanvoldetail_item *item;
  for (i_list_move_head(static_vol_list); (item=i_list_restore(static_vol_list))!=NULL; i_list_move_next(static_vol_list))
  { if (strcmp(item->obj->name_str, name_str) == 0) return item; }
  return NULL;
}

/* Enable / Disable */

int v_xsanvoldetail_enable (i_resource *self)
{
  /* Create array lists */
  static_vol_list = i_list_create ();
  i_list_set_destructor (static_vol_list, v_xsanvoldetail_item_free);

  return 0;
}

v_xsanvoldetail_item* v_xsanvoldetail_create (i_resource *self, char *vol_name)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;
  char *name_str;
  char *desc_str;
  i_container *cnt;
  
  /* Create/Config Container */
  asprintf (&name_str, "xsanvoldetail_%s", vol_name);
  asprintf (&desc_str, "Xsan Vol: %s", vol_name);
  cnt = i_container_create (name_str, desc_str);
  if (!cnt)
  { i_printf (1, "v_xsanvoldetail_create failed to create container"); return NULL; }
//  cnt->mainform_func = v_xsanvoldetail_cntform;
//  cnt->sumform_func = v_xsanvoldetail_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "v_xsanvoldetail_create failed to register container"); return NULL; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_xsanvoldetail_create failed to load and apply container refresh config"); return NULL; }

  /* Create item list */
  cnt->item_list = i_list_create ();
  if (!cnt->item_list)
  { i_printf (1, "v_xsanvoldetail_create failed to create item_list"); return NULL; }
  cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create info object */
  v_xsanvoldetail_item *item;

  /* Create table object */
  obj = i_object_create ("master", "Master");
//  obj->mainform_func = v_ipacct_data_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  /* Create item */
  item = v_xsanvoldetail_item_create ();
  item->obj = obj;
  item->cnt = cnt;
  obj->itemptr = item;

  /*
   * Triggers
   */
  i_triggerset *tset;
  
  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  item->allocation_strategy = i_metric_create ("allocation_strategy", "allocation_strategy", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->allocation_strategy));
  i_entity_refresh_config_apply (self, ENTITY(item->allocation_strategy), &refconfig);    
    
  item->buffer_cache_size = i_metric_create ("buffer_cache_size", "buffer_cache_size", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->buffer_cache_size));
  i_entity_refresh_config_apply (self, ENTITY(item->buffer_cache_size), &refconfig);    
    
  item->debug = i_metric_create ("debug", "debug", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->debug));
  i_entity_refresh_config_apply (self, ENTITY(item->debug), &refconfig);    
    
  item->enforce_acls = i_metric_create ("enforce_acls", "enforce_acls", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->enforce_acls));
  i_entity_refresh_config_apply (self, ENTITY(item->enforce_acls), &refconfig);    
    
  item->file_locks = i_metric_create ("file_locks", "file_locks", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->file_locks));
  i_entity_refresh_config_apply (self, ENTITY(item->file_locks), &refconfig);    
    
  item->force_stripe_alignment = i_metric_create ("force_stripe_alignment", "force_stripe_alignment", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->force_stripe_alignment));
  i_entity_refresh_config_apply (self, ENTITY(item->force_stripe_alignment), &refconfig);    
    
  item->fs_block_size = i_metric_create ("fs_block_size", "fs_block_size", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->fs_block_size));
  i_entity_refresh_config_apply (self, ENTITY(item->fs_block_size), &refconfig);    
    
  item->global_super_user = i_metric_create ("global_super_user", "global_super_user", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->global_super_user));
  i_entity_refresh_config_apply (self, ENTITY(item->global_super_user), &refconfig);    
    
  item->inode_cache_size = i_metric_create ("inode_cache_size", "inode_cache_size", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_cache_size));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_cache_size), &refconfig);    
    
  item->inode_expand_inc = i_metric_create ("inode_expand_inc", "inode_expand_inc", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_expand_inc));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_expand_inc), &refconfig);    
    
  item->inode_expand_max = i_metric_create ("inode_expand_max", "inode_expand_max", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_expand_max));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_expand_max), &refconfig);    
    
  item->inode_expand_min = i_metric_create ("inode_expand_min", "inode_expand_min", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_expand_min));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_expand_min), &refconfig);    
    
  item->journal_size = i_metric_create ("journal_size", "journal_size", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->journal_size));
  i_entity_refresh_config_apply (self, ENTITY(item->journal_size), &refconfig);    
    
  item->max_connections = i_metric_create ("max_connections", "max_connections", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->max_connections));
  i_entity_refresh_config_apply (self, ENTITY(item->max_connections), &refconfig);    
    
  item->max_logsize = i_metric_create ("max_logsize", "max_logsize", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->max_logsize));
  i_entity_refresh_config_apply (self, ENTITY(item->max_logsize), &refconfig);    
    
  item->quotas = i_metric_create ("quotas", "quotas", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->quotas));
  i_entity_refresh_config_apply (self, ENTITY(item->quotas), &refconfig);    
    
  item->threadpool_size = i_metric_create ("threadpool_size", "threadpool_size", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->threadpool_size));
  i_entity_refresh_config_apply (self, ENTITY(item->threadpool_size), &refconfig);    
    
  item->unix_fab_window = i_metric_create ("unix_fab_window", "unix_fab_window", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->unix_fab_window));
  i_entity_refresh_config_apply (self, ENTITY(item->unix_fab_window), &refconfig);    
    
  item->unix_fab_window = i_metric_create ("unix_fab_window", "unix_fab_window", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->unix_fab_window));
  i_entity_refresh_config_apply (self, ENTITY(item->unix_fab_window), &refconfig);    
    
  i_list_enqueue (static_vol_list, item);

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);
  
  return item;
}

int v_xsanvoldetail_disable (i_resource *self)
{
  /* Deregister container */

  return 0;
}

