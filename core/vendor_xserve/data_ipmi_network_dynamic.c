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

#include "fans.h"
#include "cpu.h"
#include "pci.h"
#include "psu.h"
#include "network.h"
#include "mainboard.h"
#include "ipmitool.h"
#include "csv.h"
#include "plist.h"
#include "data.h"

/*
 * IPMI Apple static network data
 */

static int static_interface_index = 0;
static v_ipmitool_apple_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_ipmi_network_dynamic_refresh (i_resource *self, i_metric *met, int opcode)
{
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh process */
      static_interface_index = 0;
      static_req = v_ipmitool_apple_get (self, "0xc6", "0x00", v_data_ipmi_network_dynamic_ipmicb, met);
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

int v_data_ipmi_network_dynamic_ipmicb (i_resource *self, v_ipmitool_apple_req *req, int result, void *passdata)
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

  /* Check for empty data */
  if (req->datasize < 60)
  {
    /* Single-response received, it's empty */
    datamet->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(datamet));
  }

  /* Get iface */
  int i = 0;
  i_container *cnt = v_network_cnt ();
  if (!cnt) return -1;
  i_object *obj;
  for (i_list_move_head(cnt->obj_list); (obj=i_list_restore(cnt->obj_list))!=NULL; i_list_move_next(cnt->obj_list))
  {
    if (i == static_interface_index) break;
    i++;
  }

  /* Process iface */
  if (obj)
  {
    v_network_item *iface = obj->itemptr;
    i_metric_value *val;
    size_t offset;
    char *ip_str = NULL;
    char *subnet_str = NULL;
    char *status_str = NULL;
    char *speed_str = NULL;
    char *duplex_str = NULL;
    
    /* Get IP */
    offset = 23;
    ip_str = v_ipmitool_string (req->data, req->datasize, offset);
    if (ip_str)
    {
      /* IF first iface, use as plist IP */
//      if (static_interface_index == 0 && strlen(ip_str) > 1 && strcmp(v_plist_ip(), ip_str) != 0)
//      { v_plist_set_ip (ip_str); }

      /* Set metric value */
      val = i_metric_value_create ();
      val->str = strdup (ip_str);
      i_metric_value_enqueue (self, iface->ip, val);
      iface->ip->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(iface->ip));
      free (ip_str);
    }
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

    /* Get Subnet Mask */
    subnet_str = v_ipmitool_string (req->data, req->datasize, offset);
    if (subnet_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (subnet_str);
      i_metric_value_enqueue (self, iface->subnetmask, val);
      iface->subnetmask->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(iface->subnetmask));
      free (subnet_str);
    }
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

    /* Get Status */
    status_str = v_ipmitool_string (req->data, req->datasize, offset);
    if (status_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (status_str);
      i_metric_value_enqueue (self, iface->linkstate, val);
      iface->linkstate->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(iface->linkstate));
      free (status_str);
    }
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

    /* Get Speed Mask */
    speed_str = v_ipmitool_string (req->data, req->datasize, offset);
    if (speed_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (speed_str);
      i_metric_value_enqueue (self, iface->speed, val);
      iface->speed->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(iface->speed));
      free (speed_str);
    }
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;

    /* Get Duplex */
    duplex_str = v_ipmitool_string (req->data, req->datasize, offset);
    if (duplex_str)
    {
      val = i_metric_value_create ();
      val->str = strdup (duplex_str);
      i_metric_value_enqueue (self, iface->duplex, val);
      iface->duplex->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(iface->duplex));
      free (duplex_str);
    }
    offset += v_ipmitool_octet (req->data, req->datasize, offset) + 1;
    
    val = i_metric_value_create ();
    val->flt = (float) v_ipmitool_uint32 (req->data, req->datasize, 2);
    i_metric_value_enqueue (self, iface->packets_in, val);
    iface->packets_in->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(iface->packets_in));
    
    val = i_metric_value_create ();
    val->flt = (float) v_ipmitool_uint32 (req->data, req->datasize, 6);
    i_metric_value_enqueue (self, iface->packets_out, val);
    iface->packets_out->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(iface->packets_out));
    
    val = i_metric_value_create ();
    val->flt = (float) v_ipmitool_uint32 (req->data, req->datasize, 10);
    i_metric_value_enqueue (self, iface->bytes_in, val);
    iface->bytes_in->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(iface->bytes_in));
    
    val = i_metric_value_create ();
    val->flt = (float) v_ipmitool_uint32 (req->data, req->datasize, 14);
    i_metric_value_enqueue (self, iface->bytes_out, val);
    iface->bytes_out->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(iface->bytes_out));
    
    /* Kick off refresh for next item */
    static_interface_index++;
    if (static_interface_index > 6)
    {
      /* Maximum index reached */
      datamet->refresh_result = REFRESULT_TOTAL_FAIL;
      i_entity_refresh_terminate (ENTITY(datamet));
      i_printf (1, "v_data_ipmi_network_dynamic_ipmicb failed, maximum index reached");
      return -1;
    }  
    char *index_str;
    asprintf (&index_str, "0x%.2x", static_interface_index);
    static_req = v_ipmitool_apple_get (self, "0xc6", index_str, v_data_ipmi_network_dynamic_ipmicb, datamet);
    free (index_str);
    if (!static_req)
    {
      /* Failed to refrsh next */
      datamet->refresh_result = REFRESULT_TOTAL_FAIL;
      i_entity_refresh_terminate (ENTITY(datamet));
      i_printf (0, "v_data_ipmi_network_dynamic_ipmicb failed to start refresh of item index %i", static_interface_index);
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

