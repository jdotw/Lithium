#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "callback.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/* 
 * Automatically Calculated Metrics - Percent
 */

/** @addtogroup metric_acpcent Percentage Metric
 *
 * A new metric is created, with an external refresh method.
 * The name_str and desc_str is configure on 
 * the new metric created. The gaugemet is the gauge-like 
 * (does not have to be a METRIC_GAUGE metric, as long as it can
 * be represented as a flt by i_metric_valflt)
 * metric that is compared to maxmet, which is the gauge-like
 * (does not have to be a METRIC_GAUGE metric, as long as it can
 * be represented as a flt by i_metric_valflt)
 * metric that defined the maximum possible value of gaugemet. If the 
 * maxmet is not specified, the max_val of the gaugemet metric is used.
 * 
 * @ingroup metric
 * @{
*/

i_metric* i_metric_acpcent_create (i_resource *self, i_object *obj, char *name_str, char *desc_str, unsigned short recmethod, i_metric *gaugemet, i_metric *maxmet, unsigned short flags)
{
  int num;
  i_metric *met;
  i_metric_acpcent_data *data;

  /* Create refresh data */
  data = i_metric_acpcent_data_create ();
  if (!data)
  { i_printf (1, "i_metric_acpcent_create failed to create data struct for %s", name_str); return NULL; }
  data->gaugemet = gaugemet;
  data->maxmet = maxmet;

  /* Create metric */
  met = i_metric_create (name_str, desc_str, METRIC_FLOAT);
  if (!met)
  { i_printf (1, "i_metric_acpcent_create failed to create metric for %s", name_str); i_metric_acpcent_data_free (data); return NULL; }
  met->min_val = i_metric_value_create ();
  met->min_val->flt = 0;
  met->max_val = i_metric_value_create ();
  met->max_val->flt = 100;
  met->refresh_func = i_metric_acpcent_refresh;
  met->refresh_method = REFMETHOD_EXTERNAL;
  met->refresh_int_sec = 0;
  met->refresh_data = data;
  met->record_method = recmethod;
  met->unit_str = strdup ("%");
  data->met = met;

  /* Add refresh callback to the gaugemet if the
   * ACPCENT_REFCB_GAUGE flag is present
   */
  if (flags & ACPCENT_REFCB_GAUGE)
  {
    data->gaugerefcb = i_entity_refreshcb_add (ENTITY(gaugemet), i_metric_acpcent_refcb, met);
    if (!data->gaugerefcb)
    { i_printf (1, "i_metric_acpcent_create failed to add gaugemet refreshcb for %s", name_str); i_entity_free (ENTITY(met)); return NULL; }
  }

  /* Add refresh callback to the maxmet if the
   * ACPCENT_REFCB_MAX flag is present and a maxmet
   * is specified.
   */
  if (maxmet && (flags & ACPCENT_REFCB_MAX))
  {
    data->maxrefcb = i_entity_refreshcb_add (ENTITY(maxmet), i_metric_acpcent_refcb, met);
    if (!data->maxrefcb)
    { i_printf (1, "i_metric_acpcent_create failed to add maxmet refreshcb for %s", name_str); i_entity_free (ENTITY(met)); return NULL; }
  }

  /* Register the metric */
  num = i_entity_register (self, ENTITY(obj), ENTITY(met));
  if (num != 0)
  { i_printf (1, "i_metric_acpcent_create failed to register metric %s", name_str); i_entity_free (ENTITY(met)); return NULL; }

  return met;
}

/* Refresh Data Struct Manipulation */

i_metric_acpcent_data* i_metric_acpcent_data_create ()
{
  i_metric_acpcent_data *data;

  data = (i_metric_acpcent_data *) malloc (sizeof(i_metric_acpcent_data));
  if (!data)
  { i_printf (1, "i_metric_acpcent_data_create failed to malloc i_metric_acpcent_data struct"); return NULL; }
  memset (data, 0, sizeof(i_metric_acpcent_data));

  return data;
}

void i_metric_acpcent_data_free (void *dataptr)
{
  i_metric_acpcent_data *data = dataptr;

  if (!data) return;

  free (data);
}

/* Gauge/Max Refresh Callback
 *
 * This metric refresh callback is called by 
 * the gaugemet metric and (if present) the 
 * maxmet metric when a refrsh is completed by 
 * either respectively. This function calls
 * i_entity_refresh to refresh the auto-calculated
 * percent metric
 * 
 */

int i_metric_acpcent_refcb (i_resource *self, i_metric *eithermet, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  i_metric *pcmet = passdata;

  i_entity_refresh (self, ENTITY(pcmet), REFFLAG_AUTO, NULL, NULL);

  return 0;
}


