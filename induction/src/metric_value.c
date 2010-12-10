#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "induction.h"
#include "list.h"
#include "ip.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"

/** \addtogroup metric Metrics
 * @{
 */

/* Metric Values - Part of Cement */

/* Struct manipulation */

i_metric_value* i_metric_value_create ()
{
  i_metric_value *val;

  val = (i_metric_value *) malloc (sizeof(i_metric_value));
  if (!val)
  { i_printf (1, "i_metric_value_create failed to malloc i_metric_value struct"); return NULL; }
  memset (val, 0, sizeof(i_metric_value));

  gettimeofday (&val->tstamp, NULL);

  return val;
}

void i_metric_value_free (void *valptr)
{
  i_metric_value *val = valptr;

  if (!val) return;

  if (val->str) free (val->str);
  if (val->oid) free (val->oid);
  if (val->ip) i_ip_free (val->ip);
  if (val->data) free (val->data);

  free (val);
}

i_metric_value* i_metric_value_duplicate (i_metric_value *val)
{
  i_metric_value *dup;

  /* Create dup struct */
  dup = i_metric_value_create ();
  if (!dup)
  { i_printf (1, "i_metric_value_duplicate failed to create dup struct"); return NULL; }

  /* Copy struct data */
  memcpy (dup, val, sizeof(i_metric_value));

  /* Duplicate any strings/data etc */
  if (val->str) dup->str = strdup (val->str); 
  if (val->oid)
  { dup->oid = malloc (val->oid_len); memcpy (dup->oid, val->oid, val->oid_len); }
  if (val->data)
  { dup->data = malloc (val->datasize); memcpy (dup->data, val->data, val->datasize); }
  if (val->ip)
  { dup->ip = i_ip_duplicate (val->ip); }

  return dup;
}

/* Value List Manipulation */

int i_metric_value_enqueue (i_resource *self, i_metric *met, i_metric_value *val)
{
  /* Enqueue a val to the met->val_list list whilst
   * observing the met->val_list_maxsize limit on the max
   * number of values that should be held in the val_list
   *
   * NOTE: This mechanism will automatically grow and shrink 
   *       the val_list depending on the current value of 
   *       met->val_list_maxsize
   */
  
  int num;

  /* Check val_list_maxsize */
  if (met->val_list_maxsize == 0)
  {
    i_printf (1, "i_metric_value_enqueue failed, met->val_list_maxsize = 0 for %s", met->name_str);
    return -1;
  }
  
  /* Check to see if the values are the same */
  char *curvalstr = i_metric_valstr_raw (met, NULL);
  char *newvalstr = i_metric_valstr_raw (met, val);
  if (!curvalstr || !newvalstr || strcmp(curvalstr, newvalstr) != 0)
  {
    /* Value has changed, update version */
    if (met->authorative) met->version = time (NULL);
  }
  if (curvalstr) free (curvalstr);
  if (newvalstr) free (newvalstr);

  /* 'Pop' old values off list as required */
  while (met->val_list->size >= met->val_list_maxsize)
  { i_list_poplast (met->val_list); }

  /* 'Push' the new value to the front of the list */
  num = i_list_push (met->val_list, val);
  if (num != 0)
  { 
    i_printf (1, "i_metric_value_enqueue failed to enqueue value to metric %s", met->name_str); 
    return -1; 
  }

  /* Call i_metric_record to perform any 
   * configured recording tasks
   */
  if (met->authorative)
  {
    num = i_metric_record (self, met);
    if (num != 0)
    { 
      i_printf (1, "i_metric_value_enqueue warning, failed to record metric %s", met->name_str);
    }

    /* Process all triggers registered to the metric */
    num = i_trigger_process_all (self, met);
    if (num != 0)
    { i_printf (1, "i_metric_value_enqueue warning, failed to process all triggers for metric %s", met->name_str); }

    /* Update aggregate delta */
    i_metric_value *last_value = NULL;
    float aggregate_delta = 0.0;
    for (i_list_move_head(met->val_list); (val=i_list_restore(met->val_list))!=NULL; i_list_move_next(met->val_list))
    {
      if (last_value)
      {
        float cur_val = i_metric_valflt (met, val);
        float prev_val = i_metric_valflt (met, last_value);
        if (cur_val > prev_val)
        { aggregate_delta = aggregate_delta + (cur_val - prev_val); }
        else
        { aggregate_delta = aggregate_delta + (prev_val - cur_val); }
      }
      last_value = val;
    }
    met->aggregate_delta = aggregate_delta;
  }

  return 0;
}

/* @} */
