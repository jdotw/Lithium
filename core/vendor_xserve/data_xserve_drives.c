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

#include "cpu.h"
#include "drives.h"
#include "plist.h"
#include "data.h"

/*
 * Xserver Drives Processing
 */

int v_data_xserve_process_drives (i_resource *self, v_plist_req *req, xmlNodePtr drivesArrayNode)
{
  /* Get Info Node */
  xmlNodePtr infoArray = v_plist_node_from_dict (req->plist, drivesArrayNode, "Info");
  if (!infoArray) return 0;

  /* Get Detail Node */
  xmlNodePtr detailArray = v_plist_node_from_dict (req->plist, drivesArrayNode, "Detail");
  if (!detailArray) return 0;

  /* Process info node */
  int index = 0;
  xmlNodePtr node;
  for (node = infoArray->children; node; node = node->next)
  {
    /* Look through each network drive info dict */
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get drive item */
      i_metric_value *val;
      v_drives_item *drive;
      char *drive_desc = v_plist_data_from_dict (req->plist, node, "Kind");
      if (!drive_desc) continue; 
      drive = v_drives_get (drive_desc);
      if (!drive)
      {
        drive = v_drives_create (self, drive_desc);
        if (!drive)
        { i_printf (1, "v_data_xserve_process_drives failed to create drive for %s", drive_desc); free (drive_desc); continue; }
      }
      free (drive_desc);
      drive_desc = NULL;

      /* Capacity */
      val = i_metric_value_create ();
      val->gauge = v_plist_int_from_dict (req->plist, node, "Capacity");
      i_metric_value_enqueue (self, drive->capacity, val);
      drive->capacity->refresh_result = REFRESULT_OK;

      /* Interconnect */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Interconnect");
      i_metric_value_enqueue (self, drive->interconnect, val);
      drive->interconnect->refresh_result = REFRESULT_OK;

      /* Disk ID (Kind) */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Kind");
      i_metric_value_enqueue (self, drive->diskid, val);
      drive->diskid->refresh_result = REFRESULT_OK;
      
      /* Disk ID (Kind) */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Location");
      i_metric_value_enqueue (self, drive->location, val);
      drive->location->refresh_result = REFRESULT_OK;

      /* Vendor */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Manufacturer");
      i_metric_value_enqueue (self, drive->vendor, val);
      drive->vendor->refresh_result = REFRESULT_OK;

      /* Model */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Model");
      i_metric_value_enqueue (self, drive->model, val);
      drive->model->refresh_result = REFRESULT_OK;

      /* Process detail */
      v_data_xserve_process_drives_detail (self, req, drive, detailArray, index);

      /* Increment index */
      index++;
    }
  }

  return 0;
}

int v_data_xserve_process_drives_detail (i_resource *self, v_plist_req *req, v_drives_item *drive, xmlNodePtr detailNode, int index)
{
  /* Processes the info in the drives detail array for the
   * specified interface at the index 
   */

  int i = 0;
  xmlNodePtr node;
  for (node = detailNode->children; node; node = node->next)
  {
    /* Look through each network iface info dict */
    if (strcmp((char *)node->name, "dict") == 0)
    {
      if (i == index)
      {
        /* Corresponding detail dictionary found, process it */
        char *str;
        i_metric_value *val;

        /* Bytes Read */
        str = v_plist_data_from_dict (req->plist, node, "BytesRead");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = atof (str);
          i_metric_value_enqueue (self, drive->bytes_read, val);
          drive->bytes_read->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(drive->bytes_read));
          free (str);
          str = NULL;
        }

        /* Bytes Written */
        str = v_plist_data_from_dict (req->plist, node, "BytesWritten");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = atof (str);
          i_metric_value_enqueue (self, drive->bytes_written, val);
          drive->bytes_written->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(drive->bytes_written));
          free (str);
          str = NULL;
        }

        /* Read Errors */
        str = v_plist_data_from_dict (req->plist, node, "ReadErrors");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = atof (str);
          i_metric_value_enqueue (self, drive->read_errors, val);
          drive->read_errors->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(drive->read_errors));
          free (str);
          str = NULL;
        }

        /* Write Errors */
        str = v_plist_data_from_dict (req->plist, node, "WriteErrors");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = atof (str);
          i_metric_value_enqueue (self, drive->write_errors, val);
          drive->write_errors->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(drive->write_errors));
          free (str);
          str = NULL;
        }

        /* SMART */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, node, "SMARTMessage");
        i_metric_value_enqueue (self, drive->smart_desc, val);
        drive->smart_desc->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(drive->smart_desc));

        return 0;
      }
      i++;
    }
  }

  return -1;
}

