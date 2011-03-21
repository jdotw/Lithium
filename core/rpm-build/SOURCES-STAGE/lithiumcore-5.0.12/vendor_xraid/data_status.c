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

#include "plist.h"
#include "drives.h"
#include "status.h"
#include "hostifaces.h"
#include "data.h"

/*
 * Xraid System Status Data Refresh
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_status_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_status data */
  char *action;
  char *plistout;
  v_data_item *item = v_data_static_item ();
  char *acp_password = v_plist_acp_crypt (self->hierarchy->dev->password_str);
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      asprintf (&action, "POST /cgi-bin/perform HTTP/1.1\r\n");
      if (met == item->status_top)
      {
        asprintf (&plistout, "Content-Length: 317\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: top\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>requests</key>\n\t<array>\n\t\t<dict>\n\t\t\t<key>inputs</key>\n\t\t\t<dict/>\n\n\t\t\t<key>method</key>\n\t\t\t<string>/system/get-system-status</string>\n\t\t</dict>\n\t</array>\n</dict>\n</plist>\n", acp_password);
      }
      else
      {
        asprintf (&plistout, "Content-Length: 317\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: bottom\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>requests</key>\n\t<array>\n\t\t<dict>\n\t\t\t<key>inputs</key>\n\t\t\t<dict/>\n\n\t\t\t<key>method</key>\n\t\t\t<string>/system/get-system-status</string>\n\t\t</dict>\n\t</array>\n</dict>\n</plist>\n", acp_password);
      }
      static_req = v_plist_get (self, action, plistout, v_data_status_plistcb, met);
      free (action);
      free (plistout);
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

  free (acp_password);

  return 0;
}

int v_data_status_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_metric *datamet = passdata;

  /* Set req ptr to NULL */
  static_req = NULL;  
  
  /* Check result */
  if (!req->plist || !req->root_node)
  {
    /* Failed to get XML */
    /* Set result and terminate */
    datamet->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(datamet));
    return -1;
  }

  /* Parse XML plist */
  xmlNodePtr node = req->root_node->children->next->children->next->children;
  for (node = req->root_node->children->next->children->next->next->next->children; node; node = node->next)
  {
    if (strcmp((char *) node->name, "dict") == 0)
    {
      xmlNodePtr dictNode;
      char *key = NULL;

      /* Process the dictionary */      
      for (dictNode = node->children; dictNode; dictNode = dictNode->next)
      {
        if (strcmp((char *) dictNode->name, "key") == 0)
        { 
          key = (char *) xmlNodeListGetString (req->plist, dictNode->xmlChildrenNode, 1); 
          if (strcmp(key, "outputs") == 0)
          {
            /* Outputs found, walk the outputs dict */
            xmlNodePtr outputNode;
            for (outputNode = dictNode->next->next->children->next->next->next->children; outputNode; outputNode = outputNode->next)
            {
              if (strcmp((char *) outputNode->name, "key") == 0)
              {
                /* Process each 'output' key */
                char *outputKey = (char *) xmlNodeListGetString (req->plist, outputNode->xmlChildrenNode, 1);

                /* ups */
                if (outputKey && strcmp(outputKey, "ups") == 0)
                { v_data_status_process_ups (self, datamet, req->plist, outputNode->next->next); }

                /* smart */
                if (outputKey && strcmp(outputKey, "smart") == 0)
                { v_data_status_process_smart (self, datamet, req->plist, outputNode->next->next); }

                /* raid-controller */
                if (outputKey && strcmp(outputKey, "raid-controller") == 0)
                { v_data_status_process_raidcont (self, datamet, req->plist, outputNode->next->next); }

                /* monitor */
                if (outputKey && strcmp(outputKey, "monitor") == 0)
                { v_data_status_process_monitor (self, datamet, req->plist, outputNode->next->next); }

                /* enclosure */
                if (outputKey && strcmp(outputKey, "enclosure") == 0)
                { v_data_status_process_enclosure (self, datamet, req->plist, outputNode->next->next); }

                /* Clean up */
                if (outputKey) xmlFree (outputKey);
              }
            }
          }
          if (key) xmlFree (key);
        }
      }
    }
  }

  static_req = NULL;

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}

int v_data_status_process_ups (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr dictNode)
{
  i_metric_value *val;
  v_data_item *dataitem = v_data_static_item ();

  /* Get top/bottom status item */
  v_status_item *item;
  if (datamet == dataitem->status_top)
  { item = v_status_static_top_item (); }
  else
  { item = v_status_static_bottom_item (); }
  
  /* Battery low */
  val = i_metric_value_create ();
  val->integer = v_plist_bool_from_dict (plist, dictNode, "battery-is-low");
  i_metric_value_enqueue (self, item->batterylow, val);
  item->batterylow->refresh_result = REFRESULT_OK;
                                                
  /* Battery in use */
  val = i_metric_value_create ();
  val->integer = v_plist_bool_from_dict (plist, dictNode, "battery-in-use");
  i_metric_value_enqueue (self, item->batteryinuse, val);
  item->batteryinuse->refresh_result = REFRESULT_OK;
                                                
  /* Line power down */
  val = i_metric_value_create ();
  val->integer = v_plist_bool_from_dict (plist, dictNode, "line-power-is-down");
  i_metric_value_enqueue (self, item->linepowerdown, val);
  item->linepowerdown->refresh_result = REFRESULT_OK;

  return 0;  
}

