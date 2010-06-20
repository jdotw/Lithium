#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "name.h"
#include "entity.h"
#include "value.h"
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
 * CEMent Triggger Sets - Set Application
 *
 * The apply function take a trigger set and an object
 * as arguments and applies the trigger set to the 
 * applicable metric in the object. The apply_all function
 * takes an object as an argument and then calls the apply 
 * function for each trigger set in the parent containers 
 * tset list.
 *
 * The strip function takes a trigger set and an object
 * as arguments and removes any triggers belonging to the
 * trigger set from the applicable metrics in the object.
 * The strip_all function takes an object as an argument 
 * and then calls the strip function for each trigger set 
 * in the parent container's tset list.
 *
 */

/* Trigger Set Application */

int i_triggerset_apply (i_resource *self, i_object *obj, i_triggerset *tset)
{
  /* Apply the specified triggerset to the applicable
   * metric in the object. This function locates the
   * metric referred to by the triggerset's metname_str
   * variable and then assigns each metric in the tset's
   * met_list to the metric
   */

  int num;
  i_metric *met;
  i_trigger *trg;

  /* Fetch metric */
  met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
  if (!met)
  { return -1; }

  /* Add object to tset's obj_list */
  num = i_list_enqueue (tset->obj_list, obj);
  if (num != 0)
  { 
    i_printf (1, "i_triggerset_apply warning, failed to enqueue object %s into trigger set %s's obj_list", 
      obj->name_str, tset->name_str);
  }

  /* Loop through triggers */
  for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
  { 
    num = i_triggerset_apply_trg (self, obj, met, tset, trg);
    if (num != 0)
    { 
      i_printf (1, "i_triggerset_apply warning, failed to apply trigger %s from tset %s to metric %s in object %s",
        trg->name_str, tset->name_str, met->name_str, obj->name_str);
    }
  }

  return 0;
}

int i_triggerset_apply_trg (i_resource *self, i_object *obj, i_metric *met, i_triggerset *tset, i_trigger *trg)
{
  /* This function creates a duplicate of the trigger
   * and registers it to the specified metric belonging
   * to the specified object. If no metric is specified,
   * an i_entity_child_get call will be performed to 
   * retrieve the metric
   */

  int num;
  i_trigger *dup;
  i_trigger *existing;
  i_value *dupval;
  i_value *dupyval = NULL;

  /* Check/Find Metric */
  if (!met)
  { 
    /* Retrieve metric */
    met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
    if (!met) return -1; 
  }

  /* Remove existing trigger */
  existing = (i_trigger *) i_entity_child_get (ENTITY(met), trg->name_str);
  if (existing)
  {
    i_entity_deregister (self, ENTITY(existing));
    i_entity_free (ENTITY(existing));
  }

  /* Duplicate trigger */
  dup = i_trigger_duplicate (trg);

  /* Register the trigger */
  num = i_entity_register (self, ENTITY(met), ENTITY(dup));
  if (num != 0)
  { i_printf (1, "i_triggerset_apply_trg warning, failed to register trigger %s to metric %s", trg->name_str, met->name_str); }

  /* Duplicate the trigger values */
  dupval = i_value_duplicate (trg->val);
  if (trg->yval)
  { dupyval = i_value_duplicate (trg->yval); }

  /* Apply the value */
  num = i_trigger_value_apply (self, dup, dupval, dupyval);
  if (num != 0)
  { 
    i_printf (1, "i_triggerset_apply_trg warning, failed to apply value to trigger %s belonging to metric %s",
      trg->name_str, met->name_str);
    i_value_free (dupval);
  }

  return 0;
}

int i_triggerset_apply_allsets (i_resource *self, i_object *obj)
{
  /* This function loops through all trigger sets
   * assigned to the object's parent container and
   * calls i_triggerset_apply on each set for
   * the specified object. 
   */

  i_container *cnt = obj->cnt;
  i_triggerset *tset;
  
  for (i_list_move_head(cnt->tset_list); (tset=i_list_restore(cnt->tset_list))!=NULL; i_list_move_next (cnt->tset_list))
  {
    /* Apply the trigger set to the object */
    i_triggerset_apply (self, obj, tset);
  }

  return 0;
}

int i_triggerset_apply_allobjs (i_resource *self, i_container *cnt, i_triggerset *tset)
{
  /* This function loops through all objects registered
   * to the container and calls i_triggerset_apply for
   * each object.
   */

  i_object *obj;

  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    /* Apply the triggerset to the object */
    i_triggerset_apply (self, obj, tset); 
  }

  return 0;
}

/* Trigger Set Stripping */

