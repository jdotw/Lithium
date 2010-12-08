#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "metric.h"
#include "str.h"
#include "callback.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"

/** \addtogroup metric_acvol Volume Metric
 * @ingroup
 * @{
 */

/* 
 * Automatically Calculated Metrics - Volume
 */

/* Metric creation 
 *
 * A new metric is created, with an external refresh method.
 * The name_str, desc_str and unit_str is configure on 
 * the new metric created. The countmet is the METRIC_COUNT
 * (must be a METRIC_COUNT metric) metric that is used to 
 * calculate the volume metric. The value period
 * flag dictates how often the volume metric is reset to 0.
 */

i_metric* i_metric_acvol_create (i_resource *self, i_object *obj, char *name_str, char *desc_str, char *unit_str, unsigned long alloc_unit, unsigned short recmethod, i_metric *countmet, i_metric *uptimemet, i_metric *discontmet, short int val_period)
{
  int num;
  i_metric *met;
  i_metric_acvol_data *data;

  /* Create refresh data */
  data = i_metric_acvol_data_create ();
  if (!data)
  { i_printf (1, "i_metric_acvol_create failed to create data struct for %s", name_str); return NULL; }
  data->countmet = countmet;
  data->uptimemet = uptimemet;
  data->discontmet = discontmet;

  /* Create metric */
  met = i_metric_create (name_str, desc_str, METRIC_GAUGE);
  if (!met)
  { i_printf (1, "i_metric_acvol_create failed to create metric for %s", name_str); i_metric_acvol_data_free (data); return NULL; }
  met->refresh_func = i_metric_acvol_refresh;
  met->refresh_method = REFMETHOD_EXTERNAL;
  met->refresh_int_sec = 0;
  met->refresh_data = data;
  met->record_method = recmethod;
  met->alloc_unit = alloc_unit;
  met->val_period = val_period;
  met->valstr_func = i_string_volume_metric;
  if (unit_str)
  { met->unit_str = strdup (unit_str); }
  data->met = met;

  /* Add refresh callback to the countmet */
  data->countrefcb = i_entity_refreshcb_add (ENTITY(countmet), i_metric_acvol_refcb, met);
  if (!data->countrefcb)
  { i_printf (1, "i_metric_acvol_create failed to add countmet callback for %s", name_str); i_entity_free (ENTITY(met)); return NULL; }

  /* Register the metric */
  num = i_entity_register (self, ENTITY(obj), ENTITY(met));
  if (num != 0)
  { i_printf (1, "i_metric_acvol_create failed to register metric %s", name_str); i_entity_free (ENTITY(met)); return NULL; }
  
  /* Attempt to load a historic value
   * for the same period as the metric's
   * val_period with a ref_sec of now
   */
  i_list *val_list = i_metric_load_sync (self, met, met->val_period, time(NULL));
  i_metric_acvol_process_history (self, met, val_list);

  return met;
}

/* Refresh Data Struct Manipulation */

i_metric_acvol_data* i_metric_acvol_data_create ()
{
  i_metric_acvol_data *data;

  data = (i_metric_acvol_data *) malloc (sizeof(i_metric_acvol_data));
  if (!data)
  { i_printf (1, "i_metric_acvol_data_create failed to malloc i_metric_acvol_data struct"); return NULL; }
  memset (data, 0, sizeof(i_metric_acvol_data));

  return data;
}

void i_metric_acvol_data_free (void *dataptr)
{
  i_metric_acvol_data *data = dataptr;

  if (!data) return;

  free (data);
}

/* Counter Refresh Callback
 *
 * This function is called by an acvol metric's
 * derivative counter metric when a refresh of
 * the counter metric occurs. This function calls
 * i_entity_refresh to refresh the autocalced vol metric
 */

int i_metric_acvol_refcb (i_resource *self, i_metric *countmet, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  i_metric *volmet = passdata;

  i_entity_refresh (self, ENTITY(volmet), REFFLAG_AUTO, NULL, NULL);

  return 0;
}

/* Historic Value Callback
 *
 * This function is called when a historic value load
 * operation occurs for a newly created metric. The first
 * value in the val_list (if any) is duplicated, the value
 * of the latest metric in the metric's val_list (if any)
 * is added to this duped value and then the duped value
 * is enqueued into the metric as a new value
 */

int i_metric_acvol_process_history (i_resource *self, i_metric *met, i_list *val_list)
{
  int num;
  i_metric_value *histval;        /* The historic value */
  i_metric_value *curval;         /* The current value of the metric */
  i_metric_value *newval;         /* The new (historic+existing) value */

  /* Retrieve historic value */
  i_list_move_head (val_list);
  histval = i_list_restore (val_list);
  if (histval)
  {
    unsigned short record_method = met->record_method;
    
    /* Create new value (duplicate of histval) */
    newval = i_metric_value_duplicate (histval);
    gettimeofday (&newval->tstamp, NULL);

    /* Get current value for metric */
    curval = i_metric_curval (met);
    if (curval)
    { 
      double curflt;
      double histflt;
      double newflt;

      curflt = i_metric_valflt (met, curval);
      histflt = i_metric_valflt (met, histval);
      newflt = curflt + histflt;
      
      i_metric_valflt_set (met, newval, newflt);
    }

    /* If there was no current value for
     * the metric, then the value being enqueued
     * is an exact duplicate of what was retrieved
     * from the SQL databsee. Hence, when this value
     * is enqueued, temporarily set the metric's
     * record flag to RECMETHOD_NONE to suppress 
     * an unnecessary SQL UPDATE
     */
    if (!curval)
    { 
      /* Save Old Value */
      record_method = met->record_method;

      /* Suppress recording */
      met->record_method = RECMETHOD_NONE;
    }

    /* Enqueue new value */
    num = i_metric_value_enqueue (self, met, newval);
    if (num != 0)
    { 
      i_printf (1, "i_metric_acvol_process_history failed to enqueue value derived from historical value for metric %s", met->name_str); 
      i_metric_value_free (newval);
      return -1; 
    }

    /* Restore previous record method */
    if (!curval)
    { met->record_method = record_method; }
  }

  return 0;
}

/* @} */
