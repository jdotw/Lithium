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
#include <induction/name.h>
#include "device/snmp.h"
#include "device/record.h"

#include "xsan.h"
#include "xsanvol.h"
#include "xsansp.h"
#include "xsannode.h"

/* Xraid System Information Sub-System */

/* Variable Retrieval */

static i_list* static_node_list = NULL;

v_xsannode_item* v_xsannode_get (i_container *node_cnt, char *node_name)
{
  i_object *obj;
  for (i_list_move_head(node_cnt->obj_list); (obj=i_list_restore(node_cnt->obj_list))!=NULL; i_list_move_next(node_cnt->obj_list))
  { if (strcmp(obj->desc_str, node_name) == 0) return obj->itemptr; }
  return NULL;
}

v_xsannode_item* v_xsannode_get_global (char *node_name)
{
  v_xsannode_item *item;;
  for (i_list_move_head(static_node_list); (item=i_list_restore(static_node_list))!=NULL; i_list_move_next(static_node_list))
  { if (strcmp(item->obj->desc_str, node_name) == 0) return item; }
  return NULL;
}

v_xsannode_item* v_xsannode_create (i_resource *self, v_xsansp_item *sp, char *node_name)
{
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create info object */
  v_xsannode_item *item;

  /* Create table object */
  char *name_str = strdup (node_name);
  i_name_parse (name_str);
  obj = i_object_create (name_str, node_name);
  free (name_str);

  /* Register table object */
  i_entity_register (self, ENTITY(sp->node_cnt), ENTITY(obj));

  /* Create item */
  item = v_xsannode_item_create ();
  item->obj = obj;
  item->cnt = sp->node_cnt;
  item->sp_item = sp;
  obj->itemptr = item;

  /* Add to list */
  if (!static_node_list) static_node_list = i_list_create ();
  i_list_enqueue (static_node_list, item);

  /*
   * Triggers
   */
  
  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  item->noderotationindex = i_metric_create ("noderotationindex", "Node Rotation Index", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->noderotationindex));
  i_entity_refresh_config_apply (self, ENTITY(item->noderotationindex), &refconfig);    
    
  item->blockdev = i_metric_create ("blockdev", "Block Device", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->blockdev));
  i_entity_refresh_config_apply (self, ENTITY(item->blockdev), &refconfig);    
    
  item->hostnumber = i_metric_create ("hostnumber", "Host Number", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->hostnumber));
  i_entity_refresh_config_apply (self, ENTITY(item->hostnumber), &refconfig);    
    
  item->hostinquiry= i_metric_create ("hostinquiry", "Host Inquiry", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->hostinquiry));
  i_entity_refresh_config_apply (self, ENTITY(item->hostinquiry), &refconfig);    
    
  item->lun = i_metric_create ("lun", "LUN", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun));
  i_entity_refresh_config_apply (self, ENTITY(item->lun), &refconfig);    

  item->maxsectors = i_metric_create ("maxsectors", "Max Sectors", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->maxsectors));
  i_entity_refresh_config_apply (self, ENTITY(item->maxsectors), &refconfig);    
    
  item->sectorsize = i_metric_create ("sectorsize", "Sector Size", METRIC_GAUGE);
  i_entity_register (self, ENTITY(obj), ENTITY(item->sectorsize));
  i_entity_refresh_config_apply (self, ENTITY(item->sectorsize), &refconfig);    
    
  item->wwn = i_metric_create ("wwn", "WWN", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->wwn));
  i_entity_refresh_config_apply (self, ENTITY(item->wwn), &refconfig);    
    
  item->size = i_metric_create ("size", "Size", METRIC_GAUGE);
  if (v_xsan_version() == 2)
  {
    item->size->alloc_unit_met = item->sectorsize;
    item->size->valstr_func = i_string_volume_metric;
    item->size->unit_str = strdup ("byte");
    item->size->kbase = 1024;
  }
    
  i_entity_register (self, ENTITY(obj), ENTITY(item->size));
  i_entity_refresh_config_apply (self, ENTITY(item->size), &refconfig);    
    
  item->unlabeled = i_metric_create ("unlabeled", "Unlabeled", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->unlabeled));
  i_entity_refresh_config_apply (self, ENTITY(item->unlabeled), &refconfig);    
    
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);
  
  return item;
}

