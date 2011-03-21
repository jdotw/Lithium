#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/socket.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/snmp.h"

#include "sysinfo.h"
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "drives.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "data.h"

/*
 * IPMI Apple CPU data
 */

static v_ipmitool_apple_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_cpu_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_req = v_ipmitool_apple_get (self, "0xc0", "0x00", v_data_ipmi_cpu_ipmicb, met);
      break;

    case REFOP_COLLISION:   /* Handle collision */
      break;

    case REFOP_TERMINATE:   /* Terminate the refresh */
      if (static_req)
      {
        /* FIX must cancel request */
        static_req = NULL;
      }
      break;

    case REFOP_CLEANDATA:   /* Cleanup persistent refresh data */
      break;
  }

  return 0;
}

int v_data_ipmi_cpu_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
{
  i_metric *datamet = passdata;

  /* Check for data */
  if (!req->data || !req->datasize || result != IPMIRESULT_OK)
  {
    /* No data, error */
    /* Set result and terminate */
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    return -1;
  }

  /* variables */  
  i_metric_value *val;
  size_t offset;
  char *type_str = NULL;
  v_sysinfo_item *sysinfo = v_sysinfo_static_item ();

  /* Get CPU desc */
  offset = 15;
  type_str= v_ipmitool_string (req->data, req->datasize, offset);
  if (!type_str || strlen(type_str) < 1)
  {
    /* No drive desc, must have reached the end of the drive list */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    return 0;
  }
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  if (type_str)
  {
    val = i_metric_value_create ();
    val->str = strdup (type_str);
    i_metric_value_enqueue (self, sysinfo->cpu_type, val);
    sysinfo->cpu_type->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(sysinfo->cpu_type));
    free (type_str);
  }

  val = i_metric_value_create ();
  val->gauge = (v_ipmitool_uint32 (req->data, req->datasize, 2));
  i_metric_value_enqueue (self, sysinfo->cpu_count, val);
  sysinfo->cpu_count->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(sysinfo->cpu_count));

  val = i_metric_value_create ();
  val->gauge = (v_ipmitool_uint32 (req->data, req->datasize, 6));
  i_metric_value_enqueue (self, sysinfo->cpu_speed, val);
  sysinfo->cpu_speed->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(sysinfo->cpu_speed));

  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));

  return 0;
}

