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

#include "osx_server.h"
#include "cpu.h"
#include "volumes.h"
#include "network.h"
#include "plist.h"
#include "data.h"

/*
 * Mac OS X "info getHardwareInfo"
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_info_hardware_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysinfo data */
  char *url;
  char *command = "/commands/servermgr_info?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetHardwareInfo%3C%2Fstring%3E%0D%0A%09%3Ckey%3Evariant%3C%2Fkey%3E%0D%0A%09%3Cstring%3EwithQuotaUsage%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command";
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      if (v_powerstate()) 
      {
        asprintf (&url, "https://%s:311/%s", v_plist_ip(), command);
        static_req = v_plist_get (self, url, v_data_info_hardware_plistcb, met);
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

int v_data_info_hardware_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_metric *datamet = passdata;
    
  /* Check result */
  if (!req)
  { return -1; }

  /* Parse XML plist */
  xmlNodePtr node;
  for (node = req->root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      if (!v_xserve_extras ())
      {
        /* Master CPU */
        v_cpu_item *master_cpu = v_cpu_master (); 
        char *str = v_plist_data_from_dict (req->plist, node, "cpuUsageBy100");
        if (str)
        {
          i_metric_value *val;
          val = i_metric_value_create ();
          val->flt = atof (str) / 100.00;
          i_metric_value_enqueue (self, master_cpu->usage, val);
          master_cpu->usage->refresh_result = REFRESULT_OK;
          free (str);
          str = NULL;
        }
        else
        {
          master_cpu->usage->refresh_result = REFRESULT_TOTAL_FAIL;
        }
      }
      
      /* CPU Usage Array */
      xmlNodePtr cpuArrayNode = v_plist_node_from_dict (req->plist, node, "cpuUsagesBy100Array");
      if (cpuArrayNode) v_data_info_hardware_process_cpuarray (self, req, cpuArrayNode);
      
      /* Volumes Array */
      xmlNodePtr volumeArrayNode = v_plist_node_from_dict (req->plist, node, "volumeInfosArray");
      if (volumeArrayNode) v_data_info_hardware_process_volumearray (self, req, volumeArrayNode);

      /* Interfaces Arrays */
      if (v_xserve_extras() == 0)
      {
        xmlNodePtr ifaceArrayNode = v_plist_node_from_dict (req->plist, node, "networkThroughputsArray");
        if (ifaceArrayNode) v_data_info_hardware_process_ifacearray (self, req, ifaceArrayNode);
      }
    }
  }

  static_req = NULL;

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}

int v_data_info_hardware_process_cpuarray (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  int i = 0;
  xmlNodePtr dictNode;

  for (dictNode = arrayNode->children; dictNode; dictNode = dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Get CPU */
      v_cpu_item *cpu = NULL;
      if (v_xserve_intel_extras())
      { 
        cpu = v_cpu_x (i / 2);
      }
      else
      { cpu = v_cpu_x (i); }
      if (!cpu)
      {
        cpu = v_cpu_create (self); 
      }
      if (!cpu)
      { i_printf (1, "v_data_info_hardware_process_cpuarray failed to create cpu %i", i); continue; }

      /* Usage */
      i_metric *met;
      if (v_xserve_intel_extras())
      {
        if (i % 2 == 0)
        { met = cpu->usage_1; }
        else
        { met = cpu->usage_2; }
      }
      else
      { met = cpu->usage; }
      char *str = v_plist_data_from_dict (req->plist, dictNode, NULL);
      if (str)
      { 
        i_metric_value *val;
        val = i_metric_value_create ();
        val->flt = atof (str) / 100.00; 
        if (met)
        {
          i_metric_value_enqueue (self, met, val);
          met->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(met));
        }
        free (str);
        str = NULL;
      }
      else if (met)
      {
        met->refresh_result = REFRESULT_TOTAL_FAIL;
        i_entity_refresh_terminate (ENTITY(met));
      }

      /* Increment cpu index */
      i++;
    }
  }

  return 0;
}

int v_data_info_hardware_process_volumearray (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  xmlNodePtr dictNode;

  for (dictNode = arrayNode->children; dictNode; dictNode = dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Get Volume */
      char *name_str = v_plist_data_from_dict (req->plist, dictNode, "name");
      if (!name_str) continue;
      v_volumes_item *volume = v_volumes_get (name_str);
      if (!volume)
      {
        volume = v_volumes_create (self, name_str);
        if (!volume)
        { i_printf (1, "v_data_info_hardware_process_volumesarray failed to create volumes %s", name_str); free (name_str); continue; }
      }
      free (name_str);
      name_str = NULL;

      /* Free Bytes */
      char *str = v_plist_data_from_dict (req->plist, dictNode, "freeBytes");
      if (str)
      {
        i_metric_value *val;
        double flt;
        flt = atof (str) / (1024 * 1024);
        val = i_metric_value_create ();
        val->gauge = (unsigned int) flt;
        i_metric_value_enqueue (self, volume->bytes_free, val);
        volume->bytes_free->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->bytes_free));
        free (str);
        str = NULL;
      }
      else
      {
        volume->bytes_free->refresh_result = REFRESULT_TOTAL_FAIL;
      }

      /* Total Bytes */
      str = v_plist_data_from_dict (req->plist, dictNode, "totalBytes");
      if (str)
      {
        i_metric_value *val;
        double flt;
        flt = atof (str) / (1024 * 1024);
        val = i_metric_value_create ();
        val->gauge = (unsigned int) flt;
        i_metric_value_enqueue (self, volume->bytes_total, val);
        volume->bytes_total->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->bytes_total));
        free (str);
        str = NULL;
      }
      else
      {
        volume->bytes_total->refresh_result = REFRESULT_TOTAL_FAIL;
      }
    }
  }

  return 0;

}

int v_data_info_hardware_process_ifacearray (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  xmlNodePtr dictNode;

  for (dictNode = arrayNode->children; dictNode; dictNode = dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Process dictionary */
      xmlNodePtr node;
      for (node = dictNode->children; node; node = node->next)
      {
        if (strcmp((char *)node->name, "key") == 0)
        {
          /* Get the key */
          char *key = (char *) xmlNodeListGetString (req->plist, node->xmlChildrenNode, 1);
          if (!key) continue;

          /* See what type of key it is and get the iface name */
          int type;
          char *iface_name;
          if (strstr(key, "networkThroughput-in"))
          {
            /* Inbound throughput */
            iface_name = strdup (key + 21);           
            type = 1;   /* Inbound */
          }
          else if (strstr(key, "networkThroughput-out"))
          {
            /* Outbound Throughput */
            iface_name = strdup (key + 22);
            type = 2;   /* Outbound */
          }
          else
          { 
            xmlFree (key);
            continue; 
          }
          xmlFree (key);

          /* Get the interface */
          v_network_item *iface = v_network_get (iface_name);
          if (!iface)
          {
            i_printf (2, "v_data_info_hardware_process_ifacearray failed to get iface %s", iface_name); 
            free (iface_name);
            iface_name = NULL;
            continue;
          }
          free (iface_name);
          iface_name = NULL;

          /* Set value */
          i_metric *met;
          if (type == 1)
          { met = iface->bps_in; }
          else if (type == 2)
          { met = iface->bps_out; }
          else 
          { continue; }
          i_metric_value *val;
          val = i_metric_value_create ();
          val->gauge = (unsigned int) v_plist_int_from_dict (req->plist, dictNode, NULL);
          i_metric_value_enqueue (self, met, val);
          met->refresh_result = REFRESULT_OK;
        }
      }
    }
  }

  return 0;
}
