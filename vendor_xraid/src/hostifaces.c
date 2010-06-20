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

#include "hostifaces.h"

/* Xraid System Information Sub-System */

static i_list* static_top_hostifaces = NULL;
static i_list* static_bottom_hostifaces = NULL;
static int static_array_count = 0;

/* Variable Retrieval */

v_hostifaces_item* v_hostifaces_get_item (int controller, int id)
{
  i_list *array_list;
  if (controller == 0)
  {
    /* Top array */
    array_list = static_top_hostifaces;
  }
  else
  {
    /* Bottom array */
    array_list = static_bottom_hostifaces;
  }

  /* Get the array */
  int i;
  i_list_move_head (array_list);
  for (i=1; i < id; i++)
  {
    i_list_move_next (array_list); 
  }

  return i_list_restore (array_list);
}

/* Enable / Disable */

int v_hostifaces_enable (i_resource *self)
{
  /* Create array lists */
  static_top_hostifaces = i_list_create ();
  i_list_set_destructor (static_top_hostifaces, v_hostifaces_item_free);
  static_bottom_hostifaces = i_list_create ();
  i_list_set_destructor (static_bottom_hostifaces, v_hostifaces_item_free);

  return 0;
}

v_hostifaces_item* v_hostifaces_create (i_resource *self, int controller)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;
  char *name_str;
  char *desc_str;
  i_container *cnt;
  
  /* Increment counter */
  static_array_count++;

  /* Create/Config Container */
  asprintf (&name_str, "xrhostiface_%i", static_array_count);
  asprintf (&desc_str, "Host Interface %i", static_array_count);
  cnt = i_container_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  if (!cnt)
  { i_printf (1, "v_hostifaces_create failed to create container"); return NULL; }
  cnt->mainform_func = v_hostifaces_cntform;
  cnt->sumform_func = v_hostifaces_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));
  if (num != 0)
  { i_printf (1, "v_hostifaces_create failed to register container"); return NULL; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_hostifaces_create failed to load and apply container refresh config"); return NULL; }

  /* Create item list */
  cnt->item_list = i_list_create ();
  if (!cnt->item_list)
  { i_printf (1, "v_hostifaces_create failed to create item_list"); return NULL; }
  cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create info object */
  v_hostifaces_item *item;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_hostifaces_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(cnt), ENTITY(obj));

  /* Create item */
  item = v_hostifaces_item_create ();
  item->obj = obj;
  item->cnt = cnt;
  obj->itemptr = item;
  item->index = static_array_count;

  /*
   * Triggers
   */
  i_triggerset *tset;
  
  /* Link State */
  tset = i_triggerset_create ("linkstate", "Link State", "raid_status");
  i_triggerset_addtrg (self, tset, "down", "Down", VALTYPE_STRING, TRGTYPE_NOTEQUAL, 0, "up", 0, NULL, 0, ENTSTATE_CRITICAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, cnt, tset);

  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  item->linkstate = i_metric_create ("linkstate", "Link State", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->linkstate));
  i_entity_refresh_config_apply (self, ENTITY(item->linkstate), &refconfig);    
  
  item->hardloopid = i_metric_create ("hardloopid", "Hardware Loop ID", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->hardloopid));
  i_entity_refresh_config_apply (self, ENTITY(item->hardloopid), &refconfig);
    
  item->topology = i_metric_create ("topology", "Topology", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->topology));
  i_entity_refresh_config_apply (self, ENTITY(item->topology), &refconfig);
    
  item->actual_topology = i_metric_create ("actual_topology", "Actual Topology", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->actual_topology));
  i_entity_refresh_config_apply (self, ENTITY(item->actual_topology), &refconfig);
    
  item->speed = i_metric_create ("speed", "Speed", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->speed));
  i_entity_refresh_config_apply (self, ENTITY(item->speed), &refconfig);
    
  item->hardloopid_enabled = i_metric_create ("hardloopid_enabled", "Hardware Loop ID Enabled", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->hardloopid_enabled));
  i_entity_refresh_config_apply (self, ENTITY(item->hardloopid_enabled), &refconfig);
    
  item->type = i_metric_create ("type", "Type", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->type));
  i_entity_refresh_config_apply (self, ENTITY(item->type), &refconfig);
    
  item->max_speed = i_metric_create ("max_speed", "Maximum Speed", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->max_speed));
  i_entity_refresh_config_apply (self, ENTITY(item->max_speed), &refconfig);
    
  item->actual_speed = i_metric_create ("actual_speed", "Actual Speed", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->actual_speed));
  i_entity_refresh_config_apply (self, ENTITY(item->actual_speed), &refconfig);

  item->wwn = i_metric_create ("wwn", "World-Wide Name", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->wwn));
  i_entity_refresh_config_apply (self, ENTITY(item->wwn), &refconfig);

  item->lun0_array = i_metric_create ("lun0_array", "LUN 0 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun0_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun0_array), &refconfig);
  item->lun0_slice = i_metric_create ("lun0_slice", "LUN 0 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun0_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun0_slice), &refconfig);

  item->lun1_array = i_metric_create ("lun1_array", "LUN 1 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun1_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun1_array), &refconfig);
  item->lun1_slice = i_metric_create ("lun1_slice", "LUN 1 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun1_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun1_slice), &refconfig);

  item->lun2_array = i_metric_create ("lun2_array", "LUN 2 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun2_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun2_array), &refconfig);
  item->lun2_slice = i_metric_create ("lun2_slice", "LUN 2 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun2_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun2_slice), &refconfig);

  item->lun3_array = i_metric_create ("lun3_array", "LUN 3 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun3_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun3_array), &refconfig);
  item->lun3_slice = i_metric_create ("lun3_slice", "LUN 3 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun3_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun3_slice), &refconfig);

  item->lun4_array = i_metric_create ("lun4_array", "LUN 4 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun4_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun4_array), &refconfig);
  item->lun4_slice = i_metric_create ("lun4_slice", "LUN 4 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun4_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun4_slice), &refconfig);

  item->lun5_array = i_metric_create ("lun5_array", "LUN 5 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun5_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun5_array), &refconfig);
  item->lun5_slice = i_metric_create ("lun5_slice", "LUN 5 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun5_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun5_slice), &refconfig);

  item->lun6_array = i_metric_create ("lun6_array", "LUN 6 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun6_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun6_array), &refconfig);
  item->lun6_slice = i_metric_create ("lun6_slice", "LUN 6 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun6_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun6_slice), &refconfig);

  item->lun7_array = i_metric_create ("lun7_array", "LUN 7 Array", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun7_array));
  i_entity_refresh_config_apply (self, ENTITY(item->lun7_array), &refconfig);
  item->lun7_slice = i_metric_create ("lun7_slice", "LUN 7 Slice", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->lun7_slice));
  i_entity_refresh_config_apply (self, ENTITY(item->lun7_slice), &refconfig);
    
  item->linkfailure_count = i_metric_create ("linkfailure_count", "Link Failure Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->linkfailure_count));
  i_entity_refresh_config_apply (self, ENTITY(item->linkfailure_count), &refconfig);
    
  item->syncloss_count = i_metric_create ("syncloss_count", "Loss of Sync Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->syncloss_count));
  i_entity_refresh_config_apply (self, ENTITY(item->syncloss_count), &refconfig);
    
  item->signalloss_count = i_metric_create ("signalloss_count", "Loss of Signal Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->signalloss_count));
  i_entity_refresh_config_apply (self, ENTITY(item->signalloss_count), &refconfig);
    
  item->protocolerror_count = i_metric_create ("protocolerror_count", "Protocol Error Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->protocolerror_count));
  i_entity_refresh_config_apply (self, ENTITY(item->protocolerror_count), &refconfig);
    
  item->invalidword_count = i_metric_create ("invalidword_count", "Invalid Word Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->invalidword_count));
  i_entity_refresh_config_apply (self, ENTITY(item->invalidword_count), &refconfig);
    
  item->invalidcrc_count = i_metric_create ("invalidcrc_count", "Invalid CRC Count", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->invalidcrc_count));
  i_entity_refresh_config_apply (self, ENTITY(item->invalidcrc_count), &refconfig);
    
  /* Enqueue item */
  if (controller == 0)
  { i_list_enqueue (static_top_hostifaces, item); }
  else
  { i_list_enqueue (static_bottom_hostifaces, item); }

  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);
  
  return item;
}

int v_hostifaces_disable (i_resource *self)
{
  /* Deregister container */

  return 0;
}

