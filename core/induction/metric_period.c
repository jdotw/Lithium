#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric Metrics
 * @{
 */

/* 
 * CEMent Metrics - Period Validation
 */

int i_metric_period_validate (i_metric *met, time_t ref_sec, time_t now_sec)
{
  /* Check to see if now_sec is in the same period
   * as ref_sec as determined by the met->period_flag
   *
   * Returns 0 if now_sec is in the same period as ref_sec
   *
   * Return 1 if now_sec is in a period LATER than ref_sec
   * Returns -1 if now_sec is in a period EARLIER than ref_sec
   */

  struct tm ref_tm;
  struct tm now_tm;

  /* Get a 'tm' struct representation of the ref_sec and now_sec times*/
  localtime_r (&ref_sec, &ref_tm);
  localtime_r (&now_sec, &now_tm);

  /* Check whether now_tm is in the same period as ref_tm */
  switch (met->val_period)
  {
    case VALPERIOD_INSTANT:
      /* Instantaneous value, -1 is always returned */
      return -1;
    case VALPERIOD_DAY:
      if (ref_tm.tm_year == now_tm.tm_year && ref_tm.tm_yday == now_tm.tm_yday)
      { return 0; }
      else
      { 
        if (ref_tm.tm_year == now_tm.tm_year)
        {
          if (now_tm.tm_yday > ref_tm.tm_yday) return 1;
          else return -1;
        }
        if (now_tm.tm_year > ref_tm.tm_year) return 1;
        else return -1;
      }
    case VALPERIOD_MONTH:
      if (ref_tm.tm_year == now_tm.tm_year && ref_tm.tm_mon == now_tm.tm_mon)
      { return 0; } 
      else
      { 
        if (ref_tm.tm_year == now_tm.tm_year)
        {
          if (now_tm.tm_mon > ref_tm.tm_mon) return 1;
          else return -1;
        }
        if (now_tm.tm_year > ref_tm.tm_year) return 1;
        else return -1;
      }
    case VALPERIOD_YEAR:
      if (ref_tm.tm_year == now_tm.tm_year)
      { return 0; }
      else
      { 
        if (now_tm.tm_year > ref_tm.tm_year) return 1;
        else return -1;
      }
    default:
      i_printf (1, "i_metric_period_validate warning, unsupported val_period (%i) set for metric %s", 
        met->val_period, met->name_str);
  }

  /* Will only reach this point if another 
   * unsupported val_period is set
   */

  return -1;
}

/* @} */
