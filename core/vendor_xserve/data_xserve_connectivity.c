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

#include "cpu.h"
#include "volumes.h"
#include "network.h"
#include "plist.h"
#include "data.h"

/*
 * Xserver Connectivity Processing
 */

int v_data_xserve_process_connectivity (i_resource *self, v_plist_req *req, xmlNodePtr connNode)
{
  /* Get Info Node */
  xmlNodePtr infoArray = v_plist_node_from_dict (req->plist, connNode, "Info");
  if (!infoArray) return 0;

  /* Get Detail Node */
  xmlNodePtr detailArray = v_plist_node_from_dict (req->plist, connNode, "Detail");
  if (!detailArray) return 0;

  /* Process info node */
  int index = 0;
  xmlNodePtr node;
  for (node = infoArray->children; node; node = node->next)
  {
    /* Look through each network iface info dict */
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get iface item */
      i_metric_value *val;
      v_network_item *iface;
      char *iface_name = v_plist_data_from_dict (req->plist, node, "Name");
      if (!iface_name) continue; 
      iface = v_network_get (iface_name);
      if (!iface)
      { i_printf (1, "v_data_xserve_process_connectivity failed to locate interfacd %s", iface_name); free (iface_name); continue; }
      free (iface_name);
      iface_name = NULL;

      /* MAC */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "HWAddress");
      i_metric_value_enqueue (self, iface->mac, val);
      iface->mac->refresh_result = REFRESULT_OK;

      /* Kind */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "Kind");
      i_metric_value_enqueue (self, iface->kind, val);
      iface->kind->refresh_result = REFRESULT_OK;

      /* Process detail */
      v_data_xserve_process_connectivity_detail (self, req, iface, detailArray, index);

      /* Increment index */
      index++;
    }
  }

  return 0;
}

int v_data_xserve_process_connectivity_detail (i_resource *self, v_plist_req *req, v_network_item *iface, xmlNodePtr detailNode, int index)
{
  /* Processes the info in the connectvity info array for the
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

        /* Bytes In */
        str = v_plist_data_from_dict (req->plist, node, "BytesIn");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = (double) strtoul (str, NULL, 10);
          i_metric_value_enqueue (self, iface->bytes_in, val);
          iface->bytes_in->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(iface->bytes_in));
          free (str);
          str = NULL;
        }

        /* Bytes Out */
        str = v_plist_data_from_dict (req->plist, node, "BytesOut");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = (double) strtoul (str, NULL, 10);
          i_metric_value_enqueue (self, iface->bytes_out, val);
          iface->bytes_out->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(iface->bytes_out));
          free (str);
          str = NULL;
        }

        /* Packets In */
        str = v_plist_data_from_dict (req->plist, node, "PacketsIn");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = (double) strtoul (str, NULL, 10);
          i_metric_value_enqueue (self, iface->packets_in, val);
          iface->packets_in->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(iface->packets_in));
          free (str);
          str = NULL;
        }

        /* Packets Out */
        str = v_plist_data_from_dict (req->plist, node, "PacketsOut");
        if (str)
        {
          val = i_metric_value_create ();
          val->flt = (double) strtoul (str, NULL, 10);
          i_metric_value_enqueue (self, iface->packets_out, val);
          iface->packets_out->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(iface->packets_out));
          free (str);
          str = NULL;
        }

        /* Link State */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, node, "Link");
        i_metric_value_enqueue (self, iface->linkstate, val);
        iface->linkstate->refresh_result = REFRESULT_OK;

        /* Duplex Mode */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, node, "DuplexMode");
        i_metric_value_enqueue (self, iface->duplex, val);
        iface->duplex->refresh_result = REFRESULT_OK;

        /* Speed */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, node, "Mbps");
        i_metric_value_enqueue (self, iface->speed, val);
        iface->speed->refresh_result = REFRESULT_OK;

        return 0;
      }
      i++;
    }
  }

  return -1;
}

