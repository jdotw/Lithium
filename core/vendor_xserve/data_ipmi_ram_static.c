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
#include "ram.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "data.h"

/*
 * IPMI Apple static ram data
 */

static int static_ram_index = 0;
static v_ipmitool_apple_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_ram_static_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_ram_index = 0;
      static_req = v_ipmitool_apple_get (self, "0xc2", "0x00", v_data_ipmi_ram_static_ipmicb, met);
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

int v_data_ipmi_ram_static_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
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

  /* ram variables */  
  i_metric_value *val;
  size_t offset;
  char *desc_str = NULL;
  char *speed_str = NULL;
  char *type_str = NULL;

  /* Get Descr */
  offset = 9;
  desc_str = v_ipmitool_string (req->data, req->datasize, offset);
  if (desc_str && strlen(desc_str) > 0)
  {
    /* desc is present, ram slot is populated */
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

    speed_str= v_ipmitool_string (req->data, req->datasize, offset);
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;
  
    type_str = v_ipmitool_string (req->data, req->datasize, offset);
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

    /* Get ram */
    v_ram_item *ram = v_ram_get (desc_str);
    if (!ram)
    { ram = v_ram_create (self, desc_str); }
    free (desc_str);

    if (speed_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (speed_str);
      i_metric_value_enqueue (self, ram->speed, val);
      ram->speed->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(ram->speed));
      free (speed_str);
    }

    if (type_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (type_str);
      i_metric_value_enqueue (self, ram->type, val);
      ram->type->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(ram->type));
      free (type_str);
    }

    val = i_metric_value_create ();
    val->gauge = (unsigned long) v_ipmitool_uint32 (req->data, req->datasize, 4);
    i_metric_value_enqueue (self, ram->size, val);
    ram->size->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(ram->size));
  }
  else if (desc_str) free (desc_str);

  /* Kick off refresh for next item */
  static_ram_index++;
  if (static_ram_index >= 8)
  {
    /* Maximum index reached, no more RAM slots */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    return 0;
  } 
  char *index_str;
  asprintf (&index_str, "0x%.2x", static_ram_index);
  static_req = v_ipmitool_apple_get (self, "0xc2", index_str, v_data_ipmi_ram_static_ipmicb, datamet);
  free (index_str);
  if (!static_req)
  {
    /* Failed to refrsh next */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    return -1;
  }

  return 0;
}

