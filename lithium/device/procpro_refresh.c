#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>
#include <regex.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/socket.h>
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
#include <induction/postgresql.h>
#include <induction/path.h>
#include <induction/xml.h>

#include "snmp_swrun.h"
#include "procpro.h"

/*
 * Process profile refreshing
 */

int l_procpro_refresh (i_resource *self, i_object *obj, int opcode)
{
  l_procpro *procpro = obj->itemptr;

  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      l_procpro_refresh_procpro (self, procpro);
      return 1;
      break;

    case REFOP_COLLISION:   /* Handle collision */
      break;

    case REFOP_TERMINATE:   /* Terminate the refresh */
      break;

    case REFOP_CLEANDATA:   /* Cleanup persistent refresh data */
      break;
  }

  return 0;
}

int l_procpro_refresh_procpro (i_resource *self, l_procpro *procpro)
{
  /* Compile regexp */
  regex_t exp;
  int num = regcomp (&exp, procpro->match_str, REG_EXTENDED);
  if (num != 0)
  { i_printf (1, "l_procpro_refresh_procpro failed to compile regexp '%s'", procpro->match_str); return -1; }

  /* Compile arguments match regexp */
  int match_arguments = 0;
  regex_t argexp;
  if (procpro->argmatch_str && strlen(procpro->argmatch_str) > 0)
  {
    int num = regcomp (&argexp, procpro->argmatch_str, REG_EXTENDED);
    if (num != 0)
    { i_printf (1, "l_procpro_refresh_procpro failed to compile arguments matching regexp '%s'", procpro->argmatch_str); return -1; }
    match_arguments = 1;

  }

  /* DEBUG */
  i_printf(0, "l_procpro_refresh_procpro commencing refresh for procpro %s (match=%s, args=%s) with %i objects in swrun container", procpro->desc_str, procpro->match_str, procpro->argmatch_str, swrun_cnt->obj_list ? swrun_cnt->obj_list->size : 0);
  /* END DEBUG */

  /* Variables */
  int proc_count = 0;
  float mem_total = 0.0;
  float cpu_total = 0.0;
  float mem_maxsingle = 0.0;
  float cpu_maxsingle = 0.0;

  /* Loop through each process in the swrun table */
  i_container *swrun_cnt = l_snmp_swrun_cnt ();
  i_object *proc_obj;
  for (i_list_move_head(swrun_cnt->obj_list); (proc_obj=i_list_restore(swrun_cnt->obj_list))!=NULL; i_list_move_next(swrun_cnt->obj_list))
  {
    /* Get process name */
    l_snmp_swrun *proc_item = (l_snmp_swrun *) proc_obj->itemptr;
    if (!proc_item) 
    {
      i_printf (1, "l_procpro_refresh_procpro error, process %s doesnt have an proc_item and has been skipped", proc_obj->name_str);
      continue;
    }
    char *proc_name = i_metric_valstr (proc_item->procname, NULL);
    if (!proc_name) 
    {
      i_printf (1, "l_procpro_refresh_procpro warning, process %s doesnt have an proc_name, skipped", proc_obj->name_str);
      continue; 
    }
    char *proc_args = i_metric_valstr (proc_item->params, NULL);
    if (!proc_args && match_arguments == 1) 
    {
      i_printf (1, "l_procpro_refresh_procpro warning, process %s doesnt have any proc_params but argument matching is enabled, skipped", proc_obj->name_str);
      continue; 
    }

    /* DEBUG */
    i_printf(0, "l_procpro_refresh_procpro matching process %s against profile match string %s", proc_name, procpro->match_str);
    /* END DEBUG */

    /* Check for a match in the process name */
    regmatch_t match[1];
    num = regexec (&exp, proc_name, 1, match, 0);
    if (num == 0)
    {
      /* Process Name Matches, check or arguments */
      if (match_arguments == 0 || regexec (&argexp, proc_args, 1, match, 0) == 0)
      {
        /* Process Name and optional arguments check matches */
        l_snmp_swrun *proc = (l_snmp_swrun *) proc_obj->itemptr;
        proc_count++;

        /* Check CPU */
        float cpu_pc = i_metric_valflt (proc->cpu_pc, NULL);
        if (cpu_pc > cpu_maxsingle) cpu_maxsingle = cpu_pc;
        cpu_total = cpu_total + cpu_pc;

        /* Check Mem */
        float mem = i_metric_valflt (proc->mem, NULL);
        if (mem > mem_maxsingle) mem_maxsingle = mem;
        mem_total = mem_total + mem;
      }
    }

    /* Clean up */
    free (proc_name);
    free (proc_args);
  }

  /* Clean up */
  regfree (&exp);

  /* 
   * Update Metrics
   */

  /* Status */
  i_metric_value *val = i_metric_value_create ();
  val->integer = proc_count ? 1 : 0;
  i_metric_value_enqueue (self, procpro->status_met, val);
  procpro->status_met->refresh_result = REFRESULT_OK;
  procpro->status_met->summary_flag = 1;
  i_entity_refresh_terminate (ENTITY(procpro->status_met));

  /* Process Count */
  val = i_metric_value_create ();
  i_metric_valflt_set (procpro->count_met, val, (float) proc_count);
  i_metric_value_enqueue (self, procpro->count_met, val);
  procpro->count_met->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(procpro->count_met));

  /* Highest Process Count */
  float highest_proc_count = i_metric_valflt (procpro->highest_count_met, NULL);
  val = i_metric_value_create ();
  if (proc_count > highest_proc_count)
  { i_metric_valflt_set (procpro->highest_count_met, val, proc_count); }
  else
  { i_metric_valflt_set (procpro->highest_count_met, val, highest_proc_count); }
  i_metric_value_enqueue (self, procpro->highest_count_met, val);
  procpro->highest_count_met->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(procpro->highest_count_met));

  /* CPU Total */
  val = i_metric_value_create ();
  i_metric_valflt_set (procpro->cpu_total_met, val, (float) cpu_total);
  i_metric_value_enqueue (self, procpro->cpu_total_met, val);
  procpro->cpu_total_met->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(procpro->cpu_total_met));

  /* CPU Max. Single */
  val = i_metric_value_create ();
  i_metric_valflt_set (procpro->cpu_maxsingle_met, val, (float) cpu_maxsingle);
  i_metric_value_enqueue (self, procpro->cpu_maxsingle_met, val);
  procpro->cpu_maxsingle_met->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(procpro->cpu_maxsingle_met));

  /* Mem Total */
  val = i_metric_value_create ();
  i_metric_valflt_set (procpro->mem_total_met, val, (float) mem_total);
  i_metric_value_enqueue (self, procpro->mem_total_met, val);
  procpro->mem_total_met->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(procpro->mem_total_met));

  /* Mem Max. Single */
  val = i_metric_value_create ();
  i_metric_valflt_set (procpro->mem_maxsingle_met, val, (float) mem_maxsingle);
  i_metric_value_enqueue (self, procpro->mem_maxsingle_met, val);
  procpro->mem_maxsingle_met->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(procpro->mem_maxsingle_met));

  /* DEBUG */
  i_printf(0, "l_procpro_refresh_procpro finished refresh for procpro %s", procpro->desc_str);
  /* END DEBUG */

  return 0;
}
