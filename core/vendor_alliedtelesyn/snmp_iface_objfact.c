#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/timeutil.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/str.h"

#include "device/snmp.h"
#include "device/snmp_iface.h"

#include "snmp_iface.h"

/* 
 * Allied Telesyn Switch Interface
 */

/* Object Factory Fabrication */

int v_snmp_iface_objfact_fab (i_resource *self, i_container *cnt, i_object *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata)
{
  i_metric *met;

  /* Call l_snmp_iface fabrictator */
  l_snmp_iface_objfact_fab (self, cnt, obj, pdu, index_oidstr, passdata);

  /* Re-config the object */
  obj->mainform_func = v_snmp_iface_objform;
  obj->histform_func = v_snmp_iface_objform_hist;  
  
  /* 
   * Metric Creation 
   */

  /* Duplex */
  met = l_snmp_metric_create (self, obj, "duplex", "Duplex", METRIC_INTEGER, ".1.3.6.1.4.1.207.8.4.4.4.23.1.1.4", index_oidstr, RECMETHOD_NONE, 0);
  i_metric_enumstr_add (met, 1, "Full");
  i_metric_enumstr_add (met, 2, "Half");
  i_metric_enumstr_add (met, 3, "Unknown");

  return 0;
}

/* Object Factory Control Func
 *
 * Called when the Object Factory has completed a refresh op
 */

int v_snmp_iface_objfact_ctrl (i_resource *self, i_container *cnt, int result, void *passdata)
{
  return l_snmp_iface_objfact_ctrl (self, cnt, result, passdata);
}

/* Object Factory Clean Func
 *
 * Called when an object is obsolete prior to it being deregistered and free
 */

int v_snmp_iface_objfact_clean (i_resource *self, i_container *cnt, i_object *obj)
{
  return l_snmp_iface_objfact_clean (self, cnt, obj);
}
