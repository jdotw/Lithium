#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/value.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <lithium/snmp.h>

#include "cpu.h"

/* Item Manipulation */

l_snmp_nscpu_item* l_snmp_nscpu_item_create ()
{
  l_snmp_nscpu_item *cpu;

  cpu = (l_snmp_nscpu_item *) malloc (sizeof(l_snmp_nscpu_item));
  if (!cpu)
  { i_printf (1, "l_snmp_nscpu_item_create failed to malloc cpu"); return NULL; }
  memset (cpu, 0, sizeof(l_snmp_nscpu_item));

  return cpu;
}

void l_snmp_nscpu_item_free (void *itemptr)
{
  l_snmp_nscpu_item *cpu = itemptr;

  free (cpu);
}
