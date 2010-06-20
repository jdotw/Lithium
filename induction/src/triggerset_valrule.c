#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "hashtable.h"
#include "timer.h"
#include "name.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "triggerset.h"

/** \addtogroup triggerset Trigger Sets
 * @ingroup trigger
 * @{
 */

/* 
 * CEMent Trigggers - Value Rules
 */

/* Struct manipulation */

i_triggerset_valrule* i_triggerset_valrule_create ()
{
  i_triggerset_valrule *rule;

  rule = (i_triggerset_valrule *) malloc (sizeof(i_triggerset_valrule));
  if (!rule) return NULL;
  memset (rule, 0, sizeof(i_triggerset_valrule));

  return rule;
}

void i_triggerset_valrule_free (void *ruleptr)
{
  i_triggerset_valrule *rule = ruleptr;

  if (rule->site_name) free (rule->site_name);
  if (rule->site_desc) free (rule->site_desc);
  if (rule->dev_name) free (rule->dev_name);
  if (rule->dev_desc) free (rule->dev_desc);
  if (rule->obj_name) free (rule->obj_name);
  if (rule->obj_desc) free (rule->obj_desc);
  if (rule->trg_name) free (rule->trg_name);
  if (rule->trg_desc) free (rule->trg_desc);
  if (rule->xval_str) free (rule->xval_str);
  if (rule->yval_str) free (rule->yval_str);

  free (rule);
}

/* Apprule Manipulation */

int i_triggerset_valrule_add (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_valrule *rule)
{
  int num;
  
  /* Insert into SQL */
  num = i_triggerset_valrule_sql_insert (self, obj, tset, rule);
  if (num != 0)
  { i_printf (1, "i_triggerset_valrule_add failed to add new rule to SQL"); return -1; }

  return 0;
}

int i_triggerset_valrule_add_exclusive (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_valrule *rule)
{
  /* Remove any other fully specific match */
  i_triggerset_valrule_sql_delete_specific (self, obj, tset, rule->trg_name);

  /* Add rule normally */
  i_triggerset_valrule_add (self, obj, tset, rule);

  return 0;
}

int i_triggerset_valrule_update (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_valrule *rule)
{
  int num;
  
  /* Update SQL */
  num = i_triggerset_valrule_sql_update (self, tset, rule);
  if (num != 0)
  { i_printf (1, "i_triggerset_valrule_update failed to add new rule to SQL"); return -1; }

  return 0;
}

int i_triggerset_valrule_remove (i_resource *self, i_object *obj, i_triggerset *tset, long rule_id)
{
  int num;

  /* Delete from SQL */
  num = i_triggerset_valrule_sql_delete (self, tset, rule_id);
  if (num != 0)
  { i_printf (1, "i_triggerset_valrule_remove failed to add new rule to SQL"); return -1; }

  return 0;
}

int i_triggerset_valrule_remove_exclusive (i_resource *self, i_entity *ent)
{
  /* Removes all the value rules that apply to the specified
   * entity and all entities below it
   */
  int num;

  /* Delete from SQL */
  num = i_triggerset_valrule_sql_delete_wildcard (self, ent);
  if (num != 0)
  { i_printf (1, "i_triggerset_valrule_remove_exclusive failed to remove rules from SQL"); return -1; }

  return 0;
}
/* @} */
