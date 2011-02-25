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
 * CEMent Trigggers - Application Rules
 */

/* Struct manipulation */

i_triggerset_apprule* i_triggerset_apprule_create ()
{
  i_triggerset_apprule *rule;

  rule = (i_triggerset_apprule *) malloc (sizeof(i_triggerset_apprule));
  if (!rule) return NULL;
  memset (rule, 0, sizeof(i_triggerset_apprule));

  return rule;
}

void i_triggerset_apprule_free (void *ruleptr)
{
  i_triggerset_apprule *rule = ruleptr;

  if (rule->site_name) free (rule->site_name);
  if (rule->site_desc) free (rule->site_desc);
  if (rule->dev_name) free (rule->dev_name);
  if (rule->dev_desc) free (rule->dev_desc);
  if (rule->obj_name) free (rule->obj_name);
  if (rule->obj_desc) free (rule->obj_desc);

  free (rule);
}

/* Apprule Manipulation */

int i_triggerset_apprule_add (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_apprule *rule)
{
  int num;
  
  /* Insert into SQL */
  num = i_triggerset_apprule_sql_insert (self, obj, tset, rule);
  if (num != 0)
  { i_printf (1, "i_triggerset_apprule_add failed to add new rule to SQL"); return -1; }

  return 0;
}

int i_triggerset_apprule_add_exclusive (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_apprule *rule)
{
  /* Remove any other fully specific match */
  i_triggerset_apprule_sql_delete_exclusive (self, obj, tset, rule);

  /* Add rule normally */
  i_triggerset_apprule_add (self, obj, tset, rule);

  return 0;
}

int i_triggerset_apprule_update (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_apprule *rule)
{
  int num;
  
  /* Update SQL */
  num = i_triggerset_apprule_sql_update (self, rule, tset);
  if (num != 0)
  { i_printf (1, "i_triggerset_apprule_update failed to add new rule to SQL"); return -1; }

  return 0;
}

int i_triggerset_apprule_update_exclusive (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_apprule *rule)
{
  /* Remove any other fully specific match */
  i_triggerset_apprule_sql_delete_exclusive (self, obj, tset, rule);

  /* Add rule normally */
  i_triggerset_apprule_update (self, obj, tset, rule);

  return 0;
}

int i_triggerset_apprule_remove (i_resource *self, i_object *obj, i_triggerset *tset, long rule_id)
{
  int num;

  /* Delete from SQL */
  num = i_triggerset_apprule_sql_delete (self, rule_id, tset);
  if (num != 0)
  { i_printf (1, "i_triggerset_apprule_remove failed to add new rule to SQL"); return -1; }

  return 0;
}

int i_triggerset_apprule_remove_exclusive (i_resource *self, i_entity *ent)
{
  /* Delete all apprules for all tsets that apply directly to the 
   * given entity and anything below it 
   */

  /* Delete from SQL */
  int num = i_triggerset_apprule_sql_delete_wildcard (self, ent);
  if (num != 0)
  { i_printf (1, "i_triggerset_apprule_remove_exclusive failed to add new rule to SQL"); return -1; }

  return 0;
}

/* @} */
