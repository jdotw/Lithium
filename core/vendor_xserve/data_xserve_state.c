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

#include "osx_server.h"
#include "network.h"
#include "plist.h"
#include "data.h"

/*
 * Mac OS X "xserve getState"
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_xserve_state_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysxserve data */
  char *url;
  char *command = "/commands/servermgr_xserve?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3Estatus%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command";
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      if (v_powerstate()) 
      {
        asprintf (&url, "https://%s:311/%s", v_plist_ip(), command);
        static_req = v_plist_get (self, url, v_data_xserve_state_plistcb, met);
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

int v_data_xserve_state_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_metric *datamet = passdata;

  /* Clear req */ 
  static_req = NULL;
  
  /* Check result */
  if (!req)
  { return -1; }

  /* Get status node */
  xmlNodePtr statusNode = v_plist_node_from_dict (req->plist, req->root_node->children->next, "Status");
  if (!statusNode)
  { i_printf (1, "v_data_xserve_state_plistcb status node not found"); return 0; }

  /* Connectivity Node */
  xmlNodePtr connNode = v_plist_node_from_dict (req->plist, statusNode, "Connectivity"); 
  if (connNode)
  { v_data_xserve_process_connectivity (self, req, connNode); }

  /* Controls */
  xmlNodePtr controlNode = v_plist_node_from_dict (req->plist, statusNode, "Controls"); 
  if (controlNode)
  { v_data_xserve_process_controls (self, req, controlNode); }
  else
  { i_printf (1, "v_data_xserve_state_plistcb Controls node not found!"); }

  /* Drives */
  xmlNodePtr drivesNode = v_plist_node_from_dict (req->plist, statusNode, "Drives");
  if (drivesNode)
  { v_data_xserve_process_drives (self, req, drivesNode); }

  /* Identity */
  xmlNodePtr identityNode = v_plist_node_from_dict (req->plist, statusNode, "Identity");
  if (identityNode)
  { v_data_xserve_process_identity (self, req, identityNode); }
  
  /* Memory */
  xmlNodePtr memoryNode = v_plist_node_from_dict (req->plist, statusNode, "Memory");
  if (memoryNode)
  { v_data_xserve_process_memory (self, req, memoryNode); }

  /* Sensors */
  xmlNodePtr sensorsNode = v_plist_node_from_dict (req->plist, statusNode, "Sensors");
  if (sensorsNode)
  { v_data_xserve_process_sensors (self, req, sensorsNode); }

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}


