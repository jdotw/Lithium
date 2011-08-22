#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>

#include "induction.h"
#include "callback.h"
#include "list.h"
#include "timeutil.h"
#include "ip.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric Metrics
 * @{
 */

/* 
 * Metric Utilities 
 */

/* Value retrieval */

i_metric_value* i_metric_curval (i_metric *met)
{
  if (!met) return NULL;

  i_list_move_head (met->val_list);
  return i_list_restore (met->val_list);
}

i_callback* i_metric_histval (i_resource *self, i_metric *met, unsigned short period, time_t ref_sec, int (*cbfunc) (i_resource *self, i_metric *met, i_list *val_list, void *passdata), void *passdata)
{
  /* This function retrieves historical values that occur
   * throughout the period specified by the period variable 
   * that the ref_sec time occurs in. 
   *
   * If the period var and the metric's period value are the
   * same, then the metric's val_list will be examined and the 
   * first (most recent) valid value will be passed to the
   * callback func in a single-entry val_list. The list will
   * then be freed and the func will return 1.
   *
   * The callback function supplied will ALWAYS been called
   * regardless of whether the values are found without a load
   * being called, a load is called or an error occurs. This means
   * the callback func CAN be called BEFORE this function returns.
   *
   * An i_callback struct is returned if the callback func has
   * not yet been called. Otherwise, by the time a NULL is returned, 
   * the callback func has already been called either will a val_list
   * or a NULL list indicating an error
   */
  int num;
  i_metric_value *val;

  /* Check/Set ref_sec */
  if (ref_sec == 0)
  { ref_sec = time(NULL); }

  /* If the period var and the metric->val_period
   * are the same, examine the met->val_list
   */
  if (period == met->val_period)
  {
    i_list *val_list;

    /* Create val_list */
    val_list = i_list_create ();
    
    for (i_list_move_head(met->val_list); (val=i_list_restore(met->val_list))!=NULL; i_list_move_next(met->val_list))
    {
      /* Check if the value is valid for the specified ref_sec */
      num = i_metric_period_validate (met, val->tstamp.tv_sec, ref_sec);
      if (num == 0)
      {
        /* Value is valid for the ref_sec. 
         * Add the val to the val_list, call
         * the callback, free the list and
         * return NULL.
         */
        i_list_enqueue (val_list, val);
        if (cbfunc)
        { cbfunc (self, met, val_list, passdata); }
        i_list_free (val_list);
        return NULL;
      }
      else if (num == 1)
      {
        /* The specified reference time ref_sec is 
         * LATER than the current value. This being
         * the case, it is impossible that any of the
         * older values (and hence any recorded value)
         * could be valid. Call the callback with an
         * empty val_list, free the list and return NULL
         */
        if (cbfunc)
        { cbfunc (self, met, val_list, passdata); }
        i_list_free (val_list);
        return NULL;
      }
    }

    if (val_list) i_list_free (val_list);
  }

  /* At this point, no value in the 
   * val_list is valid. Hence, the 
   * record_method of the metric should
   * be used to retrieve a a historic
   * value
   */
  return i_metric_load (self, met, period, ref_sec, cbfunc, passdata);
}
  
/* 
 * Value Representation 
 */

/* String Representation */

char* i_metric_valstr (i_metric *met, i_metric_value *val)
{
  /* Return a string representation of the 
   * supplied value for the given metric. 
   *
   * This will include the addition
   * of the '*' to indicate that the value is not
   * current and will also add the units string.
   *
   * If the val is NULL, the current value for the metric
   * will be used.
   *
   * NOTE: Does not natively support alloc_unit
   */

  char *rtnstr;             /* String to be returned */
  char *valstr;             /* Raw value string */
  char *unitstr;            /* Units string */
  char *curstr;             /* Currency indicator */

  if (!met) return NULL;

  /* Check for val struct */
  if (!val)
  {
    /* Fetch current value */
    val = i_metric_curval (met);
    if (!val)
    { return NULL; }
  }

  /* Check for an external valstr_func */
  if (met->valstr_func)
  { return met->valstr_func (met, val); }

  /* Create raw value str */
  valstr = i_metric_valstr_raw (met, val);
  if (!valstr)
  {
    /* Handle a NULL raw valstr */
    switch (met->met_type)
    {
      case METRIC_IP:
        valstr = strdup ("[No IP string in value]");
        break;
      case METRIC_OID:
        valstr = strdup ("[SNMP OID]");
        break;
      case METRIC_DATA:
        valstr = strdup ("[Opaque Data Metric]"); 
        break;
      default:
        valstr = strdup ("[Unknown Metric Type]"); 
    }
  }

  /* Metric-type specific special handling.
   *
   * E.g. enumerated strings for integers,
   * time interval strings, etc.
   */

  /* If metric is an integer, check for an enumerated string */
  if (met->met_type == METRIC_INTEGER && met->enumstr_list)
  {
    char *enumstr;
    enumstr = i_metric_enumstr_eval (met, val->integer);
    if (enumstr)
    { free (valstr); valstr = strdup (enumstr); }
  }

  /* If the metric is an interval, use i_time_interval_str */
  if (met->met_type == METRIC_INTERVAL)
  {
    char *intervalstr;
    intervalstr = i_time_interval_str (&val->tv);
    if (intervalstr)
    { free (valstr); valstr = intervalstr; }
  }

  /* Value suffix application
   *
   * If a units string is present it will be applied. 
   * If the value isnt current (i.e refresh status is not
   * REFRESULT_OK, then apply a ' *' to the end of the vale
   */

  /* Check for unit_str */
  if (met->unit_str) unitstr = met->unit_str;
  else unitstr = "";

  /* Check currency of value */
  if (met->refresh_result != REFRESULT_OK) curstr = " *";
  else curstr = "";

  /* Create Return String */
  asprintf (&rtnstr, "%s%s%s", valstr, unitstr, curstr);
  free (valstr);

  return rtnstr;
}

char* i_metric_valstr_raw (i_metric *met, i_metric_value *val)
{
  /* Return a raw string representation of the 
   * supplied value for the given metric. 
   *
   * If the val is NULL, the current value for the metric
   * will be used.
   *
   * NOTE: Does not natively support alloc_unit
   */

  char *valstr = NULL;             /* Raw value string */

  if (!met) return NULL;

  /* Check for val struct */
  if (!val)
  {
    /* Fetch current value */
    val = i_metric_curval (met);
    if (!val)
    { return NULL; }
  }

  /* Create raw value str */
  switch (met->met_type)
  {
    case METRIC_STRING:
      if (val->str) valstr = strdup (val->str);
      break;
    case METRIC_HEXSTRING:
      if (val->str) valstr = strdup (val->str);
      break;
    case METRIC_INTEGER:
      asprintf (&valstr, "%li", val->integer);
      break;
    case METRIC_COUNT:
      asprintf (&valstr, "%lu", val->count);
      break;
    case METRIC_COUNT64:
    case METRIC_COUNT_HEX64:
      asprintf (&valstr, "%llu", val->count64);
      break;
    case METRIC_GAUGE:
      asprintf (&valstr, "%lu", val->gauge);
      break;
    case METRIC_GAUGE_HEX64:
      asprintf (&valstr, "%llu", val->gauge64);
      break;
    case METRIC_FLOAT:
      asprintf (&valstr, "%.2f", val->flt);
      break;
    case METRIC_IP:
      if (val->ip && val->ip->str) valstr = strdup (val->ip->str);
      break;
    case METRIC_INTERVAL:
      asprintf (&valstr, "%lu.%.2f", val->tv.tv_sec, (float) val->tv.tv_usec / 10000);
      break;
    default:
      valstr = NULL;
  }

  return valstr;
}

/* String Value Setting */

int i_metric_valstr_set (i_metric *met, i_metric_value *val, char *str)
{
  /* Set the value of the specified value to
   * the metric-type specific value represented
   * by the specified string.
   */

  switch (met->met_type)
  {
    case METRIC_INTEGER:
      val->integer = atoi (str);
      break;
    case METRIC_COUNT:
      val->count = strtoul (str, NULL, 10);
      break;
    case METRIC_COUNT64:
    case METRIC_COUNT_HEX64:
      val->count64 = strtoull (str, NULL, 10);
      break;
    case METRIC_GAUGE:
      val->gauge = strtoul (str, NULL, 10);
      break;
    case METRIC_GAUGE_HEX64:
      val->gauge64 = strtoull (str, NULL, 10);
      break;
    case METRIC_FLOAT:
      val->flt = atof (str);
      break;
    case METRIC_STRING:
      val->str = strdup (str);
      break;
    case METRIC_HEXSTRING:
      val->str = strdup (str);
      break;
    case METRIC_IP:
      val->ip = i_ip_create ();
      val->ip->str = strdup (str);
      inet_aton (str, &val->ip->addr);
      break;
    case METRIC_DATA:
      val->data = strdup (str);
      val->datasize = strlen (str)+1;
      break;
    default:
      i_printf (2, "i_metric_valstr_set failed for %s %s, setting of metric type %s is unsupported",
        i_entity_typestr (met->ent_type), met->name_str, i_metric_typestr (met->met_type));
  }

  return 0;
}

/* Floating Point Representation */

double i_metric_valflt (i_metric *met, i_metric_value *val)
{
  /* Return a floating point representation of the 
   * supplied value for the given metric. 
   *
   * If the val is NULL, the current value for the metric
   * will be used.
   *
   * Note: DOES natively support alloc_unit and div_by
   */

  double valflt = 0;

  if (!met) return 0;

  /* Check for val struct */
  if (!val)
  {
    /* Fetch current value */
    val = i_metric_curval (met);
    if (!val)
    { return 0.0; }
  }

  /* Create floating point representation */
//  if (strcmp(met->name_str, "size")==0) i_printf(0, "i_metric_valflt (%s) met type is %i (int=%li, count=%lu gauge=%lu)", met->obj->name_str, met->met_type, val->integer, val->count, val->gauge); // DEBUG
  switch (met->met_type)
  {
    case METRIC_INTEGER:
      valflt = (double) val->integer;
      break;
    case METRIC_COUNT:
//      if (strcmp(met->name_str, "size")==0) i_printf(0, "i_metric_valflt (%s) count is %lu (float=%.2f)", met->obj->name_str, val->count, (double)val->count); // DEBUG
      valflt = (double) val->count;
      break;
    case METRIC_COUNT64:
    case METRIC_COUNT_HEX64:
      valflt = (double) val->count64;
      break;
    case METRIC_GAUGE:
      valflt = (double) val->gauge;
      break;
    case METRIC_GAUGE_HEX64:
      valflt = (double) val->gauge64;
      break;
    case METRIC_FLOAT:
      valflt = val->flt;
      break;
    case METRIC_INTERVAL:
      valflt = val->tv.tv_sec;
      valflt += val->tv.tv_usec / 1000000;
      break;
    case METRIC_STRING:
      if (val->str) valflt = strtod (val->str, NULL);
#ifdef NAN
      else valflt = NAN;
#else
      else valflt = nan(NULL);
#endif
      break;
    default:
#ifdef NAN
      valflt = NAN;
#else
      valflt = nan(NULL);
#endif
  }

  /* Perform alloc units */
  unsigned long alloc_unit;
  if (met->alloc_unit_met)
  {
    double allocflt;
    allocflt = i_metric_valflt (met->alloc_unit_met, NULL);
    alloc_unit = (unsigned long) allocflt;
  }
  else
  { alloc_unit = met->alloc_unit; }

 // if (strcmp(met->name_str, "size")==0) i_printf(0, "i_metric_valflt (%s) alloc_unit calc is %.2f x %.2f = %.2f", met->obj->name_str, valflt, (double)alloc_unit, (valflt * (double) alloc_unit)); // DEBUG

  valflt = valflt * (double) alloc_unit;

  /* Perform div_by */
  if (met->div_by > 1)
  { valflt = valflt / (float) met->div_by; }

  /* Perform multiply_by */
  valflt = valflt * met->multiply_by;

//  if (strcmp(met->name_str, "size")==0) i_printf(0, "i_metric_valflt (%s) returning %.2f", met->obj->name_str, valflt); // DEBUG
  
  return valflt;
}

/* Floating Point Value Set */

int i_metric_valflt_set (i_metric *met, i_metric_value *val, double valflt)
{
  if (!met || !val) return -1;

  /* Perform alloc units */
  unsigned long alloc_unit;
  if (met->alloc_unit_met)
  {
    double allocflt;
    allocflt = i_metric_valflt (met->alloc_unit_met, NULL);
    alloc_unit = (unsigned long) allocflt;
  }
  else
  { alloc_unit = met->alloc_unit; }
  valflt = valflt / (double) alloc_unit;

  /* Perform div_by */
  if (met->div_by > 1)
  { valflt = valflt * (float) met->div_by; }

  /* Perform multiply_by */
  valflt = valflt / met->multiply_by;

  /* Check signing of value */
  if (valflt < 0)
  {
    switch (met->met_type)
    {
      case METRIC_GAUGE:
      case METRIC_COUNT:
        i_printf (2, "i_metric_valflt_set failed, %s %s  type (%s) is unsigned and value is < 0",
          i_entity_typestr (met->ent_type), met->name_str, i_metric_typestr (met->met_type));
        return -1;
    }
  }
  
  /* Set value of metric to floating point value specified */
  switch (met->met_type)
  {
    case METRIC_STRING:
      asprintf (&val->str, "%.2f", valflt);
    case METRIC_INTEGER:
      val->integer = (int) valflt;
      break;
    case METRIC_COUNT:
      val->count = (unsigned long) valflt;
      break;
    case METRIC_COUNT64:
    case METRIC_COUNT_HEX64:
      val->count64 = (unsigned long long) valflt;
      break;
    case METRIC_GAUGE:
      val->gauge = (unsigned long) valflt;
      break;
    case METRIC_GAUGE_HEX64:
      val->gauge64 = (unsigned long long) valflt;
      break;
    case METRIC_FLOAT:
      val->flt = valflt;
      break;
    case METRIC_INTERVAL:
      val->tv.tv_sec = (time_t) valflt;
      val->tv.tv_usec = 0;                      /* FIX should calculate useconds from valflt */
      break;
    default:
      i_printf (1, "i_metric_valflt_set failed for %s %s, setting of metric type %s is unsupported",
        i_entity_typestr (met->ent_type), met->name_str, i_metric_typestr (met->met_type));
      return -1;
  }

  return 0;
}

/* Metric Value Type Checking */

int i_metric_valisnum (i_metric *met)
{
  /* Returns 0 if the metric can be
   * represented as a numeric (float)
   * value. Returns -1 if not.
   */
  switch (met->met_type)
  {
    case METRIC_INTEGER:
    case METRIC_COUNT:
    case METRIC_GAUGE:
    case METRIC_FLOAT:
    case METRIC_INTERVAL:
      return 0;
  }

  return -1;
}

/* Last updated time stamp */

char* i_metric_lastupdatestr (i_metric *met)
{
  char *timestr;
  i_metric_value *val;

  val = i_metric_curval (met);
  if (val)
  { timestr = i_time_ctime ((time_t) val->tstamp.tv_sec); }
  else
  { timestr = strdup ("Never"); }

  return timestr;
}

/* @} */
