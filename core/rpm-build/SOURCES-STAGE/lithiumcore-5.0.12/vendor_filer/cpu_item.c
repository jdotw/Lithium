#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/navtree.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/value.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "device/snmp.h"

#include "cpu.h"

/* Item Manipulation */

v_cpu_item* v_cpu_item_create ()
{
  v_cpu_item *cpu;

  cpu = (v_cpu_item *) malloc (sizeof(v_cpu_item));
  if (!cpu)
  { i_printf (1, "v_cpu_item_create failed to malloc cpu"); return NULL; }
  memset (cpu, 0, sizeof(v_cpu_item));

  return cpu;
}

void v_cpu_item_free (void *itemptr)
{
  v_cpu_item *cpu = itemptr;

  free (cpu);
}
