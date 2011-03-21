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
#include "induction/name.h"
#include "induction/str.h"
#include "induction/xsanregistry.h"
#include "induction/adminstate.h"
#include "device/snmp.h"
#include "device/record.h"

#include "osx_server.h"
#include "xsan.h"
#include "data.h"
#include "xsanvol.h"

/* Xsan Volumes Info */

static i_container *static_cnt = NULL;
static l_snmp_objfact *static_objfact = NULL;

/* Variable Retrieval */

i_container* v_xsanvol_cnt ()
{ return static_cnt; }

v_xsanvol_item* v_xsanvol_get (char *desc_str)
{
  v_xsanvol_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->desc_str, desc_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_xsanvol_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsanvol", "Xsan Volumes");
  if (!static_cnt)
  { i_printf (1, "v_xsanvol_enable failed to create container"); v_xsanvol_disable (self); return -1; }
//  static_cnt->mainform_func = v_xsanvol_cntform;
//  static_cnt->sumform_func = v_xsanvol_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_xsanvol_enable failed to register container"); v_xsanvol_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_xsanvol_enable failed to load and apply container refresh config"); v_xsanvol_disable (self); return -1; }

  /*
   * Triggers
   */
   
  i_triggerset *tset;

  tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 75, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
//  tset = i_triggerset_create ("host_state", "Host State", "host_state");
//  i_triggerset_addtrg (self, tset, "mdcfailover", "MDC Failed Over", VALTYPE_INTEGER, TRGTYPE_EQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);

//  Commented out because of no state flag in Xsan 2.2.1
//  tset = i_triggerset_create ("state", "State", "state");
//  i_triggerset_addtrg (self, tset, "not_registered", "Not Registered", VALTYPE_INTEGER, TRGTYPE_NOTEQUAL, 1, NULL, 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
//  i_triggerset_assign (self, static_cnt, tset);

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_xsanvol_enable failed to create item_list"); v_xsanvol_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /*
   * Setup data polling
   */

  if (l_snmp_xsnmp_enabled())
  {
    /* Using Xsnmp -- nice. */

    /* Create the object factory */
    static_objfact = l_snmp_objfact_create (self, static_cnt->name_str, static_cnt->desc_str);
    if (!static_objfact)
    {
      i_printf (1, "v_xsanvol_enable failed to call l_snmp_objfact_create to create the objfact"); 
      return -1;
    }
    static_objfact->dev = self->hierarchy->dev;
    static_objfact->cnt = static_cnt;
    static_objfact->name_oid_str = strdup (".1.3.6.1.4.1.20038.2.1.1.1.1.2");
    static_objfact->fabfunc = v_xsanvol_objfact_fab;
    static_objfact->ctrlfunc = v_xsanvol_objfact_ctrl;
    static_objfact->cleanfunc = v_xsanvol_objfact_clean;
    num = l_snmp_objfact_start (self, static_objfact);
    if (num != 0)
    { 
      i_printf (1, "v_xsanvol_enable failed to call l_snmp_objfact_start to start the object factory"); 
    }
  }
  else
  {
    /* Using servermgrd -- evil. */
    v_data_item *dataitem = v_data_static_item();
  
    /* Standard refresh config */
    memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
    defrefconfig.refresh_method = REFMETHOD_PARENT;
    defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
    defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

    /* Sysinfo metric */
    dataitem->xsan_settings = i_metric_create ("xsan_settings", "Xsan Settings", METRIC_INTEGER);
    i_metric_enumstr_add (dataitem->xsan_settings, 0, "Invalid");
    i_metric_enumstr_add (dataitem->xsan_settings, 1, "Current");
    i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan_settings));
    i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan_settings), &defrefconfig);
    dataitem->xsan_settings->refresh_func = v_data_xsan_settings_refresh;
  }
  
  return 0;
}

