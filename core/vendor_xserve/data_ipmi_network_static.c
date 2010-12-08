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

#include "fans.h"
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "network.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "data.h"

/*
 * IPMI Apple static network data
 */

static int static_interface_index = 0;
static v_ipmitool_apple_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_network_static_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_interface_index = 0;
      static_req = v_ipmitool_apple_get (self, "0xc4", "0x00", v_data_ipmi_network_static_ipmicb, met);
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

int v_data_ipmi_network_static_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
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

  /* network variables */  
  i_metric_value *val;
  size_t offset;
  char *mac_str = NULL;
  char *fullname_str = NULL;
  char *desc_str = NULL;

  /* Get MAC */
  offset = 3;
  mac_str = v_ipmitool_string (req->data, req->datasize, offset);
  if (!mac_str || strlen(mac_str) < 1)
  {
    /* No MAC, must have reached the end of the network list */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    return 0;
  }
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  fullname_str= v_ipmitool_string (req->data, req->datasize, offset);
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  desc_str = v_ipmitool_string (req->data, req->datasize, offset);
  offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

  /* Get iface */
  if (desc_str)
  {
    v_network_item *iface = v_network_get (desc_str);
    if (!iface)
    { iface = v_network_create (self, desc_str); }
    free (desc_str);
    if (!iface) return -1;

    if (fullname_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (fullname_str);
      i_metric_value_enqueue (self, iface->fullname, val);
      iface->fullname->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(iface->fullname));
      free (fullname_str);
    }

    if (mac_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (mac_str);
      i_metric_value_enqueue (self, iface->mac, val);
      iface->mac->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(iface->mac));
      free (mac_str);
    }
  }
  else
  {
    i_printf (1, "v_data_ipmi_network_static_ipmicb did not receive an interface description. Received values are fullname_str=%s mac=%s desc=%s", fullname_str, mac_str, desc_str); 
  }

  /* Kick off refresh for next item */
  static_interface_index++;
  if (static_interface_index > 6)
  {
    /* Maximum index reached */
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    i_printf (1, "v_data_ipmi_network_static_ipmicb failed, maximum index reached");
    return -1;
  } 
  char *index_str;
  asprintf (&index_str, "0x%.2x", static_interface_index);
  static_req = v_ipmitool_apple_get (self, "0xc4", index_str, v_data_ipmi_network_static_ipmicb, datamet);
  free (index_str);
  if (!static_req)
  {
    /* Failed to refrsh next */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
    i_printf (0, "v_data_ipmi_network_static_ipmicb failed to start refresh of item index %i", static_interface_index);
    return -1;
  }

  return 0;
}

