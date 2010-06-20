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

#include "osx_server.h"
#include "network.h"
#include "plist.h"
#include "data.h"

/*
 * Mac OS X "network getState"
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_network_state_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysnetwork data */
  char *url;
  char *command = "/commands/servermgr_network?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetState%3C%2Fstring%3E%0D%0A%09%3Ckey%3Evariant%3C%2Fkey%3E%0D%0A%09%09%3Cstring%3EwithDetails%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command";
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      if (v_powerstate()) 
      {
        asprintf (&url, "https://%s:311/%s", v_plist_ip(), command);
        static_req = v_plist_get (self, url, v_data_network_state_plistcb, met);
        free (url);
      }
      else return 1;
      break;

    case REFOP_COLLISION:   /* Handle collision */
      break;

    case REFOP_TERMINATE:   /* Terminate the refresh */
      if (static_req)
      {
        v_plist_get_cancel (self, static_req);
        static_req = NULL;
      }
      break;

    case REFOP_CLEANDATA:   /* Cleanup persistent refresh data */
      break;
  }

  return 0;
}

int v_data_network_state_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_metric *datamet = passdata;

  /* Clear req */
  static_req = NULL;

  /* Check result */
  if (!req)
  { return -1; }

  /* Parse XML plist */
  xmlNodePtr node;
  for (node = req->root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get interfaces array node */
      xmlNodePtr ifaceArrayNode = v_plist_node_from_dict (req->plist, node, "interfaces");
      if (ifaceArrayNode)
      { v_data_network_state_process_ifacearray (self, req, ifaceArrayNode); }
    }
  }

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}

int v_data_network_state_process_ifacearray (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  xmlNodePtr node;
  for (node = arrayNode->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      i_metric_value *val;
      xmlNodePtr listNode;

      /* Interface name */
      char *name_str = v_plist_data_from_dict (req->plist, node, "name");
      if (!name_str) continue;
      v_network_item *iface = v_network_get (name_str);
      if (!iface)
      {
        iface = v_network_create (self, name_str);
        if (!iface)
        { i_printf (1, "v_data_network_state_process_ifacearray failed to create network interface object for %s", name_str); free (name_str); continue; }
      }
      free (name_str);

      /* Descriptive Name */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "descriptiveName");
      if (val->str)
      {
        i_metric_value_enqueue (self, iface->fullname, val);
        iface->fullname->refresh_result = REFRESULT_OK;
      }
      else { i_metric_value_free (val); }
      
      /* IP Address  */
      listNode = v_plist_node_from_dict (req->plist, node, "ipv4Addresses");
      if (listNode)
      {
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, listNode, NULL);
        if (val->str)
        {
          i_metric_value_enqueue (self, iface->ip, val);
          iface->ip->refresh_result = REFRESULT_OK;
        }
        else { i_metric_value_free (val); }
      }

      /* Broadcast */
      listNode = v_plist_node_from_dict (req->plist, node, "ipv4BroadcastAddresses");
      if (listNode)
      {
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, listNode, NULL);
        if (val->str)
        {
          i_metric_value_enqueue (self, iface->broadcast, val);
          iface->broadcast->refresh_result = REFRESULT_OK;
        }
        else { i_metric_value_free (val); }
      }

      /* Subnet Mask */
      listNode = v_plist_node_from_dict (req->plist, node, "ipv4SubnetMasks");
      if (listNode)
      {
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, listNode, NULL);
        if (val->str)
        {
          i_metric_value_enqueue (self, iface->subnetmask, val);
          iface->subnetmask->refresh_result = REFRESULT_OK;
        }
        else { i_metric_value_free (val); }
      }
      
      /* Router */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "router");
      if (val->str) { i_metric_value_enqueue (self, iface->router, val); iface->router->refresh_result = REFRESULT_OK; }
      else { i_metric_value_free (val); }
    }
  }

  return 0;
}

