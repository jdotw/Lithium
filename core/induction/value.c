#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "ip.h"
#include "value.h"

/* i_value - Generic Value System */

/* Struct Manipulation */

i_value* i_value_create ()
{
  i_value *val;

  val = (i_value *) malloc (sizeof(i_value));
  if (!val)
  { i_printf (1, "i_value_create failed to malloc i_value struct"); return NULL; }
  memset (val, 0, sizeof(i_value));

  gettimeofday (&val->tstamp, NULL);

  return val;
}

void i_value_free (void *valptr)
{
  i_value *val = valptr;

  if (!val) return;

  if (val->str) free (val->str);
  if (val->oid) free (val->oid);
  if (val->ip) i_ip_free (val->ip);
  if (val->data) free (val->data);
        
  free (val);
}

i_value* i_value_duplicate (i_value *val)
{
  i_value *dup;

  /* Create dup struct */
  dup = i_value_create ();
  if (!dup)
  { i_printf (1, "i_value_duplicate failed to create dup struct"); return NULL; }

  /* Copy struct data */
  memcpy (dup, val, sizeof(i_value));

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

