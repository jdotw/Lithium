#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/ip.h>
#include <induction/timeutil.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/form.h>
#include <induction/str.h>

#include "sysinfo.h"

int v_sysinfo_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  v_sysinfo_item *item = v_sysinfo_static_item ();
  if (item)
  { return v_sysinfo_objform (self, ENTITY(item->obj), reqdata); }
  return -1;
}

