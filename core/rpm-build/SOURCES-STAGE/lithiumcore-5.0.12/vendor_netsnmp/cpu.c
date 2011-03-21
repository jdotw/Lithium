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

/* CPU - CPU Usage Sub-System */

static int static_enabled = 0;                        /* 0 = disabled / 1 = enabled */
static i_container *static_cnt = NULL;                /* Cement Container */

/* Variable Fetching */

i_container* l_snmp_nscpu_cnt ()
{ return static_cnt; }

/* Sub-System Enable / Disable */

int l_snmp_nscpu_enable (i_resource *self)
{
  int num;
  i_triggerset *tset;
  l_snmp_nscpu_item *cpu;
  i_object *obj;
  static i_entity_refresh_config defrefconfig;
  static i_entity_refresh_config refconfig;

  if (static_enabled == 1)
  { i_printf (1, "l_snmp_nscpu_enable warning, sub-system already enabled"); return 0; }

  /* Set the enabled flag */
  static_enabled = 1;

  /* Create/Config Container */
  static_cnt = i_container_create ("nscpu", "CPU");
  if (!static_cnt)
  { i_printf (1, "l_snmp_nscpu_enable failed to create container"); l_snmp_nscpu_disable (self); return -1; }
  //static_cnt->mainform_func = l_snmp_nscpu_cntform;
  //static_cnt->sumform_func = l_snmp_nscpu_cntform;
  static_cnt->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Register entity */
  num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_snmp_nscpu_enable failed to register container"); l_snmp_nscpu_disable (self); return -1; }

  /* Normal Handling */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)
  { i_printf (1, "l_snmp_nscpu_enable failed to cpu and apply container refresh config"); l_snmp_nscpu_disable (self); return -1; }

  /* 
   * Trigger sets 
   */

  tset = i_triggerset_create ("user", "User Utilization", "user");
  tset->default_duration = 600;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 95, NULL, 0, NULL, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  tset = i_triggerset_create ("system", "System Utilization", "system");
  tset->default_duration = 600;
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_GT, 95, NULL, 0, NULL, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);
        
  /* 
   * Items and Master Object
   */

  /* Create item list */
  static_cnt->item_list = i_list_create ();
  if (!static_cnt->item_list)
  { i_printf (1, "l_snmp_nscpu_enable failed to create item_list"); l_snmp_nscpu_disable (self); return -1; }
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Create object */
  obj = i_object_create ("master", "Master");
  if (!obj)
  { i_printf (1, "l_snmp_nscpu_enable failed to create object"); l_snmp_nscpu_disable (self); return -1; }
  obj->cnt = static_cnt;
  //obj->mainform_func = l_snmp_nscpu_objform;
  //obj->histform_func = l_snmp_nscpu_objform_hist;
  obj->navtree_expand = NAVTREE_EXP_RESTRICT;

  /* Create item */
  cpu = l_snmp_nscpu_item_create ();
  if (!cpu)
  { i_printf (1, "l_snmp_nscpu_enable failed to create item"); l_snmp_nscpu_disable (self); return -1; }
  cpu->obj = obj;
  obj->itemptr = cpu;

  /* Register object */
  i_entity_register (self, ENTITY(static_cnt), ENTITY(obj));

  /* Set refresh params */
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  /* User */
  cpu->user = l_snmp_metric_create (self, obj, "user", "User", METRIC_INTEGER, ".1.3.6.1.4.1.2021.11.9.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  cpu->user->record_defaultflag = 1;
  cpu->user->unit_str = strdup ("%");
  cpu->user->min_val = i_metric_value_create ();
  cpu->user->min_val->gauge = 0;
  cpu->user->max_val = i_metric_value_create ();
  cpu->user->max_val->gauge = 100;

  /* System */
  cpu->system = l_snmp_metric_create (self, obj, "system", "System", METRIC_INTEGER, ".1.3.6.1.4.1.2021.11.10.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  cpu->system->record_defaultflag = 1;
  cpu->system->unit_str = strdup ("%");
  cpu->system->min_val = i_metric_value_create ();
  cpu->system->min_val->gauge = 0;
  cpu->system->max_val = i_metric_value_create ();
  cpu->system->max_val->gauge = 100;

  /* Idle */
  cpu->idle = l_snmp_metric_create (self, obj, "idle", "Idle", METRIC_INTEGER, ".1.3.6.1.4.1.2021.11.11.0", NULL, RECMETHOD_RRD, SMET_PARENTREFMETHOD);
  cpu->idle->record_defaultflag = 1;
  cpu->idle->unit_str = strdup ("%");
  cpu->idle->min_val = i_metric_value_create ();
  cpu->idle->min_val->gauge = 0;
  cpu->idle->max_val = i_metric_value_create ();
  cpu->idle->max_val->gauge = 100;

  /*
   * Newer Style 
   */

  memset (&refconfig, 0, sizeof(i_entity_refresh_config));
  refconfig.refresh_method = REFMETHOD_EXTERNAL;
  refconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  refconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;

  cpu->user = i_metric_create ("user", "User", METRIC_FLOAT);
  cpu->user->unit_str = strdup ("%");
  cpu->user->record_defaultflag = 1;
  cpu->user->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(cpu->user));
  i_entity_refresh_config_apply (self, ENTITY(cpu->user), &refconfig);

  cpu->nice = i_metric_create ("nice", "Nice", METRIC_FLOAT);
  cpu->nice->unit_str = strdup ("%");
  cpu->nice->record_defaultflag = 1;
  cpu->nice->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(cpu->nice));
  i_entity_refresh_config_apply (self, ENTITY(cpu->nice), &refconfig);

  cpu->system = i_metric_create ("system", "System", METRIC_FLOAT);
  cpu->system->unit_str = strdup ("%");
  cpu->system->record_defaultflag = 1;
  cpu->system->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(cpu->system));
  i_entity_refresh_config_apply (self, ENTITY(cpu->system), &refconfig);

  cpu->idle = i_metric_create ("idle", "Idle", METRIC_FLOAT);
  cpu->idle->unit_str = strdup ("%");
  i_entity_register (self, ENTITY(obj), ENTITY(cpu->idle));
  i_entity_refresh_config_apply (self, ENTITY(cpu->idle), &refconfig);

  cpu->wait = i_metric_create ("wait", "Wait", METRIC_FLOAT);
  cpu->wait->unit_str = strdup ("%");
  cpu->wait->record_defaultflag = 1;
  cpu->wait->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(cpu->wait));
  i_entity_refresh_config_apply (self, ENTITY(cpu->wait), &refconfig);

  cpu->kernel = i_metric_create ("kernel", "Kernel", METRIC_FLOAT);
  cpu->kernel->unit_str = strdup ("%");
  i_entity_register (self, ENTITY(obj), ENTITY(cpu->kernel));
  i_entity_refresh_config_apply (self, ENTITY(cpu->kernel), &refconfig);

  cpu->interrupt = i_metric_create ("interrupt", "Interrupt", METRIC_FLOAT);
  cpu->interrupt->unit_str = strdup ("%");
  cpu->interrupt->record_defaultflag = 1;
  cpu->interrupt->record_method = RECMETHOD_RRD;
  i_entity_register (self, ENTITY(obj), ENTITY(cpu->interrupt));
  i_entity_refresh_config_apply (self, ENTITY(cpu->interrupt), &refconfig);

  cpu->user_ticks = l_snmp_metric_create (self, obj, "user_ticks", "User Ticks", METRIC_COUNT, ".1.3.6.1.4.1.2021.11.50.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(cpu->user_ticks), l_snmp_nscpu_refcb, cpu->user);
  cpu->nice_ticks = l_snmp_metric_create (self, obj, "nice_ticks", "Nice Ticks", METRIC_COUNT, ".1.3.6.1.4.1.2021.11.51.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(cpu->nice_ticks), l_snmp_nscpu_refcb, cpu->nice);
  cpu->system_ticks = l_snmp_metric_create (self, obj, "system_ticks", "System Ticks", METRIC_COUNT, ".1.3.6.1.4.1.2021.11.52.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(cpu->system_ticks), l_snmp_nscpu_refcb, cpu->system);
  cpu->idle_ticks = l_snmp_metric_create (self, obj, "idle_ticks", "Idle Ticks", METRIC_COUNT, ".1.3.6.1.4.1.2021.11.53.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(cpu->idle_ticks), l_snmp_nscpu_refcb, cpu->idle);
  cpu->wait_ticks = l_snmp_metric_create (self, obj, "wait_ticks", "Wait Ticks", METRIC_COUNT, ".1.3.6.1.4.1.2021.11.54.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(cpu->wait_ticks), l_snmp_nscpu_refcb, cpu->wait);
  cpu->kernel_ticks = l_snmp_metric_create (self, obj, "kernel_ticks", "Kernel Ticks", METRIC_COUNT, ".1.3.6.1.4.1.2021.11.55.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(cpu->kernel_ticks), l_snmp_nscpu_refcb, cpu->kernel);
  cpu->interrupt_ticks = l_snmp_metric_create (self, obj, "interrupt_ticks", "Interrupt Ticks", METRIC_COUNT, ".1.3.6.1.4.1.2021.11.56.0", NULL, RECMETHOD_NONE, SMET_PARENTREFMETHOD);
  i_entity_refreshcb_add (ENTITY(cpu->interrupt_ticks), l_snmp_nscpu_refcb, cpu->interrupt);

  /* Load/Apply refresh configuration for the object */
  i_entity_refresh_config_loadapply (self, ENTITY(obj), &defrefconfig);

  /* Evaluate recording rules */
  l_record_eval_recrules_obj (self, obj);
  
  /* Apply all triggersets */
  num = i_triggerset_evalapprules_allsets (self, obj);
  if (num != 0)
  { i_printf (1, "l_snmp_nscpu_enable warning, failed to apply all triggersets for master object"); }

  /*
   * Custom/Combined Graphs
   */
  cpu->cpu_cg = i_metric_cgraph_create (obj, "cpu_cg", "%");
  cpu->cpu_cg->title_str = strdup ("CPU Usage");
  asprintf (&cpu->cpu_cg->render_str, "\"LINE1:met_%s_user_min#000E73:Min.\" \"LINE1:met_%s_user_avg#001EFF:Avg.\" \"LINE1:met_%s_user_max#00B4FF:Max. One Minute Load\" \"GPRINT:met_%s_user_min:MIN:        Min %%.2lf %%%%\" \"GPRINT:met_%s_user_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_user_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_system_min#006B00:Min.\" \"LINE1:met_%s_system_avg#009B00:Avg.\" \"LINE1:met_%s_system_max#00ED00:Max. Five Minute Load\" \"GPRINT:met_%s_system_min:MIN:       Min %%.2lf %%%%\" \"GPRINT:met_%s_system_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_system_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_idle_min#6B0000:Min.\" \"LINE1:met_%s_idle_avg#9B0000:Avg.\" \"LINE1:met_%s_idle_max#ED0000:Max. Fifteen Minute Load\" \"GPRINT:met_%s_idle_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_idle_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_idle_max:MAX: Max. %%.2lf %%%%\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (cpu->cpu_cg->met_list, cpu->user);
  i_list_enqueue (cpu->cpu_cg->met_list, cpu->system);
  i_list_enqueue (cpu->cpu_cg->met_list, cpu->idle);

  /* Enqueue the cpu item */
  i_list_enqueue (static_cnt->item_list, cpu);

  return 0;
}

int l_snmp_nscpu_disable (i_resource *self)
{
  /* Disable the sub-system */

  if (static_enabled == 0)
  { i_printf (1, "l_snmp_nscpu_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  /* Deregister container */
  if (static_cnt)
  { i_entity_deregister (self, ENTITY(static_cnt)); i_entity_free (ENTITY(static_cnt)); static_cnt = NULL; }

  return 0;
}

