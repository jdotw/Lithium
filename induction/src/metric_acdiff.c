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
 * Automatically Calculated Metrics - Difference
 */

/* Metric creation 
 *
 * A new metric is created, with an external refresh method.
 * The name_str, desc_str and unit_str is configure on 
 * the new metric created. The xmet is the X value 
 * (can me any metric as long as it can be repd as a floating point val)
 * and the xmet is the Y value. The autocalc refresh func will
 * calculat X - Y and store the value according to the met_type. 
 * If no Y value is supplied, the second most recent value from 
 * the X metric is used as the Y value. 
 */

/** @addtogroup metric_acdiff Difference Metric
 * @ingroup metric
 * @{
 */

i_metric* i_metric_acdiff_create (i_resource *self, i_object *obj, char *name_str, char *desc_str, unsigned short met_type, unsigned short recmethod, i_metric *xmet, i_metric *ymet, unsigned short flags)
{
  int num;
  i_metric *met;
  i_metric_acdiff_data *data;

  /* Create refresh data */
  data = i_metric_acdiff_data_create ();
  if (!data)
  { i_printf (1, "i_metric_acdiff_create failed to create data struct for %s", name_str); return NULL; }
  data->xmet = xmet;
  data->ymet = ymet;

  /* Create metric */
  met = i_metric_create (name_str, desc_str, met_type);
  if (!met)
  { i_printf (1, "i_metric_acdiff_create failed to create metric for %s", name_str); i_metric_acdiff_data_free (data); return NULL; }
  met->refresh_func = i_metric_acdiff_refresh;
  met->refresh_method = REFMETHOD_EXTERNAL;
  met->refresh_int_sec = 0;
  met->refresh_data = data;
  met->record_method = recmethod;
  data->met = met;

  /* Add refresh callback to the xmet if the
   * ACDIFF_REFCB_XMET flag is present
   */
  if (flags & ACDIFF_REFCB_XMET)
  {
    data->xrefcb = i_entity_refreshcb_add (ENTITY(xmet), i_metric_acdiff_refcb, met);
    if (!data->xrefcb)
    { 
      i_printf (1, "i_metric_acdiff_create failed to add xmet callback for %s", name_str); 
      i_metric_acdiff_data_free (data); 
      i_entity_free (ENTITY(met)); 
      return NULL; 
    }
  }
  
  /* Add refresh callback to the xmet if the
   * ACDIFF_REFCB_YMET flag is present
   */
  if (flags & ACDIFF_REFCB_YMET)
  {
    data->yrefcb = i_entity_refreshcb_add (ENTITY(ymet), i_metric_acdiff_refcb, met);
    if (!data->yrefcb)
    { 
      i_printf (1, "i_metric_acdiff_create failed to add ymet callback for %s", name_str); 
      i_metric_acdiff_data_free (data); 
      i_entity_free (ENTITY(met)); 
      return NULL; 
    }
  }

  /* Register the metric */
  num = i_entity_register (self, ENTITY(obj), ENTITY(met));
  if (num != 0)
  { i_printf (1, "i_metric_acdiff_create failed to register metric %s", name_str); i_metric_acdiff_data_free (data); i_entity_free (ENTITY(met)); return NULL; }

  return met;
}

/* Refresh Data Struct Manipulation */

i_metric_acdiff_data* i_metric_acdiff_data_create ()
{
  i_metric_acdiff_data *data;

  data = (i_metric_acdiff_data *) malloc (sizeof(i_metric_acdiff_data));
  if (!data)
  { i_printf (1, "i_metric_acdiff_data_create failed to malloc i_metric_acdiff_data struct"); return NULL; }
  memset (data, 0, sizeof(i_metric_acdiff_data));

  return data;
}

void i_metric_acdiff_data_free (void *dataptr)
{
  i_metric_acdiff_data *data = dataptr;

  if (!data) return;

  free (data);
}

/* Derivitive Metric Refresh Callback
 *
 * This function is called by an acdiff metric's
 * derivative metric (xmet or ymet) when a refresh of
 * the counter metric occurs. This function calls
 * i_entity_refresh to refresh the autocalced diff metric
 */

int i_metric_acdiff_refcb (i_resource *self, i_metric *countmet, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  i_metric *diffmet = passdata;
  
  i_entity_refresh (self, ENTITY(diffmet), REFFLAG_AUTO, NULL, NULL);

  return 0;
}

