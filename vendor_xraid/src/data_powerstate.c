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

#include "status.h"
#include "plist.h"
#include "data.h"

/*
 * Xraid Power State Data Refresh
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_powerstate_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_powerstate data */
  char *action;
  char *plistout;
  char *acp_password = v_plist_acp_crypt (self->hierarchy->dev->password_str);
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      asprintf (&action, "POST /cgi-bin/acp-action HTTP/1.1\r\n");
      asprintf (&plistout, "Content-Length: 195\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>power-state-get</key>\n\t<dict/>\n</dict>\n</plist>\n", acp_password);
      static_req = v_plist_get (self, action, plistout, v_data_powerstate_plistcb, met);
      free (plistout);
      free (action);
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

int v_data_powerstate_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_metric *datamet = passdata;

  /* Set req ptr to NULL */
  static_req = NULL;
    
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
    if (strcmp((char *) node->name, "dict") == 0)
    {
      xmlNodePtr dictNode;
      char *key = NULL;
      char *data = NULL;
      v_status_item *top_item = v_status_static_top_item ();
      v_status_item *bottom_item = v_status_static_bottom_item ();
        
      for (dictNode = node->children; dictNode; dictNode = dictNode->next)
      {
        if (strcmp((char *) dictNode->name, "key") == 0)
        { 
          key = (char *) xmlNodeListGetString (req->plist, dictNode->xmlChildrenNode, 1); 
          data = v_plist_data_from_dict (req->plist, dictNode->next->next, NULL);
            
          if (!key || !data) continue;

          i_metric_value *val;
          if (strcmp(key, "power-state-get") == 0)
          {
            val = i_metric_value_create ();
            val->integer = atoi (data);
            i_metric_value_enqueue (self, top_item->powerstate, val);
            top_item->powerstate->refresh_result = REFRESULT_OK;
            val = i_metric_value_create ();
            val->integer = atoi (data);
            i_metric_value_enqueue (self, bottom_item->powerstate, val);
            bottom_item->powerstate->refresh_result = REFRESULT_OK;
          }

          if (key) xmlFree (key);
          if (data) free (data);
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


