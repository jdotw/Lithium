#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>

#include "snmp.h"

int l_snmp_parse_oidstr (char *oid_str, oid *name, size_t *name_size)
{
  int num;
  size_t orig_size = *name_size;
  
  /* Attempt read_objid method */
  num = read_objid (oid_str, name, name_size);
  if (num == 1)                                 /* Returns 1 on success */
  { return 0; }

  /* Attempt get_node method */
  *name_size = orig_size;
  num = get_node (oid_str, name, name_size);
  if (num == 1)                                 /* Returns 1 on success */
  { return 0; }

  /* Attempt get_wild_node method */
  *name_size = orig_size;
  num = get_wild_node (oid_str, name, name_size); 
  if (num == 1)                                 /* Returns 1 on success */
  { return 0; }

  /* All methods have failed */
  return -1;
}
