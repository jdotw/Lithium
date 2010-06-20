#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <lithium/snmp.h>
#include <lithium/snmp_iface.h>
#include <lithium/record.h>

#include "snmp_iface.h"

/* Allied Telesyn Interfaces
 *
 * This is a wrapper for the standard snmp_iface code that
 * adds support for AT-specific iface info such as Duplex
 * 
 */

/* Pointer retrieval */

i_container* v_snmp_iface_cnt ()
{ return l_snmp_iface_cnt(); }

l_snmp_objfact* v_snmp_iface_objfact ()
{ return l_snmp_iface_objfact(); }

/* Enable / Disable */

int v_snmp_iface_enable (i_resource *self)
{
  int num;
  i_container *cnt;
  l_snmp_objfact *objfact;
  
  /* Enable iface */
  num = l_snmp_iface_enable (self);
  if (num != 0)
  { i_printf (0, "v_snmp_iface_enable failed to enable l_snmp_iface"); return -1; }

  /* Change forms */
  cnt = v_snmp_iface_cnt();
  cnt->mainform_func = v_snmp_iface_cntform;
  cnt->sumform_func = v_snmp_iface_cntform;

  /* Reconfigure the object factory */
  objfact = v_snmp_iface_objfact ();
  objfact->fabfunc = v_snmp_iface_objfact_fab;
  objfact->ctrlfunc = v_snmp_iface_objfact_ctrl;
  objfact->cleanfunc = v_snmp_iface_objfact_clean;

  return 0;
}

int v_snmp_iface_disable (i_resource *self)
{
  return l_snmp_iface_disable (self);
}

