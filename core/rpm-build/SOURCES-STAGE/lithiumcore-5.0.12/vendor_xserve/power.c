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
#include "device/snmp.h"
#include "device/record.h"

#include "osx_server.h"
#include "data.h"
#include "power.h"

/* Xraid Enclosure Sub-System */

static i_container *static_cnt = NULL;
static v_power_item *static_item = NULL;

/* Variable Retrieval */

i_container* v_power_cnt ()
{ return static_cnt; }

v_power_item* v_power_static_item ()
{ return static_item; }

/* Enable / Disable */

int v_power_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_object *obj;
  i_entity_refresh_config refconfig;

  /* Create/Config Container */
  static_cnt = i_container_create ("xspower", "Power");
  if (!static_cnt)
  { i_printf (1, "v_power_enable failed to create container"); v_power_disable (self); return -1; }
  static_cnt->mainform_func = v_power_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;
  
  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_power_enable failed to register container"); v_power_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_power_enable failed to load and apply container refresh config"); v_power_disable (self); return -1; }

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_power_enable failed to create item_list"); v_power_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create table object */
  obj = i_object_create ("master", "Master");
  obj->mainform_func = v_power_objform;

  /* Register table object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Create item */
  static_item = v_power_item_create ();
  static_item->obj = obj;
  obj->itemptr = static_item;

  /*
   * Create Metrics 
   */ 
  
  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  static_item->sc_vcore = i_metric_create ("sc_vcore", "System Controller Vcore", METRIC_FLOAT);
  static_item->sc_vcore->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->sc_vcore));
  i_entity_refresh_config_apply (self, ENTITY(static_item->sc_vcore), &refconfig);

  static_item->ddr_io = i_metric_create ("ddr_io", "DDR IO", METRIC_FLOAT);
  static_item->ddr_io->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->ddr_io));
  i_entity_refresh_config_apply (self, ENTITY(static_item->ddr_io), &refconfig);

  static_item->ddr_io_sleep = i_metric_create ("ddr_io_sleep", "DDR IO Sleep", METRIC_FLOAT);
  static_item->ddr_io_sleep->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->ddr_io_sleep));
  i_entity_refresh_config_apply (self, ENTITY(static_item->ddr_io_sleep), &refconfig);

  static_item->io_vdd = i_metric_create ("io_vdd", "System Bus IO VDD", METRIC_FLOAT);
  static_item->io_vdd->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->io_vdd));
  i_entity_refresh_config_apply (self, ENTITY(static_item->io_vdd), &refconfig);

  static_item->v_1_2v = i_metric_create ("v_1_2v", "1.2v", METRIC_FLOAT);
  static_item->v_1_2v->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_1_2v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_1_2v), &refconfig);

  static_item->v_1_2v_sleep = i_metric_create ("v_1_2v_sleep", "1.2v Sleep", METRIC_FLOAT);
  static_item->v_1_2v_sleep->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_1_2v_sleep));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_1_2v_sleep), &refconfig);

  static_item->v_1_5v = i_metric_create ("v_1_5v", "1.5v", METRIC_FLOAT);
  static_item->v_1_5v->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_1_5v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_1_5v), &refconfig);

  static_item->v_1_5v_sleep = i_metric_create ("v_1_5v_sleep", "1.5v Sleep", METRIC_FLOAT);
  static_item->v_1_5v_sleep->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_1_5v_sleep));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_1_5v_sleep), &refconfig);

  static_item->v_1_8v = i_metric_create ("v_1_8v", "1.8v", METRIC_FLOAT);
  static_item->v_1_8v->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_1_8v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_1_8v), &refconfig);

  static_item->v_3_3v = i_metric_create ("v_3_3v", "3.3v", METRIC_FLOAT);
  static_item->v_3_3v->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_3_3v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_3_3v), &refconfig);

  static_item->v_3_3v_sleep = i_metric_create ("v_3_3v_sleep", "3.3v Sleep", METRIC_FLOAT);
  static_item->v_3_3v_sleep->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_3_3v_sleep));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_3_3v_sleep), &refconfig);

  static_item->v_3_3v_trickle = i_metric_create ("v_3_3v_trickle", "3.3v Trickle", METRIC_FLOAT);
  static_item->v_3_3v_trickle->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_3_3v_trickle));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_3_3v_trickle), &refconfig);

  static_item->v_5v = i_metric_create ("v_5v", "5v", METRIC_FLOAT);
  static_item->v_5v->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_5v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_5v), &refconfig);

  static_item->v_5v_sleep = i_metric_create ("v_5v_sleep", "5v Sleep", METRIC_FLOAT);
  static_item->v_5v_sleep->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_5v_sleep));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_5v_sleep), &refconfig);

  static_item->v_12v = i_metric_create ("v_12v", "12v", METRIC_FLOAT);
  static_item->v_12v->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_12v));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_12v), &refconfig);

  static_item->v_12v_trickle = i_metric_create ("v_12v_trickle", "12v Trickle", METRIC_FLOAT);
  static_item->v_12v_trickle->unit_str = strdup ("v");
  i_entity_register (self, ENTITY(obj), ENTITY(static_item->v_12v_trickle));
  i_entity_refresh_config_apply (self, ENTITY(static_item->v_12v_trickle), &refconfig);
  
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, obj);
  
  return 0;
}

int v_power_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

