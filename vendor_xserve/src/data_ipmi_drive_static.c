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
#include <lithium/snmp.h>

#include "fans.h"
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "drives.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "data.h"

/*
 * IPMI Apple static drive data
 */

static int static_drive_index = 0;
static v_ipmitool_apple_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_drive_static_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_drive_index = 0;
      static_req = v_ipmitool_apple_get (self, "0xc3", "0x00", v_data_ipmi_drive_static_ipmicb, met);
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

int v_data_ipmi_drive_static_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
{
  i_metric *datamet = passdata;

  /* Check for data */
  if (!req->data || !req->datasize || result != IPMIRESULT_OK)
  {
    /* No data, error */
    /* Set result and terminate */
    i_printf (1, "v_data_ipmi_drive_static_ipmicb failed to refresh, no data returned");
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    return -1;
  }

  /* Drive variables */  
  i_metric_value *val;
  size_t offset;
  char *diskid_str = NULL;
  char *vendor_str = NULL;
  char *model_str = NULL;
  char *interconnect_str = NULL;
  char *location_str = NULL;

  /* Get drive desc */
  offset = 11;
  diskid_str = v_ipmitool_string (req->data, req->datasize, offset);
  if (!diskid_str || strlen(diskid_str) < 1)
  {
    /* No drive desc, must have reached the end of the drive list */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    return 0;
  }
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  vendor_str= v_ipmitool_string (req->data, req->datasize, offset);
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  model_str = v_ipmitool_string (req->data, req->datasize, offset);
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;
  
  interconnect_str = v_ipmitool_string (req->data, req->datasize, offset);
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  location_str = v_ipmitool_string (req->data, req->datasize, offset);
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  /* Get drive */
  if (location_str)
  {
    v_drives_item *drive = v_drives_get (location_str);
    if (!drive)
    { drive = v_drives_create (self, location_str); }
    if (location_str) free (location_str);

    if (vendor_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (vendor_str);
      i_metric_value_enqueue (self, drive->vendor, val);
      drive->vendor->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(drive->vendor));
      free (vendor_str);
    }

    if (model_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (model_str);
      i_metric_value_enqueue (self, drive->model, val);
      drive->model->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(drive->model));
      free (model_str);
    }

    if (interconnect_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (interconnect_str);
      i_metric_value_enqueue (self, drive->interconnect, val);
      drive->interconnect->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(drive->interconnect));
      free (interconnect_str);
    }

    if (diskid_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (diskid_str);
      i_metric_value_enqueue (self, drive->diskid, val);
      drive->diskid->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(drive->diskid));
      free (diskid_str);
    }

    val = i_metric_value_create ();
    val->gauge = (v_ipmitool_uint32 (req->data, req->datasize, 3) * 256);
    i_metric_value_enqueue (self, drive->capacity, val);
    drive->capacity->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(drive->capacity));
  }
  
  /* Kick off refresh for next item */
  static_drive_index++;
  if (static_drive_index > 6)
  {
    /* Maximum index reached */
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    i_printf (1, "v_data_ipmi_drive_static_ipmicb failed, maximum index reached");
    return -1;
  } 
  char *index_str;
  asprintf (&index_str, "0x%.2x", static_drive_index);
  static_req = v_ipmitool_apple_get (self, "0xc3", index_str, v_data_ipmi_drive_static_ipmicb, datamet);
  free (index_str);
  if (!static_req)
  {
    /* Failed to refrsh next */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    i_printf (0, "v_data_ipmi_drive_static_ipmicb failed to start refresh of item index %i", static_drive_index);
    return -1;
  }

  return 0;
}

