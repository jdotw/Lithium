#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/name.h>
#include <induction/entity.h>
#include <induction/value.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>

#include "record.h"

/*
 * Recording rule evaluation
 */

int l_record_eval_recrules (i_resource *self, i_metric *met)
{
  int recordflag;
  i_list *list;
  l_record_recrule *rule;

  /* Load application rules */
  list = l_record_recrule_sql_load_sync (self, met);

  /* Check first (most-valid) rule */
  i_list_move_head (list);
  rule = i_list_restore (list);
  if (rule)
  { recordflag = rule->recordflag; }
  else
  { recordflag = met->record_defaultflag; }
  if (list) i_list_free (list);

  /* Action rule */
  if (recordflag == 1 && met->record_enabled == 0)
  {
    /* Record if not already recording */
    l_record_enable (self, met);
  }
  else if (recordflag == 0 && met->record_enabled == 1)
  { 
    /* Strip trigger if applied */
    l_record_disable (self, met);
    return 0;
  }

  return 0;
}

int l_record_eval_recrules_obj (i_resource *self, i_object *obj)
{
  /* Evaluate all metrics in the object */
  i_metric *met;

  for (i_list_move_head(obj->met_list); (met=i_list_restore(obj->met_list))!=NULL; i_list_move_next(obj->met_list))
  {
    if (met->record_method != 0)
    { l_record_eval_recrules (self, met); }
  }

  return 0;
}
