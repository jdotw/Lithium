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
#include "services.h"
#include "xsaninfo.h"
#include "plist.h"
#include "data.h"

/*
 * Mac OS X -- Xsan Performance History
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_xsan2_history_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysinfo data */
  char *url;
  char *command = "/commands/servermgr_xsan?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetHistory%3C%2Fstring%3E%0D%0A%09%3Ckey%3Eidentifier%3C%2Fkey%3E%0D%0A%09%3Cstring%3Esystem%3C%2Fstring%3E%0D%0A%09%3Ckey%3EtimeScale%3C%2Fkey%3E%0D%0A%09%3Cinteger%3E3600%3C%2Finteger%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command";
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      if (v_powerstate()) 
      {
        asprintf (&url, "https://%s:311/%s", v_plist_ip(), command);
        static_req = v_plist_get (self, url, v_data_xsan2_history_plistcb, met);
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

int v_data_xsan2_history_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_metric *datamet = passdata;
  i_object *info_obj = (i_object *) v_xsaninfo_obj ();
  v_xsaninfo_item *info = info_obj->itemptr;

  /* Clear req */
  static_req = NULL;
    
  /* Check result */
  if (!req || !info)
  { return -1; }

  /* Parse XML plist */
  xmlNodePtr node;
  for (node = req->root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      xmlNodePtr samplesNode = v_plist_node_from_dict (req->plist, node, "samplesArray");
      if (samplesNode)
      {
        v_data_xsan_fibre_process_samples (self, req, info, samplesNode);
        v_data_xsan_network_process_samples (self, req, info, samplesNode);
        v_data_xsan_cpu_process_samples (self, req, info, samplesNode);
      }
    }
  }

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));

  return 0;
}

