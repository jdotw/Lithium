#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "ip.h"
#include "value.h"

/* i_value - Generic Value System - Type-related functions */

/* Type string utility */

char* i_value_typestr (unsigned short val_type)
{
  switch (val_type)
  {
    case VALTYPE_INTEGER: return "Integer";
    case VALTYPE_COUNT: return "Counter";
    case VALTYPE_GAUGE: return "Gauge";
    case VALTYPE_FLOAT: return "Floating Point";
    case VALTYPE_STRING: return "String";
    case VALTYPE_OID: return "OID";
    case VALTYPE_IP: return "IP Address";
    case VALTYPE_DATA: return "Data";
    default: return "Unknown";
  }

  return NULL;
}

/* Type identifer */

unsigned short i_value_isnum (unsigned short val_type)
{
  /* Returns 1 if the specified val_type can
   * be represented as a floating point number.
   * Otherwise, returns 0
   */

  switch (val_type)
  {
    case VALTYPE_INTEGER:
    case VALTYPE_COUNT:
    case VALTYPE_GAUGE:
    case VALTYPE_FLOAT:
    case VALTYPE_INTERVAL:
      return 1;
  }

  return 0;
}
