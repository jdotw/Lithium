#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "induction.h"
#include "list.h"
#include "callback.h"
#include "ip.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric_record Recording and Graphing
 * @ingroup metric
 * @{
 */

/* Metric Values - Loading of historic values
 *
 * This function loads all values for the 
 * given metric that occur within the period
 * (day/month/year) as indicated by the 
 * period variable in which the ref_sec time
 * occurs.
 *
 * For example, if the period of VALPERIOD_MONTH is
 * set, then all values that occurred within the month
 * referenced by ref_sec will be loaded.
 *
 * This function calls the record_method specific 
 * load function. These functions return an i_callback 
 * if the load operation is successfully started. 
 * This function DOES NOT check to see if a valid and 
 * complete set of values can be found in the 
 * metrics val_list. See i_metric_histval for a 
 * combination val_list / metric_load method of retrieving
 * historical values.
 */

i_callback* i_metric_load (i_resource *self, i_metric *met, unsigned short period, time_t ref_sec, int (*cbfunc) (i_resource *self, i_metric *met, i_list *val_list, void *passdata), void *passdata)
{
  if (met->record_method & RECMETHOD_SQL)
  {
    /* SQL Value retrieval */
    return i_metric_load_sql (self, met, period, ref_sec, cbfunc, passdata);
  }
  else
  {
    /* Unsupported record method */
    i_printf (1, "i_metric_load failed, unsupported record_method (%i) set", 
      met->record_method);
  }

  return NULL;
}

/* Synchronous loading... 
 *
 * As above, but synchronous
 */

i_list* i_metric_load_sync (i_resource *self, i_metric *met, unsigned short period, time_t ref_sec)
{
  if (met->record_method & RECMETHOD_SQL)
  {
    /* SQL Value retrieval */
    return i_metric_load_sql_sync (self, met, period, ref_sec);
  }
  else
  {
    /* Unsupported record method */
    i_printf (1, "i_metric_load_sync failed, unsupported record_method (%i) set", 
      met->record_method);
  }

  return NULL;
}

/* @} */
