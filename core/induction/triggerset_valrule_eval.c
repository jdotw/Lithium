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
#include "value.h"
#include "adminstate.h"
#include "trigger.h"
#include "triggerset.h"

/** \addtogroup triggerset Trigger Sets
 * @ingroup trigger
 * @{
 */

/* 
 * CEMent Trigggers - Value Rules
 */

/* Rule evaluation */

int i_triggerset_evalvalrules (i_resource *self, i_object *obj, i_triggerset *tset)
{
  /* Loads all the value rules for the given object and triggerset
   * and then applied the value rules to the triggers 
   */

  i_metric *met;
  i_trigger *tset_trg;

  /* Get metric */
  met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
  if (!met) return -1;

  /* Loop through triggers */
  for (i_list_move_head(tset->trg_list); (tset_trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
  {
    i_list *rules;
    i_trigger *trg;
    i_triggerset_valrule *rule;

    /* Get trigger */
    trg = (i_trigger *) i_entity_child_get (ENTITY(met), tset_trg->name_str);
    if (!trg) continue;

    /* Get rules */
    rules = i_triggerset_valrule_sql_load_sync (self, tset, obj, trg);

    /* Get most applicable rule */
    i_list_move_head (rules);
    rule = i_list_restore (rules);
    if (!rule) { i_list_free (rules); continue; }

    /* Apply values */
    if (rule->xval_str) trg->val = i_value_valstr_set (trg->val_type, trg->val, rule->xval_str);
    if (rule->yval_str) trg->yval = i_value_valstr_set (trg->val_type, trg->yval, rule->yval_str);

    /* Set Trigger type */
    trg->trg_type = rule->trg_type;

    /* Set duration */
    trg->duration_sec = rule->duration_sec;

    /* Set adminstate */
    if (trg->adminstate != rule->adminstate)
    { i_adminstate_change (self, ENTITY(trg), rule->adminstate); }

    /* Free rules */
    i_list_free (rules); 
  }
  
  return 0;
}

/* @} */
