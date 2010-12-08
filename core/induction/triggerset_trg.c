#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "name.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "value.h"
#include "trigger.h"
#include "triggerset.h"

/** \addtogroup triggerset Trigger Sets
 * @ingroup trigger
 * @{
 */

/* 
 * CEMent Triggger Sets - Trigger Manipulation
 *
 * The addtrg and removetrg functions add 
 * and remove individual triggers from 
 * trigger sets.
 */

int i_triggerset_addtrg (i_resource *self, i_triggerset *tset, char *name_str, char *desc_str, unsigned short val_type, unsigned short trg_type, double valflt, char *valstr, double yvalflt, char *yvalstr, time_t duration, unsigned short effect, unsigned short flags)
{
  /* This function creates a new trigger
   * with a default value as specified.
   */

  int num;
  i_object *obj;
  i_trigger *trg;

  /* Create trigger */
  trg = i_trigger_create (name_str, desc_str, val_type, trg_type, effect);
  if (!trg)
  { i_printf (1, "i_triggerset_addtrg failed to create trg struct"); return -1; }

  /* Set trigger duration (default if not set) */
  if (duration == 0)
  { trg->duration_sec = tset->default_duration; }
  else
  { trg->duration_sec = duration; }
  
  /* Load/Apply Value */
  if ((flags & TSET_FLAG_VALLOADAPPLY) || (flags & TSET_FLAG_VALAPPLY))
  {
    /* A default value is specified. Convert the 
     * string/float into a value according to
     * the val_type
     */
    i_value *val = NULL;
    i_value *yval = NULL;

    if (i_value_isnum (val_type) == 1)
    {
      /* Numeric Value */
      val = i_value_valflt_set (val_type, NULL, valflt);
      if (trg_type == TRGTYPE_RANGE)
      { yval = i_value_valflt_set (val_type, NULL, yvalflt); }
    }
    else
    {
      /* String Value */
      if (valstr)
      { val = i_value_valstr_set (val_type, NULL, valstr); }
      if (yvalstr)
      { yval = i_value_valstr_set (val_type, NULL, yvalstr); }
    }

    if (flags & TSET_FLAG_VALLOADAPPLY)
    {
      /* Value is to be applied without a load being attempted */
      i_trigger_value_loadapply (self, trg, val, yval); 
    }
    else if (flags & TSET_FLAG_VALAPPLY)
    {
      /* Value it to be used as a default if a specific value can not be loaded */ 
      i_trigger_value_apply (self, trg, val, yval); 
      val = NULL;           /* Val is now applied to the trg, NULLify it so it wont be freed */
      yval = NULL;
    }

    if (val) i_value_free (val);
    if (yval) i_value_free (yval);
  }

  /* Enqueue into tset */
  num = i_list_enqueue (tset->trg_list, trg);
  if (num != 0)
  { 
    i_printf (1, "i_triggerset_addtrg failed to enqueue trg %s into triggerset %s", trg->name_str, tset->name_str); 
    i_entity_free (trg);
    return -1;
  }

  /* Register this trigger to the applicable metrics
   * registered to all objects to which this triggerset
   * has already been applied to
   */
  for (i_list_move_head(tset->obj_list); (obj=i_list_restore(tset->obj_list))!=NULL; i_list_move_next(tset->obj_list))
  { 
    num = i_triggerset_apply_trg (self, obj, NULL, tset, trg);
    if (num != 0)
    { i_printf (1, "i_triggerset_addtrg warning, failed to apply trigger %s to object %s", trg->name_str, obj->name_str); }
  }

  return 0;
}

int i_triggerset_removetrg (i_resource *self, i_triggerset *tset, char *name_str)
{
  int num;
  i_trigger *trg;
  i_object *obj;

  /* Loop through trigger list */
  for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list))
  {
    /* Compare trigger name with name_str */
    if (!strcmp(trg->name_str, name_str))
    {
      /* Match found */
      break;
    }
  }

  /* Check if a trigger was found */
  if (!trg)
  { return -1; }

  /* Strip trigger from the applicable metrics registered
   * to all objects which this triggerset is applied to
   */

  for (i_list_move_head(tset->obj_list); (obj=i_list_restore(tset->obj_list))!=NULL; i_list_move_next(tset->obj_list))
  {
    num = i_triggerset_strip_trg (self, obj, NULL, tset, trg);
    if (num != 0)
    { i_printf (1, "i_triggerset_removetrg warning, failed to strip trigger %s from object %s", trg->name_str, obj->name_str); }
  }

  /* Remove trigger from list */
  i_list_delete (tset->trg_list);
      
  return 0;
}

/* @} */