v_xsanvol_item* v_xsanvol_create (i_resource *self, char *name)
{
  char *name_str;
  char *desc_str;
  v_xsanvol_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Set name/desc */
  name_str = strdup (name);
  i_name_parse (name_str);
  desc_str = strdup (name);

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  free (name_str);
  name_str = NULL;
  free (desc_str);
  desc_str = NULL;
  obj->refresh_func = v_data_xsan_volume_refresh;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_xsanvol_item_create ();
  item->obj = obj;
  item->raw_volume_name = strdup (name);
  obj->itemptr = item;
  i_list_enqueue (static_cnt->item_list, item);

  /*
   * Create Metrics 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* RUn-time */
  
  item->bandwidth_management = i_metric_create ("bandwidth_management", "Bandwidth Management", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->bandwidth_management));
  i_entity_refresh_config_apply (self, ENTITY(item->bandwidth_management), &refconfig);
  
  item->creation_time = i_metric_create ("creation_time", "Creation Time", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->creation_time));
  i_entity_refresh_config_apply (self, ENTITY(item->creation_time), &refconfig);
  
  item->license_valid = i_metric_create ("license_valid", "License Valid", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->license_valid));
  i_entity_refresh_config_apply (self, ENTITY(item->license_valid), &refconfig);
  
  item->host = i_metric_create ("host", "Host", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->host));
  i_entity_refresh_config_apply (self, ENTITY(item->host), &refconfig);
  
  item->host_ip = i_metric_create ("host_ip", "Host IP Address", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->host_ip));
  i_entity_refresh_config_apply (self, ENTITY(item->host_ip), &refconfig);
  
  item->host_state = i_metric_create ("host_state", "Host State", METRIC_INTEGER);
  i_metric_enumstr_add (item->host_state, 0, "Unknown");
  i_metric_enumstr_add (item->host_state, 1, "Backup MDC");
  i_metric_enumstr_add (item->host_state, 2, "Active MDC");
  i_entity_register (self, ENTITY(obj), ENTITY(item->host_state));
  i_entity_refresh_config_apply (self, ENTITY(item->host_state), &refconfig);    
  
  item->msgbuf_size = i_metric_create ("msgbuf_size", "Message Buffer Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->msgbuf_size));
  i_entity_refresh_config_apply (self, ENTITY(item->msgbuf_size), &refconfig);
  
  item->connected_clients = i_metric_create ("connected_clients", "Connected Clients", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->connected_clients));
  i_entity_refresh_config_apply (self, ENTITY(item->connected_clients), &refconfig);
  
  item->devices = i_metric_create ("devices", "Devices", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->devices));
  i_entity_refresh_config_apply (self, ENTITY(item->devices), &refconfig);
  
  item->stripe_groups = i_metric_create ("stripe_groups", "Stripe Groups", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->stripe_groups));
  i_entity_refresh_config_apply (self, ENTITY(item->stripe_groups), &refconfig);
  
  item->port = i_metric_create ("port", "Port", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->port));
  i_entity_refresh_config_apply (self, ENTITY(item->port), &refconfig);
  
  item->priority = i_metric_create ("priority", "Priority", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->priority));
  i_entity_refresh_config_apply (self, ENTITY(item->priority), &refconfig);
  
  item->pid = i_metric_create ("pid", "PID", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->pid));
  i_entity_refresh_config_apply (self, ENTITY(item->pid), &refconfig);
  
  item->qos = i_metric_create ("qos", "QOS Enabled", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->qos));
  i_entity_refresh_config_apply (self, ENTITY(item->qos), &refconfig);

