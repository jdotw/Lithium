#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/value.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/snmp.h"
#include "device/record.h"

#include "cpu.h"

/* cpu - CPU Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */

/* Variable Fetching */

i_container* v_cpu_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int v_cpu_enable (i_resource *self)
{
  int num;
  i_triggerset *tset;
  v_cpu_item *cpu;
  i_object *obj;
  static i_entity_refresh_config defrefconfig;

  if (static_enabled == 1)
  { i_printf (1, "v_cpu_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("cpu", "CPU");
  if (!static_cnt)
  { i_printf (1, "v_cpu_enable failed to create container"); v_cpu_disable (self); return -1; }
  static_cnt->mainform_func = v_cpu_cntform;
  static_cnt->sumform_func = v_cpu_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "v_cpu_enable failed to register container"); v_cpu_disable (self); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "v_cpu_enable failed to load and apply container refresh config"); v_cpu_disable (self); return -1; }

  /* 
   * Trigger sets 
   */

  tset = i_triggerset_create ("busy_pc", "Busy Percent", "busy_pc");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 85, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  /* 
   * Items and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "v_cpu_enable failed to create item_list"); v_cpu_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create object */
  obj = i_object_create ("master", "Master");
  if (!obj)
  { i_printf (1, "v_cpu_enable failed to create object"); v_cpu_disable (self); return -1; }
  obj->cnt = static_cnt;
  obj->mainform_func = v_cpu_objform;
  obj->histform_func = v_cpu_objform_hist;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create item */
  cpu = v_cpu_item_create ();
  if (!cpu)
  { i_printf (1, "v_cpu_enable failed to create item"); v_cpu_disable (self); return -1; }
  cpu->obj = obj;
  obj->itemptr = cpu;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Set refresh params */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* Busy Metric */
  cpu->busy_pc = l_snmp_metric_create (self, obj, "busy_pc", "Busy Percent", METRIC_INTEGER, "enterprises.789.1.2.1.3", "0", RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  cpu->busy_pc->unit_str = strdup ("%");
  cpu->busy_pc->record_defaultflag = 1;

  /* CPU Count Metric */
  cpu->cpu_count = l_snmp_metric_create (self, obj, "cpu_count", "CPU Count", METRIC_INTEGER, "enterprises.789.1.2.1.6", "0", RECMETHOD_NONE, SMET_PARENTREFMETHOD);

  /* Load/Apply refresh configuration for the object */
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &defrefconfig);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);
  
  /* Apply all triggersets */
  num = i_triggerset_evalapprules_allsets (self, obj);
  if (num != 0)
  { i_printf (1, "v_cpu_enable warning, failed to apply all triggersets for master object"); }

  /* Enqueue the cpu item */
  i_list_enqueue (static_cnt->item_list, cpu);

  return 0;
}

int v_cpu_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "v_cpu_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

