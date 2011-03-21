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

#include "sysinfo.h"
#include "plist.h"
#include "data.h"

/*
 * Xraid Sysinfo Data Refresh
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_sysinfo_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysinfo data */
  char *action;
  char *plistout;
  char *acp_password = v_plist_acp_crypt (self->hierarchy->dev->password_str);
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      asprintf (&action, "POST /cgi-bin/acp-get HTTP/1.1\r\n");
      asprintf (&plistout, "Content-Length: 781\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<array>\n\t<string>syNm</string>\n\t<string>syDN</string>\n\t<string>syPN</string>\n\t<string>syCt</string>\n\t<string>syDs</string>\n\t<string>syLo</string>\n\t<string>syVs</string>\n\t<string>syPR</string>\n\t<string>syGP</string>\n\t<string>syUT</string>\n\t<string>syFl</string>\n\t<string>laMA</string>\n\t<string>laCV</string>\n\t<string>laIP</string>\n\t<string>laSM</string>\n\t<string>laRA</string>\n\t<string>waC1</string>\n\t<string>waC2</string>\n\t<string>waC3</string>\n\t<string>laDC</string>\n\t<string>laIS</string>\n\t<string>smtp</string>\n\t<string>ntpC</string>\n\t<string>snAF</string>\n\t<string>svMd</string>\n\t<string>effF</string>\n\t<string>emNo</string>\n</array>\n</plist>\n", acp_password);
      static_req = v_plist_get (self, action, plistout, v_data_sysinfo_plistcb, met);
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

int v_data_sysinfo_plistcb (i_resource *self, v_plist_req *req, void *passdata)
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
  for (node = req->root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      xmlNodePtr dictNode;
      char *key = NULL;
      char *data = NULL;
      v_sysinfo_item *item = v_sysinfo_static_item ();
        
      for (dictNode = node->children; dictNode; dictNode = dictNode->next)
      {
        if (strcmp((char *)dictNode->name, "key") == 0)
        { 
          key = (char *) xmlNodeListGetString (req->plist, dictNode->xmlChildrenNode, 1); 
          data = v_plist_data_from_dict (req->plist, dictNode->next->next, NULL);
            
          if (!key || !data) continue;

          i_metric_value *val;
          if (strcmp(key, "syNm") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->name, val);
            item->name->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syDN") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->DN, val);
            item->DN->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syPN") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->PN, val);
            item->PN->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syCt") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->contact, val);
            item->contact->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syDs") == 0)
          {
              val = i_metric_value_create ();
              val->str = strdup (data);
              i_metric_value_enqueue (self, item->DS, val);
              item->DS->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syLo") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->location, val);
            item->location->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syVs") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->version, val);
            item->version->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syUT") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->uptime, val);
            item->uptime->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "syFl") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->Fl, val);
            item->Fl->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "laMA") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->mac, val);
            item->mac->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "laIP") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->ip, val);
            item->ip->refresh_result = REFRESULT_OK;
          }
          else if (strcmp(key, "laSM") == 0)
          {
            val = i_metric_value_create ();
            val->str = strdup (data);
            i_metric_value_enqueue (self, item->subnet, val);
            item->subnet->refresh_result = REFRESULT_OK;
          }
        
          if (key) xmlFree (key);
          if (data) free (data);
        }
      }
    }
  }

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}


