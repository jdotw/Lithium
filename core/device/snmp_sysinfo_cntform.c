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

#include "snmp_sysinfo.h"

int l_snmp_sysinfo_cntform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{
  i_object *obj;

  obj = l_snmp_sysinfo_obj ();
  if (obj)
  {
    return l_snmp_sysinfo_objform (self, ENTITY(obj), reqdata);
  }

  return -1;
}
