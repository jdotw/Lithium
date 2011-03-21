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
#include "device/record.h"
#include "device/snmp.h"

#include "cpu.h"

/* HP Procurve CPU Sub-System */

static i_container *static_cnt = NULL;
static i_object *static_obj = NULL;

/* Variable Retrieval */

i_container* v_cpu_cnt ()
{ return static_cnt; }

/* Enable / Disable */

int v_cpu_enable (i_resource *self)
{
  int num;
  static i_entity_refresh_config defrefconfig;
  i_triggerset *tset;

  /* Create/Config Container */
  static_cnt = i_container_create ("hrcpu", "CPU");
  if (!static_cnt)
  { i_printf (1, "v_cpu_enable failed to create container"); v_cpu_disable (self); return -1; }
  static_cnt->mainform_func = v_cpu_cntform;
  static_cnt->sumform_func = v_cpu_cntform;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_cpu_enable failed to register container"); v_cpu_disable (self); return -1; }

  /* Load/Apply refresh config */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_cpu_enable failed to load and apply container refresh config"); v_cpu_disable (self); return -1; }

  /*
   * Trigger Sets 
   */

  /* Capacity */
  tset = i_triggerset_create ("load_pc", "Utilization", "load_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_INTEGER, TRGTYPE_RANGE, 70, NULL, 85, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_INTEGER, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_cpu_enable failed to create item_list"); v_cpu_disable (self); return -1; }
  i_list_set_destructor (static_cnt->item_list, v_cpu_item_free);
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create master object */
  static_obj = i_object_create ("master", "Master");
  if (!static_obj)
  { i_printf (1, "v_cpu_enable failed to create master object"); v_cpu_disable (self); return -1; }
  static_obj->mainform_func = v_cpu_objform;

  /* Create cpu item */
  v_cpu_item *cpu = v_cpu_item_create ();
  if (!cpu)
  { i_printf (1, "v_cpu_enable failed to create cpu item struct"); v_cpu_disable (self); return -1; }
  cpu->obj = static_obj;
  static_obj->itemptr = cpu;

  /* Enqueue Item */
  i_list_enqueue (static_cnt->item_list, cpu);
  if (num != 0)
  { i_printf (1, "v_cpu_enable failed to enqueue item into static_cnt->item_list"); v_cpu_disable (self); return -1; }

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(static_obj));

  /*
   * Metric Creation 
   */

  /* Basic */
  cpu->util_pc = l_snmp_metric_create (self, static_obj, "load_pc", "Utilization", METRIC_GAUGE, "enterprises.11.2.14.11.5.1.9.6.1.0",NULL, RECMETHOD_RRD, 0);
  cpu->util_pc->record_defaultflag = 1;
  cpu->util_pc->min_val = i_metric_value_create ();
  cpu->util_pc->min_val->gauge = 0;
  cpu->util_pc->max_val = i_metric_value_create ();
  cpu->util_pc->max_val->gauge = 100;
  cpu->util_pc->unit_str = strdup ("%");
            
  /* Evaluate apprules for all triggersets */
  i_triggerset_evalapprules_allsets (self, static_obj);

  /* Evaluate recrules for all metrics */
  l_record_eval_recrules_obj (self, static_obj);

  return 0;
}

int v_cpu_disable (i_resource *self)
{
  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

