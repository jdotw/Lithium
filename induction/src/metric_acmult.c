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
 * Automatically Calculated Metrics - Multiply
 */

/* Metric creation 
 *
 * A new metric is created, with an external refresh method.
 * The name_str, desc_str and unit_str is configure on 
 * the new metric created. Two metrics (xmet and ymet) are accepted
 * as params. These two metrics will be enqueued into the metric list.
 * The autocalc refresh func will multiply all metrics in that
 * list and store the value in the mult metric.
 *
 * Additional metrics can be added using i_metric_acmult_addmet
 */

/** @addtogroup metric_acmult Multiplier Metric
 * @ingroup metric
 * @{
 */

i_metric* i_metric_acmult_create (i_resource *self, i_object *obj, char *name_str, char *desc_str, unsigned short met_type, unsigned short recmethod, i_metric *xmet, i_metric *ymet, unsigned short flags)
{
  int num;
  i_metric *met;
  i_metric_acmult_data *data;

  /* Create refresh data */
  data = i_metric_acmult_data_create ();
  if (!data)
  { i_printf (1, "i_metric_acmult_create failed to create data struct for %s", name_str); return NULL; }

  /* Create metric */
  met = i_metric_create (name_str, desc_str, met_type);
  if (!met)
  { i_printf (1, "i_metric_acmult_create failed to create metric for %s", name_str); i_metric_acmult_data_free (data); return NULL; }
  met->refresh_func = i_metric_acmult_refresh;
  met->refresh_method = REFMETHOD_EXTERNAL;
  met->refresh_int_sec = 0;
  met->refresh_data = data;
  met->record_method = recmethod;
  data->met = met;

  /* Add the xmet to the metric list.
   * A refresh callback will be added if the
   * ACMULT_REFCB_XMET flag is present
   */
  num = i_metric_acmult_addmet (met, xmet, ((flags & ACMULT_REFCB_XMET) == ACMULT_REFCB_XMET));
  if (num != 0)
  { i_printf (1, "i_metric_acmult_create failed to add xmet to met_list for %s", name_str); i_entity_free (ENTITY(met)); return NULL; }

  /* Add the ymet to the metric list.
   * A refresh callback will be added if the
   * ACMULT_REFCB_YMET flag is present
   */
  num = i_metric_acmult_addmet (met, ymet, ((flags & ACMULT_REFCB_YMET) == ACMULT_REFCB_YMET));
  if (num != 0)
  { i_printf (1, "i_metric_acmult_create failed to add ymet to met_list for %s", name_str); i_entity_free (ENTITY(met)); return NULL; }

  /* Register the metric */
  num = i_entity_register (self, ENTITY(obj), ENTITY(met));
  if (num != 0)
  { i_printf (1, "i_metric_acmult_create failed to register metric %s", name_str); i_entity_free (ENTITY(met)); return NULL; }

  return met;
}

/* Add an additional metric */

int i_metric_acmult_addmet (i_metric *multmet, i_metric *xmet, unsigned short refcb_flag)
{
  /* Add xmet to the list of metrics used to 
   * calcualte the value of multmet
   */
  int num;
  i_callback *cb = NULL;
  i_metric_acmult_data *data = multmet->refresh_data;

  if (refcb_flag == 1)
  {
    cb = i_entity_refreshcb_add (ENTITY(xmet), i_metric_acmult_refcb, multmet);
    if (!cb)
    { i_printf (1, "i_metric_acmult_addmet failed to add xmet callback for %s", multmet->name_str); return -1; }

    num = i_list_enqueue (data->cb_list, cb);
    if (num != 0)
    { 
      i_printf (1, "i_metric_acmult_addmet failed to enqueue cb into data->cb_list for %s", multmet->name_str);
      i_entity_refreshcb_remove (cb);
      return -1; 
    }
  }
  
  num = i_list_enqueue (data->met_list, xmet);
  if (num != 0)
  { 
    i_printf (1, "i_metric_acmult_addmet failed to enqueue xmet for acmult metric %s", multmet->name_str); 
    if (cb) 
    {
      i_entity_refreshcb_remove (cb);
      num = i_list_search (data->cb_list, cb);
      if (num == 0) i_list_delete (data->cb_list); 
    }
    return -1; 
  }

  return 0;
}

/* Refresh Data Struct Manipulation */

i_metric_acmult_data* i_metric_acmult_data_create ()
{
  i_metric_acmult_data *data;

  data = (i_metric_acmult_data *) malloc (sizeof(i_metric_acmult_data));
  if (!data)
  { i_printf (1, "i_metric_acmult_data_create failed to malloc i_metric_acmult_data struct"); return NULL; }
  memset (data, 0, sizeof(i_metric_acmult_data));

  data->met_list = i_list_create ();
  if (!data->met_list)
  { i_printf (1, "i_metric_acmult_data_create failed to create data->met_list"); i_metric_acmult_data_free (data); return NULL; }

  data->cb_list = i_list_create ();
  if (!data->cb_list)
  { i_printf (1, "i_metric_acmult_data_create failed to create data->cb_list"); i_metric_acmult_data_free (data); return NULL; }

  return data;
}

void i_metric_acmult_data_free (void *dataptr)
{
  i_metric_acmult_data *data = dataptr;

  if (!data) return;

  free (data);
}

/* Derivitive Metric Refresh Callback
 *
 * This function is called by an acmult metric's
 * derivative metric when a refresh of
 * the counter metric occurs. This function calls
 * i_entity_refresh to refresh the autocalced multiply metric
 */

int i_metric_acmult_refcb (i_resource *self, i_metric *derivmet, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the callback alive */
  i_metric *multmet = passdata;

  i_entity_refresh (self, ENTITY(multmet), REFFLAG_AUTO, NULL, NULL);

  return 0;
}

/* @} */