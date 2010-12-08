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
#include "device/snmp.h"
#include "device/record.h"

#include "osx_server.h"
#include "data.h"
#include "volumes.h"

/* OS X / Xserve Volumes Info */

static i_container *static_cnt = NULL;

/* Variable Retrieval */

i_container* v_volumes_cnt ()
{ return static_cnt; }

v_volumes_item* v_volumes_get (char *desc_str)
{
  v_volumes_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->desc_str, desc_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_volumes_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xvolumes", "Volumes");
  if (!static_cnt)
  { i_printf (1, "v_volumes_enable failed to create container"); v_volumes_disable (self); return -1; }
  static_cnt->mainform_func = v_volumes_cntform;
  static_cnt->sumform_func = v_volumes_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_volumes_enable failed to register container"); v_volumes_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_volumes_enable failed to load and apply container refresh config"); v_volumes_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  tset = i_triggerset_create ("used_pc", "Percent Used", "used_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 97, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 97, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
  
  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_volumes_enable failed to create item_list"); v_volumes_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  return 0;
}

v_volumes_item* v_volumes_create (i_resource *self, char *name)
{
  char *name_str;
  char *desc_str;
  v_volumes_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Set name/desc */
  name_str = strdup (name);
  i_name_parse (name_str);
  desc_str = strdup (name);

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  free (name_str);
  free (desc_str);
  obj->mainform_func = v_volumes_objform;
  obj->histform_func = v_volumes_objform_hist;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_volumes_item_create ();
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

  item->bytes_free = i_metric_create ("bytes_free", "Free Bytes", METRIC_GAUGE);
  item->bytes_free->alloc_unit = (1024 * 1024);
  item->bytes_free->unit_str = strdup ("byte");
  item->bytes_free->valstr_func = i_string_volume_metric;
  item->bytes_free->record_method = RECMETHOD_RRD;
  item->bytes_free->record_defaultflag = 1;
  i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_free));
  i_entity_refresh_config_apply (self, ENTITY(item->bytes_free), &refconfig);

  item->bytes_total = i_metric_create ("bytes_total", "Total Bytes", METRIC_GAUGE);
  item->bytes_total->alloc_unit = (1024 * 1024);
  item->bytes_total->unit_str = strdup ("byte");
  item->bytes_total->valstr_func = i_string_volume_metric;
  i_entity_register (self, ENTITY(obj), ENTITY(item->bytes_total));
  i_entity_refresh_config_apply (self, ENTITY(item->bytes_total), &refconfig);

  item->bytes_used = i_metric_acdiff_create (self, obj, "bytes_used", "Used Bytes", METRIC_GAUGE, RECMETHOD_RRD, item->bytes_total, item->bytes_free, ACDIFF_REFCB_YMET);
  item->bytes_used->unit_str = strdup ("byte");
  item->bytes_used->alloc_unit = (1024 * 1024);
  item->bytes_used->valstr_func = i_string_volume_metric;
  
  item->used_pc = i_metric_acpcent_create (self, obj, "used_pc", "Used Percent", RECMETHOD_RRD, item->bytes_used, item->bytes_total, ACPCENT_REFCB_GAUGE);
  item->used_pc->record_defaultflag = 1;
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_volumes_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

