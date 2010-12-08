#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "callback.h"
#include "cement.h"
#include "entity.h"
#include "metric.h"
#include "container.h"
#include "object.h"
#include "str.h"

/** \addtogroup metric_acrate Rate Metric
 * @ingroup metric
 * @{
 */

/* 
 * Automatically Calculated Metrics - Rate
 */

/* Metric creation 
 *
 * A new metric is created, with an external refresh method.
 * The name_str, desc_str and unit_str is configure on 
 * the new metric created. The countmet is the counter-like 
 * (does not have to be a METRIC_COUNT metric, as long as it can
 * be represented as a flt by i_metric_valflt)
 * metric that is used to calculate the rate metric. The modflag
 * allows for common modifiers from the counter value to the 
 * rate value (e.g. bytes to bits)
 */

i_metric* i_metric_acrate_create (i_resource *self, i_object *obj, char *name_str, char *desc_str, char *unit_str, unsigned short recmethod, i_metric *countmet, short int modflag)
{
  int num;
  i_metric *met;
  i_metric_acrate_data *data;

  /* Create refresh data */
  data = i_metric_acrate_data_create ();
  if (!data)
  { i_printf (1, "i_metric_acrate_create failed to create data struct for %s", name_str); return NULL; }
  data->countmet = countmet;
  data->modflag = modflag;

  /* Create metric */
  met = i_metric_create (name_str, desc_str, METRIC_FLOAT);
  if (!met)
  { i_printf (1, "i_metric_acrate_create failed to create metric for %s", name_str); i_metric_acrate_data_free (data); return NULL; }
  met->refresh_func = i_metric_acrate_refresh;
  met->refresh_method = REFMETHOD_EXTERNAL;
  met->refresh_int_sec = 0;
  met->refresh_data = data;
  met->record_method = recmethod;
  met->valstr_func = i_string_rate_metric;
  if (unit_str)
  { met->unit_str = strdup (unit_str); }
  data->met = met;

  /* Add refresh callback to the countmet */
  data->countrefcb = i_entity_refreshcb_add (ENTITY(countmet), i_metric_acrate_refcb, met);
  if (!data->countrefcb)
  { i_printf (1, "i_metric_acrate_create failed to add countmet callback for %s", name_str); i_entity_free (ENTITY(met)); return NULL; }

  /* Register the metric */
  num = i_entity_register (self, ENTITY(obj), ENTITY(met));
  if (num != 0)
  { i_printf (1, "i_metric_acrate_create failed to register metric %s", name_str); i_entity_free (ENTITY(met)); return NULL; }

  return met;
}

/* Refresh Data Struct Manipulation */

i_metric_acrate_data* i_metric_acrate_data_create ()
{
  i_metric_acrate_data *data;

  data = (i_metric_acrate_data *) malloc (sizeof(i_metric_acrate_data));
  if (!data)
  { i_printf (1, "i_metric_acrate_data_create failed to malloc i_metric_acrate_data struct"); return NULL; }
  memset (data, 0, sizeof(i_metric_acrate_data));

  return data;
}

void i_metric_acrate_data_free (void *dataptr)
{
  i_metric_acrate_data *data = dataptr;

  if (!data) return;

  free (data);
}

/* Counter Refresh Callback
 *
 * This function is called by an acrate metric's
 * derivative counter metric when a refresh of
 * the counter metric occurs. This function calls
 * i_entity_refresh to refresh the autocalced rate metric
 */

int i_metric_acrate_refcb (i_resource *self, i_metric *countmet, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  i_metric *ratemet = passdata;

  i_entity_refresh (self, ENTITY(ratemet), REFFLAG_AUTO, NULL, NULL);

  return 0;
}

/* @} */
