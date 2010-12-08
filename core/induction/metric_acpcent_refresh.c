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

/* 
 * Automatically Calculated Metrics - Percent
 */

/** @addtogroup metric_acpcent
 *
 * The real refresh work is done by the countrefcb refresh
 * callback function. The acpcent metric's refresh function is 
 * only used to handle REFOP_CLEANDATA operations to clean
 * the refresh_data.
 * 
 * @ingroup metric
 * @{
 */

int i_metric_acpcent_refresh (i_resource *self, i_metric *pcmet, int opcode)
{
  int num;
  i_metric_value *pcentval;
  i_metric_value *gaugeval;
  i_metric_value *maxval;
  i_metric_acpcent_data *data = pcmet->refresh_data;
  double gaugeflt;
  double maxflt;
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:
      /* This OP always returns 1 to inform
       * i_entity_refresh that the refresh operation
       * is already complete
       */
      
      /* Check the refresh_result of the gaugemet and
       * (if present) the maxmet. If either one is not
       * set to REFRESULT_OK, set the pcmet's refresh_result
       * to the same and return 1
       */
      if (data->gaugemet->refresh_result != REFRESULT_OK)
      { pcmet->refresh_result = data->gaugemet->refresh_result; return 1; }
      if (data->maxmet && data->maxmet->refresh_result != REFRESULT_OK)
      { pcmet->refresh_result = data->maxmet->refresh_result; return 1; }

      /* By this stage, both gaugemet and (if present) maxmet
       * have a REFRESULT_OK result. Set the percent metric's
       * refresh_result to REFESULT_OK
       */
      pcmet->refresh_result = REFRESULT_OK;

      /* Retrieve gauge value */
      gaugeval = i_metric_curval (data->gaugemet);

      /* Retrieve maximum value from either
       * the specified maximum metric of the max_val
       * of the gauge metric
       */
      if (data->maxmet)
      { maxval = i_metric_curval (data->maxmet); }
      else
      { maxval = data->gaugemet->max_val; }

      /* Check that both a gaugeval and maxval have been found */
      if (!gaugeval || !maxval)
      { pcmet->refresh_result = REFRESULT_TOTAL_FAIL; return 1; }

      /* Convert gaugeval and maxval to floating point values */
      gaugeflt = i_metric_valflt (data->gaugemet, gaugeval);
      if (data->maxmet)
      { maxflt = i_metric_valflt (data->maxmet, maxval); }
      else
      { maxflt = i_metric_valflt (data->gaugemet, maxval); }

      /* Create the value */
      pcentval = i_metric_value_create ();

      /* Calculate the value */
      if (maxflt > 0)
      {
        pcentval->flt = gaugeflt / maxflt;
        pcentval->flt *= 100;
      }
      else
      {
        /* Cant div by 0 */
        i_metric_value_free (pcentval);
        pcmet->refresh_result = REFRESULT_TOTAL_FAIL;
        return 1;
      }

      /* Enqueue metric */
      num = i_metric_value_enqueue (self, pcmet, pcentval);
      if (num != 0)
      {
        i_printf (1, "i_metric_acpcent_refresh failed to enqueue value to %s %s",
          i_entity_typestr (pcmet->ent_type), pcmet->name_str);
        i_metric_value_free (pcentval);
        pcmet->refresh_result = REFRESULT_TOTAL_FAIL;
        return 1;
      }

      return 1;

    case REFOP_TERMINATE:
      break;
    case REFOP_CLEANDATA:
      i_metric_acpcent_data_free (pcmet->refresh_data);
      pcmet->refresh_data = NULL;
      break;
    default:
      i_printf (1, "i_metric_acpcent_refresh received unsupported refresh opcode %i for %s %s", 
        opcode, i_entity_typestr(pcmet->ent_type), pcmet->name_str);
  }

  return 0;
}