int i_triggerset_strip (i_resource *self, i_object *obj, i_triggerset *tset)
{
  /* This function removes all triggers in the 
   * specified trigger set from the applicable
   * metric in the specified object
   */

  int num;
  i_metric *met;
  i_trigger *trg;

  /* Fetch metric */
  met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
  if (!met)
  { return -1; }

  /* Remove object from tset's obj_list */
  num = i_list_search (tset->obj_list, obj);
  if (num == 0)
  { i_list_delete (tset->obj_list); }
  
  /* Loop through metrics */
  for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
  {
    num = i_triggerset_strip_trg (self, obj, met, tset, trg);
    if (num != 0)
    { 
      i_printf (1, "i_triggerset_strip failed to strip trigger %s in tset %s from metric %s in object %s",
        trg->name_str, tset->name_str, met->name_str, obj->name_str);
    }
  }

  return 0;
}

int i_triggerset_strip_trg (i_resource *self, i_object *obj, i_metric *met, i_triggerset *tset, i_trigger *trg)
{
  /* This function deregisters and frees the trigger 
   * registered to the specified metric with the unique 
   * name of the specified trg. If no metric is specified, 
   * a call to i_entity_child_get will be made to 
   * retrieve the metric.
   */

  i_trigger *dup;

  /* Check/Find Metric */
  if (!met)
  {
    met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
    if (!met)
    { return -1; }
  }

  /* Find the corresponding trigger that has been
   * registered to the metric and deregister/free it
   */
  dup = (i_trigger *) i_entity_child_get (ENTITY(met), trg->name_str);
  if (dup)
  { i_entity_deregister (self, ENTITY(dup)); i_entity_free (dup); }

  return 0;
}

int i_triggerset_strip_allsets (i_resource *self, i_object *obj)
{
  /* This function loops through all trigger sets
   * assigned to the object's parent container and
   * calls i_triggerset_strip on each set for
   * the specified object. 
   */

  i_container *cnt = obj->cnt;
  i_triggerset *tset;

  for (i_list_move_head(cnt->tset_list); (tset=i_list_restore(cnt->tset_list))!=NULL; i_list_move_next (cnt->tset_list))
  {
    /* Strip the trigger set from the object */
    i_triggerset_strip (self, obj, tset);
  }

  return 0;

}

int i_triggerset_strip_allobjs (i_resource *self, i_container *cnt, i_triggerset *tset)
{
  /* This function loops through all objects registered
   * to the container and calls i_triggerset_string for
   * each object.
   */

  i_object *obj;

  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    /* Strip the triggerset from the object */
    i_triggerset_strip (self, obj, tset);
  }

  return 0;
}

/*
 * Apprule evaluation
 */

int i_triggerset_evalapprules (i_resource *self, i_object *obj, i_triggerset *tset)
{
  int applyflag;
  int isapplied;
  i_list *list;
  i_triggerset_apprule *rule;

  /* Load application rules */
  list = i_triggerset_apprule_sql_load_sync (self, tset, obj);

  /* Check first (most-valid) rule */
  i_list_move_head (list);
  rule = i_list_restore (list);
  if (rule)
  { applyflag = rule->applyflag; }
  else
  { applyflag = tset->default_applyflag; }
  if (list) i_list_free (list);

  /* Check if tset is applied */
  if (i_list_search(tset->obj_list, obj) == 0)
  { isapplied = 1; }
  else
  { isapplied = 0; }

  /* Action rule */
  if (applyflag == 1 && isapplied == 0)
  {
    /* Apply trigger, if not already applied */
    i_triggerset_apply (self, obj, tset);

    /* Evaluate Value Rules */
    i_triggerset_evalvalrules (self, obj, tset);

    /* Process trigger against metrics  */
    i_metric *met;
    for (i_list_move_head(obj->met_list); (met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
    { i_trigger_process_all (self, met); }
  }
  else if (applyflag == 0 && isapplied == 1)
  { 
    /* Strip trigger if applied */
    i_triggerset_strip (self, obj, tset);
    
    /* Process trigger against metrics  */
    i_metric *met;
    for (i_list_move_head(obj->met_list); (met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
    { i_trigger_process_all (self, met); }
  }

  return 0;
}

int i_triggerset_evalapprules_allsets (i_resource *self, i_object *obj)
{
  /* This function loops through all trigger sets
   * assigned to the object's parent container and
   * calls i_triggerset_evalapprules on each set for
   * the specified object. 
   */

  i_container *cnt = obj->cnt;
  i_triggerset *tset;

  /* Evaluate object-specific tsets */
  for (i_list_move_head(obj->tset_list); (tset=i_list_restore(obj->tset_list))!=NULL; i_list_move_next (obj->tset_list))
  {
    /* Evaluate the apprules */
    i_triggerset_evalapprules (self, obj, tset);
  } 

  /* Evaluate container-wide tsets */
  for (i_list_move_head(cnt->tset_list); (tset=i_list_restore(cnt->tset_list))!=NULL; i_list_move_next (cnt->tset_list))
  {
    /* Evaluate the apprules */
    i_triggerset_evalapprules (self, obj, tset);
  } 
  
  return 0;
  
}

/* @} */
