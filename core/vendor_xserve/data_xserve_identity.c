#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/socket.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include "device/snmp.h"

#include "sysinfo.h"
#include "plist.h"
#include "data.h"

/*
 * Xserver Identity Processing
 */

int v_data_xserve_process_identity (i_resource *self, v_plist_req *req, xmlNodePtr identityNode)
{
  i_metric_value *val;
  v_sysinfo_item *sysinfo = v_sysinfo_static_item ();
  
  /* Boot ROM */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (req->plist, identityNode, "BootROM");
  i_metric_value_enqueue (self, sysinfo->bootrom, val);
  sysinfo->bootrom->refresh_result = REFRESULT_OK;

  /* CPU Count */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, identityNode, "CPUs");
  i_metric_value_enqueue (self, sysinfo->cpu_count, val);
  sysinfo->cpu_count->refresh_result = REFRESULT_OK;
  
  /* CPU L2Cache */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, identityNode, "L2Cache");
  i_metric_value_enqueue (self, sysinfo->cpu_l2cache, val);
  sysinfo->cpu_l2cache->refresh_result = REFRESULT_OK;

  /* CPU Speed */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, identityNode, "MHz");
  i_metric_value_enqueue (self, sysinfo->cpu_speed, val);
  sysinfo->cpu_speed->refresh_result = REFRESULT_OK;

  /* RAM */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, identityNode, "RAM");
  i_metric_value_enqueue (self, sysinfo->ram_size, val);
  sysinfo->ram_size->refresh_result = REFRESULT_OK;

  return 0;
}

