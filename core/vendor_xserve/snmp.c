#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/hashtable.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/entity.h"
#include "induction/device.h"
#include "induction/object.h"
#include "induction/hierarchy.h"
#include "induction/postgresql.h"
#include "induction/interface.h"
#include "induction/inventory.h"
#include "induction/ip.h"
#include "induction/construct.h"
#include "device/avail.h"
#include "device/snmp.h"
#include "device/snmp_sysinfo.h"
#include "device/snmp_iface.h"
#include "device/snmp_ipaddr.h"
#include "device/snmp_swrun.h"
#include "device/snmp_storage.h"
#include "device/snmp_hrcpu.h"
#include "device/snmp_users.h"
#include "device/icmp.h"
#include "device/snmp_nscpu.h"
#include "device/snmp_nsload.h"
#include "device/snmp_nsdiskio.h"
#include "device/snmp_nsram.h"
#include "device/snmp_nsnwtput.h"
#include "device/snmp_nstcpconn.h"
#include "device/xsnmp_raidstatus.h"
#include "device/xsnmp_raidset.h"
#include "device/xsnmp_raiddrive.h"
#include "device/xsnmp_raidvolume.h"
#include "device/procpro.h"

#include "snmp.h"
#include "xsanvol.h"
#include "xsanvisdisk.h"

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
  { 
    v_xsanvol_enable(self); 
    // v_xsanvisdisk_enable(self); 
    l_xsnmp_raidstatus_enable(self); 
    l_xsnmp_raidset_enable(self); 
    l_xsnmp_raiddrive_enable(self); 
    l_xsnmp_raidvolume_enable(self); 
  }

  return 0;
}
