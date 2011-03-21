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
#include "xsanvol.h"
#include "xsansp.h"

/* Xsan Volumes Info */

/* Variable Retrieval */

v_xsansp_item* v_xsansp_get (i_container *sp_cnt, char *desc_str)
{
  v_xsansp_item *item;
  for (i_list_move_head(sp_cnt->item_list); (item=i_list_restore(sp_cnt->item_list))!=NULL; i_list_move_next(sp_cnt->item_list))
  {
    if (strcmp(item->obj->desc_str, desc_str) == 0)
    { return item; }
  }

  return NULL;
}

v_xsansp_item* v_xsansp_create (i_resource *self, v_xsanvol_item *vol, char *name)
{
  char *name_str;
  char *desc_str;
  v_xsansp_item *item;
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

  /* Register object */
  i_entity_register (self, ENTITY(vol->sp_cnt), ENTITY(obj));

  /* Create item */
  item = v_xsansp_item_create ();
  item->obj = obj;
  obj->itemptr = item;
  i_list_enqueue (vol->sp_cnt->item_list, item);

  /*
   * Create Metrics 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* State */

  item->enabled = i_metric_create ("enabled", "Enabled", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->enabled));
  i_entity_refresh_config_apply (self, ENTITY(item->enabled), &refconfig);
  
  item->mirrorindex = i_metric_create ("mirrorindex", "Mirror Index", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->mirrorindex));
  i_entity_refresh_config_apply (self, ENTITY(item->mirrorindex), &refconfig);
  
  item->nativekeyvalue = i_metric_create ("nativekeyvalue", "Native Key Value", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->nativekeyvalue));
  i_entity_refresh_config_apply (self, ENTITY(item->nativekeyvalue), &refconfig);
  
  item->devices = i_metric_create ("devices", "Devices", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->devices));
  i_entity_refresh_config_apply (self, ENTITY(item->devices), &refconfig);
  
  item->realtime = i_metric_create ("realtime", "Real-Time", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->realtime));
  i_entity_refresh_config_apply (self, ENTITY(item->realtime), &refconfig);
  
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

  item->affinity = i_metric_create ("affinity", "Affinity", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->affinity));
  i_entity_refresh_config_apply (self, ENTITY(item->affinity), &refconfig);
  
  item->exclusive = i_metric_create ("exclusive", "Exclusive", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->exclusive));
  i_entity_refresh_config_apply (self, ENTITY(item->exclusive), &refconfig);
  
  item->journal = i_metric_create ("journal", "Journal", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->journal));
  i_entity_refresh_config_apply (self, ENTITY(item->journal), &refconfig);
  
  item->metadata = i_metric_create ("metadata", "Metadata", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->metadata));
  i_entity_refresh_config_apply (self, ENTITY(item->metadata), &refconfig);
  
  item->multipathmethod = i_metric_create ("multipathmethod", "Multi Path Method", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->multipathmethod));
  i_entity_refresh_config_apply (self, ENTITY(item->multipathmethod), &refconfig);
  
  item->read = i_metric_create ("read", "Read", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->read));
  i_entity_refresh_config_apply (self, ENTITY(item->read), &refconfig);
  
  item->write = i_metric_create ("write", "Write", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->write));
  i_entity_refresh_config_apply (self, ENTITY(item->write), &refconfig);
  
  item->status = i_metric_create ("status", "Status", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->status));
  i_entity_refresh_config_apply (self, ENTITY(item->status), &refconfig);
  
  item->stripebreadth = i_metric_create ("stripebreadth", "Stripe Breadth", METRIC_INTEGER);
  i_entity_register (self, ENTITY(obj), ENTITY(item->stripebreadth));
  i_entity_refresh_config_apply (self, ENTITY(item->stripebreadth), &refconfig);
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  /*
   * Create xsansp_<name>_<name> container for Node objects 
   */

  i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  asprintf (&name_str, "xsansplun_%s", obj->name_str);
  i_name_parse (name_str);
  asprintf (&desc_str, "%s LUNs", obj->desc_str);
  i_container *cnt = i_container_create (name_str, desc_str);
  free (name_str);
  name_str = NULL;
  free (desc_str);
  desc_str = NULL;
//  cnt->mainform_func = v_xsanvol_cntform;
//  cnt->sumform_func = v_xsanvol_cntform;
  item->node_cnt = cnt;

  /* Register entity */
  i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(cnt));

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  i_entity_refresh_config_loadapply (self, ENTITY(cnt), &defrefconfig);

  /*
   * Triggers
   */

  /* 
   * Item and objects 
   */

  /* Create item list */
  cnt->item_list = i_list_create ();
  cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  
  return item;
}