//  Commented out because Xsan 2.2.1 doesnt support state   
//  item->state = i_metric_create ("state", "State", METRIC_INTEGER);
//  i_entity_register (self, ENTITY(obj), ENTITY(item->state));
//  i_entity_refresh_config_apply (self, ENTITY(item->state), &refconfig);
  
  item->bytes_free = i_metric_create ("bytes_free", "Free Bytes", METRIC_GAUGE);
  item->bytes_free->alloc_unit = (1024 * 1024);
  item->bytes_free->unit_str = strdup ("byte");
  item->bytes_free->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_free));
  i_entity_refresh_config_apply (self, ENTITY(item->bytes_free), &refconfig);

  item->bytes_total = i_metric_create ("bytes_total", "Total Bytes", METRIC_GAUGE);
  item->bytes_total->alloc_unit = (1024 * 1024);
  item->bytes_total->unit_str = strdup ("byte");
  item->bytes_total->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_total));
  i_entity_refresh_config_apply (self, ENTITY(item->bytes_total), &refconfig);

  item->bytes_used = i_metric_acdiff_create (self, obj, "bytes_used", "Used Bytes", METRIC_GAUGE, RECMETHOD_NONE, item->bytes_total, item->bytes_free, ACDIFF_REFCB_YMET);
  item->bytes_used->alloc_unit = (1024 * 1024);
  item->bytes_used->unit_str = strdup ("byte");
  item->bytes_used->valstr_func = i_string_volume_metric;
  
  item->used_pc = i_metric_acpcent_create (self, obj, "used_pc", "Used Percent", RECMETHOD_RRD, item->bytes_used, item->bytes_total, ACPCENT_REFCB_GAUGE);
  item->used_pc->record_defaultflag = 1;

  /* Config */

  item->allocation_strategy = i_metric_create ("allocation_strategy", "Allocation Strategy", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->allocation_strategy));
  i_entity_refresh_config_apply (self, ENTITY(item->allocation_strategy), &refconfig);    
    
  item->buffer_cache_size = i_metric_create ("buffer_cache_size", "Buffer Cache Size", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->buffer_cache_size));
  i_entity_refresh_config_apply (self, ENTITY(item->buffer_cache_size), &refconfig);    
    
  item->debug = i_metric_create ("debug", "Debug", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->debug));
  i_entity_refresh_config_apply (self, ENTITY(item->debug), &refconfig);    
    
  item->enforce_acls = i_metric_create ("enforce_acls", "Enforce ACLs", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->enforce_acls));
  i_entity_refresh_config_apply (self, ENTITY(item->enforce_acls), &refconfig);    
    
  item->file_locks = i_metric_create ("file_locks", "File Locks", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->file_locks));
  i_entity_refresh_config_apply (self, ENTITY(item->file_locks), &refconfig);    
    
  item->force_stripe_alignment = i_metric_create ("force_stripe_alignment", "Force Stripe Alignment", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->force_stripe_alignment));
  i_entity_refresh_config_apply (self, ENTITY(item->force_stripe_alignment), &refconfig);    
    
  item->fs_block_size = i_metric_create ("fs_block_size", "File System Block Size", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->fs_block_size));
  i_entity_refresh_config_apply (self, ENTITY(item->fs_block_size), &refconfig);    
    
  item->global_super_user = i_metric_create ("global_super_user", "Global Super User", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->global_super_user));
  i_entity_refresh_config_apply (self, ENTITY(item->global_super_user), &refconfig);    
    
  item->inode_cache_size = i_metric_create ("inode_cache_size", "Inode Cache Size", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_cache_size));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_cache_size), &refconfig);    
    
  item->inode_expand_inc = i_metric_create ("inode_expand_inc", "Inode Expand Inc", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_expand_inc));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_expand_inc), &refconfig);    
    
  item->inode_expand_max = i_metric_create ("inode_expand_max", "Inode Expand Max", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_expand_max));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_expand_max), &refconfig);    
    
  item->inode_expand_min = i_metric_create ("inode_expand_min", "Inode Expand Minimum", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->inode_expand_min));
  i_entity_refresh_config_apply (self, ENTITY(item->inode_expand_min), &refconfig);    
    
  item->journal_size = i_metric_create ("journal_size", "Journal Size", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->journal_size));
  i_entity_refresh_config_apply (self, ENTITY(item->journal_size), &refconfig);    
    
  item->max_connections = i_metric_create ("max_connections", "Maximum Connections", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->max_connections));
  i_entity_refresh_config_apply (self, ENTITY(item->max_connections), &refconfig);    

  item->connused_pc = i_metric_acpcent_create (self, obj, "connused_pc", "Connections Used", RECMETHOD_NONE, item->connected_clients, item->max_connections, ACPCENT_REFCB_GAUGE);
    
  item->max_logsize = i_metric_create ("max_logsize", "Max Logsize", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->max_logsize));
  i_entity_refresh_config_apply (self, ENTITY(item->max_logsize), &refconfig);    
    
  item->quotas = i_metric_create ("quotas", "Quotas", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->quotas));
  i_entity_refresh_config_apply (self, ENTITY(item->quotas), &refconfig);    
    
  item->threadpool_size = i_metric_create ("threadpool_size", "Threadpool Size", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->threadpool_size));
  i_entity_refresh_config_apply (self, ENTITY(item->threadpool_size), &refconfig);    
    
  item->unix_fab_window = i_metric_create ("unix_fab_window", "Unix Fab Window", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->unix_fab_window));
  i_entity_refresh_config_apply (self, ENTITY(item->unix_fab_window), &refconfig);    
    
  item->windows_security = i_metric_create ("windows_security", "Windows Security", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->windows_security));
  i_entity_refresh_config_apply (self, ENTITY(item->windows_security), &refconfig);    

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  /* Register */
  if (v_xsan_role() != 0)
  { i_xsanregistry_register (self, XSAN_TYPE_CONTROLLER, obj); }

  /* Set host state admin state */
  i_adminstate_change (self, ENTITY(item->host_state), ENTADMIN_DISABLED);

  /*
   * Create xsanvol_<name> container for Storage Pool objects 
   */

  /* Create/Config Container */
  asprintf (&name_str, "xsanvolsp_%s", obj->name_str);
  asprintf (&desc_str, "%s Storage Pools", obj->desc_str);
  i_name_parse (name_str);
  i_container *cnt = i_container_create (name_str, desc_str);
  free (name_str);
  name_str = NULL;
  free (desc_str);
  desc_str = NULL;
//  cnt->mainform_func = v_xsanvol_cntform;
//  cnt->sumform_func = v_xsanvol_cntform;
  item->sp_cnt = cnt;

  /* Register entity */
  i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));

  /* Load/Apply refresh config */
  i_entity_refresh_config defrefconfig;
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  
  /*
   * Triggers
   */
  i_triggerset *tset;

  tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 75, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);
  
  /* 
   * Item and objects 
   */
  
  /* Create item list */
  cnt->item_list = i_list_create ();
  cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  return item;
}

int v_xsanvol_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

