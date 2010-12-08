#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/path.h>

#include "snmp.h"
#include "snmp_nstcpconn.h"

/* Item Struct Manipulation */

l_snmp_nstcpconn_item* l_snmp_nstcpconn_item_create ()
{
  l_snmp_nstcpconn_item *item;

  item = (l_snmp_nstcpconn_item *) malloc (sizeof(l_snmp_nstcpconn_item));
  if (!item)
  { i_printf (1, "l_snmp_nstcpconn_item_create failed to malloc l_snmp_nstcpconn_item struct"); return NULL; }
  memset (item, 0, sizeof(l_snmp_nstcpconn_item));

  return item;
}

void l_snmp_nstcpconn_item_free (void *itemptr)
{
  l_snmp_nstcpconn_item *item = itemptr;

  if (!item) return;

  free (item);
}
