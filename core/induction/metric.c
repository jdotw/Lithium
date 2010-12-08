#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "navtree.h"
#include "metric.h"
#include "message.h"
#include "incident.h"

/* 
 * CEMent Metrics
 */

/** \addtogroup metric Metrics
 * @ingroup entity
 * @{
 */

/* Struct manipulation */

i_metric* i_metric_create (char *name_str, char *desc_str, unsigned short met_type)
{
  i_metric *met;

  met = (i_metric *) i_entity_create (name_str, desc_str, ENT_METRIC, sizeof(i_metric));
  if (!met)
  { i_printf (1, "i_metric_create failed to create metric entity"); return NULL; }
  met->met_type = met_type;
  met->navtree_expand = NAVTREE_EXP_RESTRICT;
  met->authorative = 1;

  met->val_list = i_list_create ();
  if (!met->val_list)
  { i_printf (1, "i_metric_create failed to create met->val_list"); return NULL; }
  i_list_set_destructor (met->val_list, i_metric_value_free);
  met->val_list_maxsize = METRIC_DEF_MAXVAL;
  met->alloc_unit = 1;                                  /* Default alloc unit */
  met->kbase = 1000;                                    /* Default kbase */
  met->div_by = 1;
  met->multiply_by = 1.0;
  met->record_priority = 1;

  gettimeofday (&met->rrd_5min_tstamp, NULL);
  gettimeofday (&met->rrd_6hr_tstamp, NULL);
              

  return met;
}

void i_metric_free (void *metptr)
{
  /* Just free the metric-specific 
   * portions of the struct. The struct
   * and everything else will be freed
   * by i_entity_free
   */
  i_metric *met = metptr;

  if (!met) return;

  if (met->unit_str) free (met->unit_str);
  if (met->val_list) i_list_free (met->val_list);
  if (met->min_val) i_metric_value_free (met->min_val);
  if (met->max_val) i_metric_value_free (met->max_val);
  if (met->enumstr_list) i_list_free (met->enumstr_list);
  if (met->rrd_update_args) free (met->rrd_update_args);
}

/* 
 * Metric Specific Registration/Deregistration 
 * (Should only be called by i_entity_register and i_entity_register)
 */

int i_metric_register (i_resource *self, i_object *obj, i_metric *met)
{
  /* Stub - Nothing done here */
  return 0;
}

int i_metric_deregister (i_resource *self, i_metric *met)
{
  /* Stub - Nothing done here */

  /* Clear Incident 
   *
   * NOTE: The fate of the incident struct is now handed over
   *       to i_incident_clear. The incident struct should not
   *       be freed here
   */

  if (met->op_inc)
  { 
    i_incident_clear (self, ENTITY(met), met->op_inc); 
    met->op_inc = NULL; 
  }

  return 0;
}


/* String utilities */

char* i_metric_typestr (unsigned short type)
{
  switch (type)
  {
    case METRIC_INTEGER: return "Integer";
    case METRIC_COUNT: return "Counter";
    case METRIC_COUNT64: return "Counter (64bit)";
    case METRIC_GAUGE: return "Gauge";
    case METRIC_FLOAT: return "Floating Point";
    case METRIC_STRING: return "String";
    case METRIC_OID: return "OID";
    case METRIC_IP: return "IP Address";
    case METRIC_DATA: return "Data";
    case METRIC_COUNT_HEX64: return "Counter (64bit octet string)";
    case METRIC_GAUGE_HEX64: return "Gauge (64bit octet string)";
    default: return "Unknown";
  }

  return NULL;
}

/* @} */
