#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "timeutil.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric_acsum Sum Metric
 * @ingroup metric
 * @{
 */

/* 
 * Automatically Calculated Metrics - Sum
 */

/* Metric refresh function
 *
 * The REFOP_REFRESH is usually generated by the 
 * i_metric_acsum_refcb refresh callback that
 * is fired when any of the derivitive 
 * metrics are refreshed
 */

int i_metric_acsum_refresh (i_resource *self, i_metric *summet, int opcode)
{
  int num;
  double valflt;
  double xflt;
  i_metric *xmet;
  i_metric_value *xval;
  i_metric_value *sumval;
  i_metric_acsum_data *data = summet->refresh_data;

  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:
      /*
       * This OP always returns 1 to signify that the 
       * refresh has already finished
       */

      /* Set refresh result to a default of REFRESULT_OK
       * If no other errors occur, this result will remain
       */
      summet->refresh_result = REFRESULT_OK;

      /* Initialise the valflt */
      valflt = 0;

      /* Loop through each derivitive metric */
      for (i_list_move_head(data->met_list); (xmet=i_list_restore(data->met_list))!=NULL; i_list_move_next(data->met_list))
      {
        /* Check the refresh_result of the derivitive
         * metric. If it is not OK, skip the metric
         */
        if (xmet->refresh_result != REFRESULT_OK)
        { continue; }

        /* Retrieve current xmet value. If no
         * value is retrieved, skip the metric
         */
        xval = i_metric_curval (xmet);
        if (!xval)
        { continue; }

        /* Convert xval to floating point value */
        xflt = i_metric_valflt (xmet, xval);

        /* Add xflt to valflt */
        valflt += xflt;
      }

      /* Create the value */
      sumval = i_metric_value_create ();

      /* Set the value of sumval using
       * the calculated valflt
       */
      num = i_metric_valflt_set (summet, sumval, valflt);
      if (num != 0)
      {
        i_printf (1, "i_metric_acsum_refresh failed to convert calculated floating point value to a metric value for %s %s",
          i_entity_typestr (summet->ent_type), summet->name_str);
        i_metric_value_free (sumval);
        summet->refresh_result = REFRESULT_TOTAL_FAIL;
        return 1;
      }

      /* Enqueue value */
      num = i_metric_value_enqueue (self, summet, sumval);
      if (num != 0)
      {
        i_printf (1, "i_metric_acsum_refresh failed to enqueue value to %s %s",
          i_entity_typestr (summet->ent_type), summet->name_str);
        i_metric_value_free (sumval);
        summet->refresh_result = REFRESULT_TOTAL_FAIL;
        return 1;
      }

      return 1;

    case REFOP_TERMINATE:
      break;
    case REFOP_CLEANDATA:
      i_metric_acsum_data_free (summet->refresh_data);
      summet->refresh_data = NULL;
      break;
    default:
      i_printf (1, "i_metric_acsum_refresh received unsupported refresh opcode %i for %s %s", 
        opcode, i_entity_typestr(summet->ent_type), summet->name_str);
  }

  return 0;
}

/* @} */