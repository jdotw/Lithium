#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/name.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "record.h"

/* 
 * Recording Rules
 */

/* Struct manipulation */

l_record_recrule* l_record_recrule_create ()
{
  l_record_recrule *rule;

  rule = (l_record_recrule *) malloc (sizeof(l_record_recrule));
  if (!rule) return NULL;
  memset (rule, 0, sizeof(l_record_recrule));

  return rule;
}

void l_record_recrule_free (void *ruleptr)
{
  l_record_recrule *rule = ruleptr;

  if (rule->site_name) free (rule->site_name);
  if (rule->site_desc) free (rule->site_desc);
  if (rule->dev_name) free (rule->dev_name);
  if (rule->dev_desc) free (rule->dev_desc);
  if (rule->obj_name) free (rule->obj_name);
  if (rule->obj_desc) free (rule->obj_desc);
  if (rule->met_name) free (rule->met_name);
  if (rule->met_desc) free (rule->met_desc);

  free (rule);
}

/* Apprule Manipulation */

int l_record_recrule_add (i_resource *self, i_metric *met, l_record_recrule *rule)
{
  int num;
  
  /* Insert into SQL */
  num = l_record_recrule_sql_insert (self, met->obj->cnt, rule);
  if (num != 0)
  { i_printf (1, "l_record_recrule_add failed to add new rule to SQL"); return -1; }

  /* Evaluate */
  num = l_record_eval_recrules (self, met);
  if (num != 0)
  { i_printf (1, "l_record_recrule_add failed to evaluate recrules for metric"); return -1; }

  return 0;
}

int l_record_recrule_update (i_resource *self, i_metric *met, l_record_recrule *rule)
{
  int num;
  
  /* Update SQL */
  num = l_record_recrule_sql_update (self, rule);
  if (num != 0)
  { i_printf (1, "l_record_recrule_update failed to add new rule to SQL"); return -1; }

  /* Evaluate */
  num = l_record_eval_recrules (self, met);
  if (num != 0)
  { i_printf (1, "l_record_recrule_add failed to evaluate recrules for metric"); return -1; }

  return 0;
}

int l_record_recrule_remove (i_resource *self, i_metric *met, long rule_id)
{
  int num;

  /* Delete from SQL */
  num = l_record_recrule_sql_delete (self, rule_id);
  if (num != 0)
  { i_printf (1, "l_record_recrule_remove failed to add new rule to SQL"); return -1; }

  /* Evaluate */
  num = l_record_eval_recrules (self, met);
  if (num != 0)
  { i_printf (1, "l_record_recrule_remove failed to evaluate recrules for metric"); return -1; }

  return 0;
}
