#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>

#include "snmp_sysinfo.h"

/* Availability Items */

/* Struct Manipulation */

l_snmp_sysinfo_item* l_snmp_sysinfo_item_create ()
{
  l_snmp_sysinfo_item *item;

  item = (l_snmp_sysinfo_item *) malloc (sizeof(l_snmp_sysinfo_item));
  if (!item)
  { i_printf (1, "l_snmp_sysinfo_item_create failed to malloc l_snmp_sysinfo_item struct"); return NULL; }
  memset (item, 0, sizeof(l_snmp_sysinfo_item));

  return item;
}

void l_snmp_sysinfo_item_free (void *itemptr)
{
  l_snmp_sysinfo_item *item = itemptr;

  if (item) return;

  free (item);
}

