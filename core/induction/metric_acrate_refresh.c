#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "timeutil.h"
#include "hierarchy.h"
#include "cement.h"
#include "device.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric_acrate Rate Metric
 * @ingroup metric
 * @{
 */

/* 
 * Automatically Calculated Metrics - Rate
 */

/* Metric refresh function
 *
 * The REFOP_REFRESH is usually generated by the 
 * i_metric_acrate_countrefcb refresh callback that
 * is fired whe the derivitive counter metric is refreshed
 */

int i_metric_acrate_refresh (i_resource *self, i_metric *ratemet, int opcode)
{
  int num;
  double curcountflt;
  double prevcountflt;
  struct timeval elapsed_tv;
  i_metric_value *curcountval;
  i_metric_value *prevcountval;
  i_metric_value *rateval;
  i_metric_acrate_data *data = ratemet->refresh_data;

  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:
      /* Set Refresh Value to be the
       * refresh value of the counter metric. 
       * If the counter metrics refresh result is not
       * REFRESULT_OK, then there cant be a new value to 
       * calculate a rate from.
       *
       * This OP always returns 1 to signify that the 
       * refresh has already finished
       */
      ratemet->refresh_result = data->countmet->refresh_result;
      if (ratemet->refresh_result != REFRESULT_OK)
      { return 1; }

      /* Retrieve cur/prev counter values if
       * a current and previous value can not
       * be retrieved, the func will exit.
       */
      i_list_move_head (data->countmet->val_list);
      curcountval = i_list_restore (data->countmet->val_list);
      i_list_move_next (data->countmet->val_list);
      prevcountval = i_list_restore (data->countmet->val_list);
      if (!curcountval || !prevcountval)
      { ratemet->refresh_result = REFRESULT_TOTAL_FAIL; return 1; }

      /* Convert cur/prevcountval to floating point values */
      curcountflt = i_metric_valflt (data->countmet, curcountval);
      prevcountflt = i_metric_valflt (data->countmet, prevcountval);

      /* Create the value */
      rateval = i_metric_value_create ();

      /* Calculate the value */
      i_time_subtract (&curcountval->tstamp, &prevcountval->tstamp, &elapsed_tv);
      if (elapsed_tv.tv_sec > 0)
      {
        if (prevcountflt <= curcountflt)
        { rateval->flt = (double) (curcountflt - prevcountflt) / elapsed_tv.tv_sec; }
        else
        { rateval->flt = (double) curcountflt / elapsed_tv.tv_sec; }
      }
      else
      {
        /* Cant div by 0 */
        i_metric_value_free (rateval);
        ratemet->refresh_result = REFRESULT_TOTAL_FAIL;
        return 1;
      }

      /* Apply modifier */
      switch (data->modflag)
      {
        case ACRATE_MOD_BYTETOBIT:
          rateval->flt *= 8;
          break;
      }

      /* Enqueue metric */
      num = i_metric_value_enqueue (self, ratemet, rateval);
      if (num != 0)
      {
        i_printf (1, "i_metric_acrate_refresh failed to enqueue value to %s %s",
          i_entity_typestr (ratemet->ent_type), ratemet->name_str);
        i_metric_value_free (rateval);
        ratemet->refresh_result = REFRESULT_TOTAL_FAIL;
        return 1;
      }

      return 1;

    case REFOP_TERMINATE:
      break;
    case REFOP_CLEANDATA:
      i_metric_acrate_data_free (ratemet->refresh_data);
      ratemet->refresh_data = NULL;
      break;
    default:
      i_printf (1, "i_metric_acrate_refresh received unsupported refresh opcode %i for %s %s", 
        opcode, i_entity_typestr(ratemet->ent_type), ratemet->name_str);
  }

  return 0;
}

/* @} */