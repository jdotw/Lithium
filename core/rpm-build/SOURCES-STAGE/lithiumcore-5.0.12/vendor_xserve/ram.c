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
#include "ram.h"

/* OS X / Xserve RAM Info */

static i_container *static_cnt = NULL;

/* Variable Retrieval */

i_container* v_ram_cnt ()
{ return static_cnt; }

v_ram_item* v_ram_get (char *desc_str)
{
  v_ram_item *item;
  for (i_list_move_head(static_cnt->item_list); (item=i_list_restore(static_cnt->item_list))!=NULL; i_list_move_next(static_cnt->item_list))
  {
    if (strcmp(item->obj->desc_str, desc_str) == 0)
    { return item; }
  }

  return NULL;
}

/* Enable / Disable */

int v_ram_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;

  /* Create/Config Container */
  if (v_xserve_intel_extras())
  { static_cnt = i_container_create ("xsiram", "RAM"); }
  else
  { static_cnt = i_container_create ("xsram", "RAM"); }
  if (!static_cnt)
  { i_printf (1, "v_ram_enable failed to create container"); v_ram_disable (self); return -1; }
  static_cnt->mainform_func = v_ram_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;
  
  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_ram_enable failed to register container"); v_ram_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_ram_enable failed to load and apply container refresh config"); v_ram_disable (self); return -1; }

  /*
   * Triggers
   */
  i_triggerset *tset;

  tset = i_triggerset_create ("err_errors", "ECC Errors", "ecc_errors");
  i_triggerset_addtrg (self, tset, "present", "Present", VALTYPE_COUNT, TRGTYPE_GT, 0, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);


  /* 
   * Item and objects 
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_ram_enable failed to create item_list"); v_ram_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  return 0;
}

v_ram_item* v_ram_create (i_resource *self, char *desc_str)
{
  char *name_str;
  v_ram_item *item;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Set name/desc */
  name_str = strdup (desc_str);
  i_name_parse (name_str);

  /* Create object */
  obj = i_object_create (name_str, desc_str);
  free (name_str);
  obj->mainform_func = v_ram_objform;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  item = v_ram_item_create ();
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

  item->size = i_metric_create ("size", "Size", METRIC_GAUGE);
  item->size->unit_str = strdup ("Mb");
  i_entity_register (self, ENTITY(obj), ENTITY(item->size));
  i_entity_refresh_config_apply (self, ENTITY(item->size), &refconfig);

  item->speed = i_metric_create ("speed", "Speed", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->speed));
  i_entity_refresh_config_apply (self, ENTITY(item->speed), &refconfig);

  item->type = i_metric_create ("Type", "Type", METRIC_STRING);
  i_entity_register (self, ENTITY(obj), ENTITY(item->type));
  i_entity_refresh_config_apply (self, ENTITY(item->type), &refconfig);

  if (v_xserve_intel_extras())
  {
    item->temp = i_metric_create ("temp", "Temperature", METRIC_FLOAT);
    item->temp->unit_str = strdup ("deg.C");
    item->temp->record_method = RECMETHOD_RRD;
    item->temp->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->temp));
    i_entity_refresh_config_apply (self, ENTITY(item->temp), &refconfig);

    item->ecc_errors = i_metric_create ("ecc_errors", "ECC Errors", METRIC_COUNT);
    item->ecc_errors->record_method = RECMETHOD_RRD;
    item->ecc_errors->record_defaultflag = 1;
    i_entity_register (self, ENTITY(obj), ENTITY(item->ecc_errors));
    i_entity_refresh_config_apply (self, ENTITY(item->ecc_errors), &refconfig);
  }
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);

  return item;
}

int v_ram_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}
