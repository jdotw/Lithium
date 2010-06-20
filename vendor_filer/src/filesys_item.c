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

#include "filesys.h"

/* Item Manipulation */

v_filesys_item* v_filesys_item_create ()
{
  v_filesys_item *filesys;

  filesys = (v_filesys_item *) malloc (sizeof(v_filesys_item));
  if (!filesys)
  { i_printf (1, "v_filesys_item_create failed to malloc filesys"); return NULL; }
  memset (filesys, 0, sizeof(v_filesys_item));

  return filesys;
}

void v_filesys_item_free (void *itemptr)
{
  v_filesys_item *filesys = itemptr;

  free (filesys);
}
