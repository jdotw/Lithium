#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "induction.h"
#include "list.h"
#include "ip.h"
#include "timeutil.h"
#include "value.h"

/* i_value - Generic Value System - String Representation */

/* String Repesentation */

char* i_value_valstr (unsigned short val_type, i_value *val, char *unit_str, i_list *enumstr_list)
{
  char *rawstr = NULL;
  char *valstr;

  /* Enumerated string handling */
  if (enumstr_list && val_type == VALTYPE_INTEGER)
  {
    char *enumstr; 
    enumstr = i_value_enumstr_eval (enumstr_list, val->integer); 
    if (enumstr)
    { rawstr = strdup (enumstr); }
  }

  /* Interval handling */
  else if (val_type == VALTYPE_INTERVAL)
  { rawstr = i_time_interval_str (&val->tv); }

  /* Default value type handling */
  if (!rawstr)
  {
    rawstr = i_value_valstr_raw (val_type, val);
    if (!rawstr)
    {
      /* Handle a NULL raw valstr */
      switch (val_type)
      {
        case VALTYPE_IP:
          rawstr = strdup ("[No IP string in value]");
          break;
        case VALTYPE_OID:
          rawstr = strdup ("[SNMP OID]");
          break;
        case VALTYPE_DATA:
          rawstr = strdup ("[Opaque Data Metric]");
          break;
        default:
          rawstr = strdup ("[Unknown Metric Type]");
      }
    }
  }

  /* Check/Set Unit string */
  if (!unit_str)
  { unit_str = ""; }
  
  /* Create full value string */
  asprintf (&valstr, "%s%s", rawstr, unit_str);
  if (rawstr) free (rawstr);

  return valstr;
}

/* Raw String Representation */

char* i_value_valstr_raw (unsigned short val_type, i_value *val)
{
  char *valstr = NULL;

  switch (val_type)
  {
    case VALTYPE_STRING:
      if (val->str) valstr = strdup (val->str);
      break;
    case VALTYPE_INTEGER:
      asprintf (&valstr, "%i", val->integer);
      break;
    case VALTYPE_COUNT:
      asprintf (&valstr, "%lu", val->count);
      break;
    case VALTYPE_GAUGE:
      asprintf (&valstr, "%lu", val->gauge);
      break;
    case VALTYPE_FLOAT:
      asprintf (&valstr, "%.2f", val->flt);
      break;
    case VALTYPE_IP:
      if (val->ip && val->ip->str) valstr = strdup (val->ip->str);
      break;
    case VALTYPE_INTERVAL:
      asprintf (&valstr, "%lu.%.2f", val->tv.tv_sec, (float) val->tv.tv_usec / 10000);
      break;
    default:
      valstr = NULL;
  }

  return valstr;
}

/* String Value Setting */

i_value* i_value_valstr_set (unsigned short val_type, i_value *orig_val, char *valstr)
{
  /* This function will set the value of the specified 
   * value to the valstr converted and stored according
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

  /* Set Value */
  switch (val_type)
  {
    case VALTYPE_INTEGER:
      val->integer = atoi (valstr);
      break;
    case VALTYPE_COUNT:
      val->count = strtoul (valstr, NULL, 10);
      break;
    case VALTYPE_GAUGE:
      val->gauge = strtoul (valstr, NULL, 10);
      break;
    case VALTYPE_FLOAT:
      val->flt = atof (valstr);
      break;
    case VALTYPE_STRING:
      val->str = strdup (valstr);
      break;
    case VALTYPE_IP:
      val->ip = i_ip_create ();
      val->ip->str = strdup (valstr);
      inet_aton (valstr, &val->ip->addr);
      break;
    case VALTYPE_DATA:
      val->data = strdup (valstr);
      val->datasize = strlen (valstr)+1;
      break;
    default:
      i_printf (1, "i_value_valstr_set failed, string value setting for val_type %s (%i) is unsupported",
        i_value_typestr (val_type), val_type);
      if (!orig_val)
      { i_value_free (val); }
      val = NULL;
  }

  return val;
}
