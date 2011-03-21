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

#include "rserv.h"

/* Item Manipulation */

v_rserv_item* v_rserv_item_create ()
{
  v_rserv_item *rserv;

  rserv = (v_rserv_item *) malloc (sizeof(v_rserv_item));
  if (!rserv)
  { i_printf (1, "v_rserv_item_create failed to malloc rserv"); return NULL; }
  memset (rserv, 0, sizeof(v_rserv_item));

  return rserv;
}

void v_rserv_item_free (void *itemptr)
{
  v_rserv_item *rserv = itemptr;

  free (rserv);
}
