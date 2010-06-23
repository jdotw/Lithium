#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/object.h>
#include <induction/hierarchy.h>
#include <induction/postgresql.h>
#include <induction/interface.h>
#include <induction/inventory.h>
#include <induction/ip.h>
#include <induction/construct.h>
#include <lithium/avail.h>
#include <lithium/snmp.h>
#include <lithium/snmp_sysinfo.h>
#include <lithium/snmp_iface.h>
#include <lithium/snmp_ipaddr.h>
#include <lithium/snmp_swrun.h>
#include <lithium/snmp_storage.h>
#include <lithium/snmp_hrcpu.h>
#include <lithium/snmp_users.h>
#include <lithium/icmp.h>
#include <lithium/snmp_nscpu.h>
#include <lithium/snmp_nsload.h>
#include <lithium/snmp_nsdiskio.h>
#include <lithium/snmp_nsram.h>
#include <lithium/snmp_nsnwtput.h>
#include <lithium/snmp_nstcpconn.h>
#include <lithium/procpro.h>


#include "snmp.h"

int v_snmp_enable (i_resource *self)
{
  /*
   * Enable snmp sub-systems for monitoring Mac OS X Server using SNMP
   * instead of the ServerMgrd interface
   */

  int num;

  num = l_snmp_objfact_init (self, self->hierarchy->dev);
  num = l_snmp_sysinfo_enable (self);
  num = l_snmp_nscpu_enable (self);
  num = l_snmp_hrcpu_enable (self);
  num = l_snmp_nsload_enable (self);
  num = l_snmp_nsram_enable (self);
  num = l_snmp_storage_enable (self);
  num = l_snmp_nsdiskio_enable (self);
//  num = l_snmp_nsnwtput_enable (self);
  num = l_snmp_iface_enable (self);
  num = l_snmp_ipaddr_enable (self);
  if (self->hierarchy->dev->swrun)
  { 
    num = l_snmp_swrun_enable (self); 
    num = l_procpro_enable (self);
  }

  if (l_snmp_xsnmp_enabled())
  { v_xsanvol_enable(); }

  return 0;
}
