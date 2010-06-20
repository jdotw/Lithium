#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "value.h"
#include "trigger.h"

/** \addtogroup trigger Triggers
 * @ingroup entity
 * @{
 */

/* 
 * CEMent Triggger Evalutation
 */

int i_trigger_eval (i_resource *self, i_trigger *trg, i_metric_value *val)
{
  /* This function evaluates the specified trigger
   * against the specified value for the specified 
   * metric. If not value is specified, the current
   * value of the metric is used.
   *
   * This function return 0 if the trigger is not
   * active, 1 if the trigger IS active and -1 on
   * an error condition. 
   */

  int result = 0;

  /* Check/Obtain Metric Value */
  if (!val)
  { 
    val = i_metric_curval (trg->met);
    if (!val) return -1;
  }

  /* Check Trigger Value */
  if (!trg || !trg->val || ((i_metric_valisnum(trg->met) == 0) && trg->trg_type == TRGTYPE_RANGE && !trg->yval))
  { return -1; }

  /* Type-specified comparison */
  if (i_metric_valisnum(trg->met) == 0)
  {
    /* Numeric Value Comparison */
    double valflt;
    double trgflt;
    double ytrgflt;

    valflt = i_metric_valflt (trg->met, val);
    trgflt = i_value_valflt (trg->val_type, trg->val);

    switch (trg->trg_type)
    {
      case TRGTYPE_EQUAL:
        if (valflt == trgflt) result = 1;
        break;
      case TRGTYPE_LT:
        if (valflt < trgflt) result = 1;
        break;
      case TRGTYPE_GT:
        if (valflt > trgflt) result = 1;
        break;
      case TRGTYPE_NOTEQUAL:
        if (valflt != trgflt) result = 1;
        break;
      case TRGTYPE_RANGE:
        ytrgflt = i_value_valflt (trg->val_type, trg->yval);
        if (valflt >= trgflt && valflt < ytrgflt) result = 1;
        break;
      default:
        i_printf (1, "i_trigger_eval warning, trigger %s for metric %s has an invalid trg->trg_type of %i", 
          trg->name_str, trg->met->name_str, trg->trg_type);
        result = -1;
    }
  }
  else
  {
    /* String Value Comparison */
    char *valstr;
    char *trgstr;

    valstr = i_metric_valstr_raw (trg->met, val);
    trgstr = i_value_valstr_raw (trg->val_type, trg->val);

    switch (trg->trg_type)
    {
      case TRGTYPE_EQUAL:
        if (strcmp(valstr, trgstr) == 0) result = 1;
        break;
      case TRGTYPE_LT:
        if (strcmp(valstr, trgstr) < 0) result = 1;
        break;
      case TRGTYPE_GT:
        if (strcmp(valstr, trgstr) >= 0) result = 1;
        break;
      case TRGTYPE_NOTEQUAL:
        if (strcmp(valstr, trgstr) != 0) result = 1;
        break;
      default:
        i_printf (1, "i_trigger_eval warning, trigger %s for metric %s has an invalid trg->trg_type of %i", 
          trg->name_str, trg->met->name_str, trg->trg_type);
        result = -1;
    }

    if (valstr) free (valstr);
    if (trgstr) free (trgstr);
  }

  /* Check duration */
  if (result == 1)
  {
    /* Trigger condition has been met 
     * First check that an active timestamp has
     * been set, if not set one. Then check to see
     * if the time between now and active_stamp is 
     * >= to the duration value for the trigger. If so,
     * we can be active. Otherwise, things are held down.
     */
    struct timeval now_tv; 
    time_t elapsed;

    /* Get time */
    gettimeofday (&now_tv, NULL);

    /* Check for an active_tstamp */
    if (trg->active_tstamp.tv_sec == 0)
    { 
      trg->active_tstamp.tv_sec = now_tv.tv_sec; 
      trg->active_tstamp.tv_usec = now_tv.tv_usec; 
    }

    /* Compare */
    elapsed = now_tv.tv_sec - trg->active_tstamp.tv_sec;
    if (elapsed >= trg->duration_sec)
    {
      /* Trigger can be active, duration has been met! */
      result = 1;
    }
    else
    {
      /* Trigger duration has not been met, hold it down */
      result = 0;
    }
  }
  else
  {
    /* Trigger condition has not been met, reset the
     * active_tstamp value
     */
    trg->active_tstamp.tv_sec = 0;
    trg->active_tstamp.tv_usec = 0;
  }

  return result;
}

/* @} */
