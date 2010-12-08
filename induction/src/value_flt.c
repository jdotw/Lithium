#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "induction.h"
#include "list.h"
#include "ip.h"
#include "value.h"

/* i_value - Generic Value System - String Representation */

/* Floating Point Number Repesentation */

double i_value_valflt (unsigned short val_type, i_value *val)
{
  double valflt = 0;

  switch (val_type)
  {
    case VALTYPE_INTEGER:
      valflt = (double) val->integer;
      break;
    case VALTYPE_COUNT:
      valflt = (double) val->count;
      break;
    case VALTYPE_GAUGE:
      valflt = (double) val->gauge;
      break;
    case VALTYPE_FLOAT:
      valflt = val->flt;
      break;
    case VALTYPE_INTERVAL:
      valflt = val->tv.tv_sec;
      valflt += val->tv.tv_usec / 1000000;
      break;
    case VALTYPE_STRING:
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

  return valflt;
}

/* Floating Point Number Value Setting */

i_value* i_value_valflt_set (unsigned short val_type, i_value *orig_val, double valflt)
{
  /* This function will set the value of the specified 
   * value to the valflt converted and stored according
   * to the val_type. If the val pointer is NULL, a new
   * value is created.
   *
   * On success, the address of the value that was 
   * created/manipulated is returned. Otherwise, NULL
   * is returned on error.
   */

  i_value *val;

  /* Check/Create val */
  if (orig_val)
  { val = orig_val; }
  else
  { val = i_value_create (); }

  /* Value setting */
  switch (val_type)
  {
    case VALTYPE_STRING:
      asprintf (&val->str, "%.2f", valflt);
      break;
    case VALTYPE_INTEGER:
      val->integer = (int) valflt;
      break;
    case VALTYPE_COUNT:
      val->count = (unsigned long) valflt;
      break;
    case VALTYPE_GAUGE:
      val->gauge = (unsigned long) valflt;
      break;
    case VALTYPE_FLOAT:
      val->flt = valflt;
      break;
    case VALTYPE_INTERVAL:
      val->tv.tv_sec = (time_t) valflt;
      val->tv.tv_usec = 0;                      /* FIX should calculate useconds from valflt */
      break;
    default:
      i_printf (1, "i_value_valflt_set failed, floating point value setting for val_type %s (%i) is unsupported",
        i_value_typestr (val_type), val_type);
      if (!orig_val)
      { i_value_free (val); }
      val = NULL;
  }

  return val;
}