int v_data_status_process_smart (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr arrayNode)
{
  v_data_item *dataitem = v_data_static_item ();

  /* Parse dict */
  xmlNodePtr dictNode;
  for (dictNode = arrayNode->children; dictNode; dictNode=dictNode->next)
  {
    if (strcmp((char *) dictNode->name, "dict") == 0)
    {
      /* Process each drives SMART info */
      i_metric_value *val;

      /* Locate drive item */
      int drive_id = v_plist_int_from_dict (plist, dictNode, "drive-id");
      if (datamet == dataitem->status_bottom) 
      { drive_id += 7; }
      v_drives_item *drive = v_drives_x (drive_id);
      if (!drive)
      { i_printf (0, "v_data_status_process_smart failed to get drive item for %i", drive_id); continue; }

      /* State */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "state");
      i_metric_value_enqueue (self, drive->smart_state, val);
      drive->smart_state->refresh_result = REFRESULT_OK;
    
      /* Status (integer) */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "status");
      i_metric_value_enqueue (self, drive->smart_status, val);
      drive->smart_status->refresh_result = REFRESULT_OK;
    }
  }

  return 0;
}

int v_data_status_process_raidcont (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr dictNode)
{
  i_metric_value *val;
  v_data_item *dataitem = v_data_static_item ();
  v_hostifaces_item *ifaceitem;

  /* Get top/bottom status item */
  v_status_item *item;
  if (datamet == dataitem->status_top)
  { 
    item = v_status_static_top_item (); 
    ifaceitem = v_hostifaces_get_item (0, 1); 
  }
  else
  { 
    item = v_status_static_bottom_item (); 
    ifaceitem = v_hostifaces_get_item (1, 1); 
  }

  if (ifaceitem)
  {
    /* Fibe Link */
    val = i_metric_value_create ();
    val->str = v_plist_data_from_dict (plist, dictNode, "fibre-link-state");
    i_metric_value_enqueue (self, ifaceitem->linkstate, val);
    ifaceitem->linkstate->refresh_result = REFRESULT_OK;
  }

  /* State */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "state");
  i_metric_value_enqueue (self, item->raidcont_state, val);
  item->raidcont_state->refresh_result = REFRESULT_OK;

  return 0;
}

int v_data_status_process_monitor (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr dictNode)
{
  i_metric_value *val;
  v_data_item *dataitem = v_data_static_item ();

  /* Get top/bottom status item */
  v_status_item *item;
  if (datamet == dataitem->status_top)
  { item = v_status_static_top_item (); }
  else
  { item = v_status_static_bottom_item (); }

  /* Power Supply */
//  val = i_metric_value_create ();
//  val->str = v_plist_data_from_dict (plist, dictNode, "power-supply");
//  i_metric_value_enqueue (self, item->powersupply, val);
//  item->powersupply->refresh_result = REFRESULT_OK;

  /* RAID Set */
//  val = i_metric_value_create ();
//  val->str = v_plist_data_from_dict (plist, dictNode, "raid-set");
//  i_metric_value_enqueue (self, item->raidset, val);
//  item->raidset->refresh_result = REFRESULT_OK;

  /* Drive Temp */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "drive-temperature");
  i_metric_value_enqueue (self, item->drivetemp, val);
  item->drivetemp->refresh_result = REFRESULT_OK;

  /* Blower Temp */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "blower-temperature");
  i_metric_value_enqueue (self, item->blowertemp, val);
  item->blowertemp->refresh_result = REFRESULT_OK;

  /* RAID Controller Temp */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "raid-controller-temperature");
  i_metric_value_enqueue (self, item->raidtemp, val);
  item->raidtemp->refresh_result = REFRESULT_OK;

  /* Blower Speed */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "blower-speed");
  i_metric_value_enqueue (self, item->blowerspeed, val);
  item->blowerspeed->refresh_result = REFRESULT_OK;

  /* EMU Temp */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "emu-temperature");
  i_metric_value_enqueue (self, item->emutemp, val);
  item->emutemp->refresh_result = REFRESULT_OK;

  /* Smart */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "smart");
  i_metric_value_enqueue (self, item->smart, val);
  item->smart->refresh_result = REFRESULT_OK;

  /* Blower */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "blower");
  i_metric_value_enqueue (self, item->blower, val);
  item->blower->refresh_result = REFRESULT_OK;

  /* Power Supply */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "raid-communication");
  i_metric_value_enqueue (self, item->raidcomms, val);
  item->raidcomms->refresh_result = REFRESULT_OK;

  /* Ambient Temp */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "ambient-temperature");
  i_metric_value_enqueue (self, item->ambientstate, val);
  item->ambientstate->refresh_result = REFRESULT_OK;

  /* Battery */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "battery");
  i_metric_value_enqueue (self, item->battery, val);
  item->battery->refresh_result = REFRESULT_OK;

  return 0;
}

int v_data_status_process_enclosure (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr dictNode)
{
  i_metric_value *val;
  v_data_item *dataitem = v_data_static_item ();
      
  /* Get top/bottom status item */
  v_status_item *item;
  if (datamet == dataitem->status_top)
  { item = v_status_static_top_item (); }
  else
  { item = v_status_static_bottom_item (); }
 
  /* Service ID */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "service-id");
  i_metric_value_enqueue (self, item->serviceid, val);
  item->serviceid->refresh_result = REFRESULT_OK;

  /* Buzzer */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "buzzer");
  i_metric_value_enqueue (self, item->buzzer, val);
  item->buzzer->refresh_result = REFRESULT_OK;
                                            
  /* Xsync State */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, dictNode, "xsync-state");
  i_metric_value_enqueue (self, item->xsyncstate, val);
  item->xsyncstate->refresh_result = REFRESULT_OK;
  
  return 0;
} 

