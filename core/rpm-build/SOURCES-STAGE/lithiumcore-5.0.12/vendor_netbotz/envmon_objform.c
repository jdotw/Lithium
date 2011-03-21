#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/ip.h"
#include "induction/timeutil.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/interface.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/form.h"
#include "induction/str.h"
#include "device/snmp.h"

#include "envmon.h"

int v_envmon_objform (i_resource *self, i_entity *ent, i_form_reqdata *reqdata)
{

  /* 
   * Environmental Conditions
   */

  i_container *cnt = v_envmon_cnt ();

  return v_envmon_cntform (self, ENTITY(cnt), reqdata);
}

