#include <stdlib.h>

#include <induction.h>
#include <induction/timer.h>
#include <induction/timeutil.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/str.h>
#include <induction/hierarchy.h>
#include <induction/list.h>

#include <lithium/snmp.h>

#include "cpu.h"

/* 
 * Cisco CPU Resources - Object Factory Functions 
 */

/* Object Factory Fabrication */

int v_cpu_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  int num;
  v_cpu_item *cpu;

  /* Object Configuration */
  obj->desc_str = l_snmp_get_string_from_pdu (pdu);
  obj->mainform_func = v_cpu_objform;
  obj->histform_func = v_cpu_objform_hist;

  /* Load/Apply Refresh config */
  num = i_entity_refresh_config_loadapply (self, ENTITY(obj), NULL);
  if (num != 0)
  { i_printf (1, "v_cpu_objfact_fab failed to load and apply object %s refresh config", obj->name_str); return -1; }

  /* Create cpu item struct */
  cpu = v_cpu_item_create ();
  if (!cpu)
  { i_printf (1, "v_cpu_objfact_fab failed to create cpu item for object %s", obj->name_str); return -1; }
  cpu->obj = obj;
  obj->itemptr = cpu;
  cpu->index = (unsigned long) pdu->variables->name[pdu->variables->name_length-1];

  /* 
   * Metric Creation 
   */

  /* Five Sec */
  if (self->hierarchy->dev->profile_str && strstr(self->hierarchy->dev->profile_str, "pix"))
  {
    /* PIX Handling */
    cpu->fivesec_pc = l_snmp_metric_create (self, obj, "fivesec_pc", "Five Second Load Average", METRIC_GAUGE, "enterprises.9.9.109.1.1.1.1.3", index_oidstr, RECMETHOD_RRD, 0);
  }
  else
  {
    /* Normal Handling */
    cpu->fivesec_pc = l_snmp_metric_create (self, obj, "fivesec_pc", "Five Second Load Average", METRIC_GAUGE, "enterprises.9.9.109.1.1.1.1.6", index_oidstr, RECMETHOD_RRD, 0);
  }
  cpu->fivesec_pc->record_defaultflag = 1;
  cpu->fivesec_pc->min_val = i_metric_value_create ();
  cpu->fivesec_pc->min_val->gauge = 0;
  cpu->fivesec_pc->max_val = i_metric_value_create ();
  cpu->fivesec_pc->max_val->gauge = 100;
  cpu->fivesec_pc->unit_str = strdup ("%");
            
  
  /* One Minutes */
  if (self->hierarchy->dev->profile_str && strstr(self->hierarchy->dev->profile_str, "pix"))
  {
    /* PIX Handling */
    cpu->onemin_pc = l_snmp_metric_create (self, obj, "onemin_pc", "One Minute Load Average", METRIC_GAUGE, "enterprises.9.9.109.1.1.1.1.4", index_oidstr, RECMETHOD_RRD, 0);
  }
  else
  {
    /* Normal Handling */
    cpu->onemin_pc = l_snmp_metric_create (self, obj, "onemin_pc", "One Minute Load Average", METRIC_GAUGE, "enterprises.9.9.109.1.1.1.1.7", index_oidstr, RECMETHOD_RRD, 0);
  }
  cpu->onemin_pc->record_defaultflag = 1;
  cpu->onemin_pc->min_val = i_metric_value_create ();
  cpu->onemin_pc->min_val->gauge = 0;
  cpu->onemin_pc->max_val = i_metric_value_create ();
  cpu->onemin_pc->max_val->gauge = 100;
  cpu->onemin_pc->unit_str = strdup ("%");
  
  /* Five Minutes */
  if (self->hierarchy->dev->profile_str && strstr(self->hierarchy->dev->profile_str, "pix"))
  {
    /* PIX Handling */
    cpu->fivemin_pc = l_snmp_metric_create (self, obj, "fivemin_pc", "Five Minute Load Average", METRIC_GAUGE, "enterprises.9.9.109.1.1.1.1.5", index_oidstr, RECMETHOD_RRD, 0);
  }
  else
  {
    /* Normal Handling */
    cpu->fivemin_pc = l_snmp_metric_create (self, obj, "fivemin_pc", "Five Minute Load Average", METRIC_GAUGE, "enterprises.9.9.109.1.1.1.1.8", index_oidstr, RECMETHOD_RRD, 0);
  }
  cpu->fivemin_pc->record_defaultflag = 1;
  cpu->fivemin_pc->min_val = i_metric_value_create ();
  cpu->fivemin_pc->min_val->gauge = 0;
  cpu->fivemin_pc->max_val = i_metric_value_create ();
  cpu->fivemin_pc->max_val->gauge = 100;
  cpu->fivemin_pc->unit_str = strdup ("%");
  
  /*
   * Custom/Combined Graphs
   */
  cpu->load_cg = i_metric_cgraph_create (obj, "load_cg", "%");
  cpu->load_cg->title_str = strdup ("CPU Load Averages");
  asprintf (&cpu->load_cg->render_str, "\"LINE1:met_%s_fivesec_pc_min#000E73:Min.\" \"LINE1:met_%s_fivesec_pc_avg#001EFF:Avg.\" \"LINE1:met_%s_fivesec_pc_max#00B4FF:Max. Five Second Load\" \"GPRINT:met_%s_fivesec_pc_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_fivesec_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_fivesec_pc_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_onemin_pc_min#006B00:Min.\" \"LINE1:met_%s_onemin_pc_avg#009B00:Avg.\" \"LINE1:met_%s_onemin_pc_max#00ED00:Max. One Minute Load\" \"GPRINT:met_%s_onemin_pc_min:MIN:    Min %%.2lf %%%%\" \"GPRINT:met_%s_onemin_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_onemin_pc_max:MAX: Max. %%.2lf %%%%\\n\" \"LINE1:met_%s_fivemin_pc_min#6B0000:Min.\" \"LINE1:met_%s_fivemin_pc_avg#9B0000:Avg.\" \"LINE1:met_%s_fivemin_pc_max#ED0000:Max. Five Minute Load\" \"GPRINT:met_%s_fivemin_pc_min:MIN:   Min %%.2lf %%%%\" \"GPRINT:met_%s_fivemin_pc_avg:AVERAGE:Avg. %%.2lf %%%%\" \"GPRINT:met_%s_fivemin_pc_max:MAX: Max. %%.2lf %%%%\\n\"",
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str,
    obj->name_str, obj->name_str, obj->name_str);
  i_list_enqueue (cpu->load_cg->met_list, cpu->fivesec_pc);
  i_list_enqueue (cpu->load_cg->met_list, cpu->onemin_pc);
  i_list_enqueue (cpu->load_cg->met_list, cpu->fivemin_pc);
  
  /* Enqueue the cpu item */
  num = i_list_enqueue (cnt->item_list, cpu);
  if (num != 0)
  { i_printf (1, "v_cpu_objfact_fab failed to enqueue cpu for object %s", obj->name_str); v_cpu_item_free (cpu); return -1; }

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_cpu_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  /* Check the result */
  if (result == SNMP_ERROR_NOERROR)
  {
    /* No errors, set item list state to NORMAL */
    cnt->item_list_state = ITEMLIST_STATE_NORMAL;
  }

  return 0;
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_cpu_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  /* FIX needs to free the cpu and remove
   * it from the item_list
   */

  int num;
  v_cpu_item *cpu = obj->itemptr;

  if (!cpu) return 0;

  /* Remove from cnt->item_list */
  num = i_list_search (cnt->item_list, cpu);
  if (num == 0)
  { i_list_delete (cnt->item_list); }
  obj->itemptr = NULL;
  
  return 0;
}
