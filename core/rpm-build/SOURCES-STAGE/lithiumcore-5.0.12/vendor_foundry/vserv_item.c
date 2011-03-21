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

#include "vserv.h"

/* Item Manipulation */

v_vserv_item* v_vserv_item_create ()
{
  v_vserv_item *vserv;

  vserv = (v_vserv_item *) malloc (sizeof(v_vserv_item));
  if (!vserv)
  { i_printf (1, "v_vserv_item_create failed to malloc vserv"); return NULL; }
  memset (vserv, 0, sizeof(v_vserv_item));

  return vserv;
}

void v_vserv_item_free (void *itemptr)
{
  v_vserv_item *vserv = itemptr;

  free (vserv);
}
