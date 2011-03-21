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

#include "ram.h"

/* Item Manipulation */

l_snmp_nsram_item* l_snmp_nsram_item_create ()
{
  l_snmp_nsram_item *ram;

  ram = (l_snmp_nsram_item *) malloc (sizeof(l_snmp_nsram_item));
  if (!ram)
  { i_printf (1, "l_snmp_nsram_item_create failed to malloc ram"); return NULL; }
  memset (ram, 0, sizeof(l_snmp_nsram_item));

  return ram;
}

void l_snmp_nsram_item_free (void *itemptr)
{
  l_snmp_nsram_item *ram = itemptr;

  free (ram);
}
