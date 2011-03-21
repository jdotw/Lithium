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
#include <induction/name.h>
#include <induction/str.h>
#include <lithium/snmp.h>
#include <lithium/record.h>

#include "osx.h"
#include "data.h"
#include "xsancomputer.h"

/* Xsan Volumes Info */

static i_container *static_cnt = NULL;

/* Variable Retrieval */

i_container* v_xsancomputer_cnt ()
{ return static_cnt; }

v_xsancomputer_item* v_xsancomputer_get (char *desc_str)
{
  v_xsancomputer_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->desc_str, desc_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_xsancomputer_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xsancomputer", "Xsan Computers");
  if (!static_cnt)
  { i_printf (1, "v_xsancomputer_enable failed to create container"); v_xsancomputer_disable (self); return -1; }
//  static_cnt->mainform_func = v_xsancomputer_cntform;
//  static_cnt->sumform_func = v_xsancomputer_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_xsancomputer_enable failed to register container"); v_xsancomputer_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_xsancomputer_enable failed to load and apply container refresh config"); v_xsancomputer_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_xsancomputer_enable failed to create item_list"); v_xsancomputer_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /*
   * Create Data Metrics 
   */

  v_data_item *dataitem = v_data_static_item();

  /* Standard refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Sysinfo metric */
  dataitem->xsan_settings = i_metric_create ("xserve", "Xserve", METRIC_INTEGER);
  i_metric_enumstr_add (dataitem->xsan_settings, 0, "Invalid");
  i_metric_enumstr_add (dataitem->xsan_settings, 1, "Current");
  i_entity_register (self, ENTITY(dataitem->obj), ENTITY(dataitem->xsan_settings));
  i_entity_refresh_config_apply (self, ENTITY(dataitem->xsan_settings), &defrefconfig);
  dataitem->xsan_settings->refresh_func = v_data_xsan_settings_refresh;

  return 0;
}

v_xsancomputer_item* v_xsancomputer_create (i_resource *self, char *name)
{
  char *name_str;
  char *desc_str;
  v_xsancomputer_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Set name/desc */
  name_str = strdup (name);
  i_name_parse (name_str);
  desc_str = strdup (name);

  /* Create object */
  obj = i_object_create (name_str, desc_str);

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_xsancomputer_item_create ();
  item->obj = obj;
  obj->itemptr = item;
  i_list_enqueue (static_cnt->item_list, item);

  /*
   * Create Metrics 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  item->ip = i_metric_create ("ip", "IP Address", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->ip));
  i_entity_refresh_config_apply (self, ENTITY(item->ip), &refconfig);
  
  item->cfsversion = i_metric_create ("cfsversion", "Client File System Version", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->cfsversion));
  i_entity_refresh_config_apply (self, ENTITY(item->cfsversion), &refconfig);
  
  item->sfsversion = i_metric_create ("sfsversion", "Server File System Version", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->sfsversion));
  i_entity_refresh_config_apply (self, ENTITY(item->sfsversion), &refconfig);
  
  item->name = i_metric_create ("name", "name", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->name));
  i_entity_refresh_config_apply (self, ENTITY(item->name), &refconfig);
  
  item->hostname = i_metric_create ("hostname", "Hostname", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->hostname));
  i_entity_refresh_config_apply (self, ENTITY(item->hostname), &refconfig);
  
  item->rendezname = i_metric_create ("redezname", "redezname", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->rendezname));
  i_entity_refresh_config_apply (self, ENTITY(item->rendezname), &refconfig);
  
  item->failoverpriority = i_metric_create ("failoverpriority", "Failover Priority", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->failoverpriority));
  i_entity_refresh_config_apply (self, ENTITY(item->failoverpriority), &refconfig);
  
  item->fsmpmrunning = i_metric_create ("mountedclients", "fsmpm Running", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->fsmpmrunning));
  i_entity_refresh_config_apply (self, ENTITY(item->fsmpmrunning), &refconfig);
  
  item->fsnameservers = i_metric_create ("fsnameservers", "fsnameservers", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->fsnameservers));
  i_entity_refresh_config_apply (self, ENTITY(item->fsnameservers), &refconfig);
  
  item->cpucount = i_metric_create ("cpucount", "CPU Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->cpucount));
  i_entity_refresh_config_apply (self, ENTITY(item->cpucount), &refconfig);
  
  item->cpuspeed = i_metric_create ("cpuspeed", "CPU Speed", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->cpuspeed));
  i_entity_refresh_config_apply (self, ENTITY(item->cpuspeed), &refconfig);
  
  item->cpukind = i_metric_create ("cpukind", "cpukind", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->cpukind));
  i_entity_refresh_config_apply (self, ENTITY(item->cpukind), &refconfig);
  
  item->ram = i_metric_create ("ram", "ram", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->ram));
  i_entity_refresh_config_apply (self, ENTITY(item->ram), &refconfig);
  
  item->licensevalid = i_metric_create ("licensevalid", "licensevalid", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->licensevalid));
  i_entity_refresh_config_apply (self, ENTITY(item->licensevalid), &refconfig);
  
  item->role = i_metric_create ("role", "role", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->role));
  i_entity_refresh_config_apply (self, ENTITY(item->role), &refconfig);
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_xsancomputer_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

