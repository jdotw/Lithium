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

int v_data_ipmi_drive_dynamic_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_drive_index = 0;
      static_req = v_ipmitool_apple_get (self, "0xc5", "0x00", v_data_ipmi_drive_dynamic_ipmicb, met);
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

int v_data_ipmi_drive_dynamic_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
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

  /* Get drive */
  int i = 0;
  i_container *cnt = v_drives_cnt ();
  i_object *obj;
  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    if (i == static_drive_index) break;
    i++;
  }

  /* Process drive */
  if (obj)
  {
    v_drives_item *drive = obj->itemptr;
    i_metric_value *val;
    size_t offset;
    char *smart_str = NULL;
    
    /* Get SMART Status */
    offset = 39;
    smart_str = v_ipmitool_string (req->data, req->datasize, offset);
    if (smart_str && strlen(smart_str) > 0)
    {
      val = i_metric_value_create ();
      val->str = strdup (smart_str);
      i_metric_value_enqueue (self, drive->smart_desc, val);
      drive->smart_desc->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(drive->smart_desc));
      free (smart_str);
    }
    else if (smart_str) free (smart_str);
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

    char *raid_desc_str = v_ipmitool_string (req->data, req->datasize, offset);
    if (raid_desc_str && strlen(raid_desc_str) > 0)
    {
      val = i_metric_value_create ();
      val->str = strdup(raid_desc_str);
      i_metric_value_enqueue (self, drive->raid_desc, val);
      drive->raid_desc->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(drive->raid_desc));
      free (raid_desc_str);

      drive->raid_desc->hidden = 0;
      drive->raid_status->hidden = 0;
    }
    else
    {
      drive->raid_desc->hidden = 1;
      drive->raid_status->hidden = 1;
    }
    
    uint32_t val256 = v_ipmitool_uint32 (req->data, req->datasize, 3);
    val = i_metric_value_create ();
    val->flt = (float) val256 * 256.0;
    i_metric_value_enqueue (self, drive->bytes_read, val);
    drive->bytes_read->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(drive->bytes_read));
    
    val256 = v_ipmitool_uint32 (req->data, req->datasize, 11);
    val = i_metric_value_create ();
    val->flt = (float) val256 * 256.0;
    i_metric_value_enqueue (self, drive->bytes_written, val);
    drive->bytes_written->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(drive->bytes_written));

    val = i_metric_value_create ();
    val->integer = v_ipmitool_octet(req->data, req->datasize, 26);
    i_metric_value_enqueue(self, drive->raid_status, val);
    drive->raid_status->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(drive->raid_status));
    
    /* Kick off refresh for next item */
    static_drive_index++;
    if (static_drive_index > 6)
    {
      /* Maximum index reached */
      datamet->refresh_result = REFRESULT_TOTAL_FAIL;
      i_entity_refresh_terminate (ENTITY(datamet));
      i_printf (1, "v_data_ipmi_drive_dynamic_ipmicb failed, maximum index reached");
      return -1;
    }
    char *index_str;
    asprintf (&index_str, "0x%.2x", static_drive_index);
    static_req = v_ipmitool_apple_get (self, "0xc5", index_str, v_data_ipmi_drive_dynamic_ipmicb, datamet);
    free (index_str);
    if (!static_req)
    {
      /* Failed to refrsh next */
      datamet->refresh_result = REFRESULT_TOTAL_FAIL;
      i_entity_refresh_terminate (ENTITY(datamet));
      i_printf (1, "v_data_ipmi_drive_dynamic_ipmicb failed to start refresh of item index %i", static_drive_index);
      return -1;
    }
  }
  else
  {
    /* Drive not found, end of refresh */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
  }

  return 0;
}

