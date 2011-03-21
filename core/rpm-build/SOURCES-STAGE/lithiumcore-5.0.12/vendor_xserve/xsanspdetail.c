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
#include "induction/name.h"
#include "device/snmp.h"
#include "device/record.h"

#include "xsanspdetail.h"

/* Xraid System Information Sub-System */

static i_list* static_sp_list = NULL;

/* Variable Retrieval */

i_container* v_xsanspdetail_get_cnt (char *vol_name, char *sp_name)
{
  char *name_str;
  i_container *cnt = NULL;
  asprintf (&name_str, "xsanspdetail_%s_%s", vol_name, sp_name);
  for (i_list_move_head(static_sp_list); (cnt=i_list_restore(static_sp_list))!=NULL; i_list_move_next(static_sp_list))
  { if (strcmp(cnt->name_str, name_str) == 0) return cnt; }
  return NULL;
}

v_xsanspdetail_item* v_xsanspdetail_get_node (i_container *cnt, char *node_name)
{
  i_object *obj;
  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  { if (strcmp(obj->name_str, node_name) == 0) return obj->itemptr; }
  return NULL;
}

/* Enable / Disable */

int v_xsanspdetail_enable (i_resource *self)
{
  /* Create array lists */
  static_sp_list = i_list_create ();
  i_list_set_destructor (static_sp_list, v_xsanspdetail_item_free);

  return 0;
}

i_container* v_xsanspdetail_create_cnt (i_resource *self, char *vol_name, char *sp_name)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  char *name_str;
  char *desc_str;
  i_container *cnt;
  
  /* Create/Config Container */
  asprintf (&name_str, "xsanspdetail_%s_%s", vol_name, sp_name);
  asprintf (&desc_str, "Xsan SP: %s", sp_name);
  cnt = i_container_create (name_str, desc_str);
  if (!cnt)
  { i_printf (1, "v_xsanspdetail_create failed to create container"); return NULL; }
//  cnt->mainform_func = v_xsanspdetail_cntform;
//  cnt->sumform_func = v_xsanspdetail_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "v_xsanspdetail_create failed to register container"); return NULL; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_xsanspdetail_create failed to load and apply container refresh config"); return NULL; }

  /* Create item list */
  cnt->item_list = i_list_create ();
  if (!cnt->item_list)
  { i_printf (1, "v_xsanspdetail_create failed to create item_list"); return NULL; }
  cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  i_list_enqueue (static_sp_list, cnt);

  return cnt;
}

v_xsanspdetail_item* v_xsanspdetail_create_node (i_resource *self, i_container *cnt, char *disk_name)
{
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create info object */
  v_xsanspdetail_item *item;

  /* Create table object */
  char *name_str = strdup (disk_name);
  i_name_parse (name_str);
  obj = i_object_create (name_str, disk_name);
  free (name_str);
//  obj->mainform_func = v_ipacct_data_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  /* Create item */
  item = v_xsanspdetail_item_create ();
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

  item->nodediskdevice = i_metric_create ("nodediskdevice", "Node Disk Device", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->nodediskdevice));
  i_entity_refresh_config_apply (self, ENTITY(item->nodediskdevice), &refconfig);    
    
  item->noderotationindex = i_metric_create ("noderotationindex", "Node Rotation Index", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->noderotationindex));
  i_entity_refresh_config_apply (self, ENTITY(item->noderotationindex), &refconfig);    
    
  item->sectors = i_metric_create ("sectors", "Sectors", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->sectors));
  i_entity_refresh_config_apply (self, ENTITY(item->sectors), &refconfig);    
    
  item->sectorsize = i_metric_create ("sectorsize", "Sector Size", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->sectorsize));
  i_entity_refresh_config_apply (self, ENTITY(item->sectorsize), &refconfig);    
    

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);
  
  return item;
}

int v_xsanspdetail_disable (i_resource *self)
{
  /* Deregister container */

  return 0;
}

