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
#include <induction/str.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>
#include <lithium/snmp.h>

#include "plist.h"
#include "drives.h"
#include "data.h"

/*
 * Xraid Drives Data Refresh
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_drives_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_drives data */
  char *action;
  char *plistout;
  v_data_item *item = v_data_static_item ();
  char *acp_password = v_plist_acp_crypt (self->hierarchy->dev->password_str);
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      asprintf (&action, "POST /cgi-bin/rsp-action HTTP/1.1\r\n");
      if (met == item->drives_top)
      {
        asprintf (&plistout, "Content-Length: 241\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: top\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>get-page</key>\n\t<dict>\n\t\t<key>page-id</key>\n\t\t<integer>16</integer>\n\t</dict>\n</dict>\n</plist>\n", acp_password);
      }
      else
      {
        asprintf (&plistout, "Content-Length: 241\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: bottom\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>get-page</key>\n\t<dict>\n\t\t<key>page-id</key>\n\t\t<integer>16</integer>\n\t</dict>\n</dict>\n</plist>\n", acp_password);
      }
      static_req = v_plist_get (self, action, plistout, v_data_drives_plistcb, met);
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

int v_data_drives_plistcb (i_resource *self, v_plist_req *req, void *passdata)
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
  xmlNodePtr node;
  for (node = req->root_node->children->next->children; node; node = node->next)
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
          if (strcmp(key, "slots") == 0)
          {
            /* Outputs found, walk the outputs dict */
            xmlNodePtr slotNode;
            for (slotNode = dictNode->next->next->children; slotNode; slotNode = slotNode->next)
            {
              if (strcmp((char *) slotNode->name, "dict") == 0)
              { v_data_drives_process_slot (self, datamet, req->plist, slotNode); }
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

int v_data_drives_process_slot (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr slotNode)
{
  /* Process each drives SMART info */
  i_metric_value *val;
  v_data_item *dataitem = v_data_static_item ();
    
  /* Locate drive item */
  int drive_id = v_plist_int_from_dict (plist, slotNode, "id");
  if (drive_id < 1) 
  { i_printf (1, "v_data_drives_process_slot failed to get drive id from plist"); return -1; }
  if (datamet == dataitem->drives_bottom) 
  { drive_id += 7; }
  v_drives_item *drive = v_drives_x (drive_id);
  if (!drive)
  { i_printf (1, "v_data_drives_process_slot failed to get drive item for %i", drive_id); return -1; }

  /* Vendor */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, slotNode, "disk-vendor-id");
  i_metric_value_enqueue (self, drive->vendor, val);
  drive->vendor->refresh_result = REFRESULT_OK;
    
  /* Firmware */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, slotNode, "disk-firmware-version");
  i_metric_value_enqueue (self, drive->firmware, val);
  drive->firmware->refresh_result = REFRESULT_OK;
    
  /* Sector Capacity */
  val = i_metric_value_create ();
  val->gauge = (unsigned long) v_plist_int_from_dict (plist, slotNode, "disk-sector-capacity");
  i_metric_value_enqueue (self, drive->sectorcapacity, val);
  drive->sectorcapacity->refresh_result = REFRESULT_OK;

  /* Capacity */
  val = i_metric_value_create ();
  val->gauge = (unsigned long) v_plist_int_from_dict (plist, slotNode, "disk-sector-capacity");
  i_metric_value_enqueue (self, drive->capacity, val);
  drive->capacity->refresh_result = REFRESULT_OK;

  /* Bad Block Count */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, slotNode, "disk-bad-block-count");
  i_metric_value_enqueue (self, drive->badblockcount, val);
  drive->badblockcount->refresh_result = REFRESULT_OK;

  /* Disk Re-Map Count */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (plist, slotNode, "disk-remap-count");
  i_metric_value_enqueue (self, drive->remapcount, val);
  drive->remapcount->refresh_result = REFRESULT_OK;
    
  return 0;
}

