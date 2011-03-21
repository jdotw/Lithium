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

#include "chassis.h"
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "drives.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "data.h"

/*
 * IPMI Apple serial number data
 */

static v_ipmitool_apple_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_serial_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_req = v_ipmitool_apple_get (self, "0xc1", "0x00", v_data_ipmi_serial_ipmicb, met);
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

int v_data_ipmi_serial_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
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
  char *model_str = NULL;
  char *serial_str = NULL;
  v_chassis_item *chassis = v_chassis_static_item ();

  /* Get drive desc */
  offset = 7;
  model_str= v_ipmitool_string (req->data, req->datasize, offset);
  if (!model_str || strlen(model_str) < 1)
  {
    /* No drive desc, must have reached the end of the drive list */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    return 0;
  }
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  serial_str= v_ipmitool_string (req->data, req->datasize, offset);
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;
  
  if (model_str)
  {
    val = i_metric_value_create ();
    val->str = strdup (model_str);
    i_metric_value_enqueue (self, chassis->model, val);
    chassis->model->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(chassis->model));
    free (model_str);
  }

  if (serial_str)
  {
    val = i_metric_value_create ();
    val->str = strdup (serial_str);
    i_metric_value_enqueue (self, chassis->serial, val);
    chassis->serial->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(chassis->serial));
    free (serial_str);
  }

  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));

  return 0;
}

