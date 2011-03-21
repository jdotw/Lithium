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
#include "induction/xsanregistry.h"
#include "device/snmp.h"

#include "osx_server.h"
#include "services.h"
#include "xsan.h"
#include "xsanvol.h"
#include "xsansp.h"
#include "xsannode.h"
#include "xsanvisdisk.h"
#include "plist.h"
#include "data.h"

/*
 * Mac OS X "xsan getState"
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_xsan_localproperties_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysinfo data */
  char *url;
  char *command = NULL;
  if (v_xsan_version() == 1)
  { command = "/commands/servermgr_nomadfss?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetLocalProperties%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command"; }
  else if (v_xsan_version() == 2)
  { command = "/commands/servermgr_xsan?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetComputerProperties%3C%2Fstring%3E%0D%0A%09%3Ckey%3EincludeLUNs%3C%2Fkey%3E%0D%0A%09%3Ctrue%2F%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command"; }
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      if (v_powerstate()) 
      {
        asprintf (&url, "https://%s:311/%s", v_plist_ip(), command);
        static_req = v_plist_get (self, url, v_data_xsan_localproperties_plistcb, met);
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

int v_data_xsan_localproperties_plistcb (i_resource *self, v_plist_req *req, void *passdata)
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
      /* Check for eror (indicating Xsan 2.x) */
      char *error_str = v_plist_data_from_dict (req->plist, node, "error");
      if (error_str && strcmp(error_str, "UNEXPECTED_COMMAND") == 0)
      {
        /* Xsan Version 2.x detected */
        if (v_xsan_version() != 2)
        { v_xsan_version_set (2); }
      }
      if (error_str) free (error_str);
      error_str = NULL;

      /* Get config node */
      xmlNodePtr propNode = v_plist_node_from_dict (req->plist, node, "properties");
      if (propNode)
      {
        /* Check role */
        if (v_xsan_role() == 0)
        {
          char *role_str = v_plist_data_from_dict (req->plist, propNode, "role");
          if (role_str && strcmp(role_str, "CONTROLLER") == 0)
          {
            /* Controller */
            v_xsan_role_set (2);
            i_xsanregistry_register (self, XSAN_TYPE_CONTROLLER, NULL);
            v_xsan_enable_controller (self);
          }
          else if (role_str && strcmp(role_str, "CLIENT") == 0)
          {
            /* Client */
            v_xsan_role_set (1);
            i_xsanregistry_register (self, XSAN_TYPE_CLIENT, NULL);
            v_xsan_enable_client (self);
          }
          if (role_str) free (role_str);
          role_str = NULL;
        }

        /* Visible Disks (Xsan 1) */
        xmlNodePtr disksArray = v_plist_node_from_dict (req->plist, propNode, "visibleDisks");
        if (disksArray)
        { v_data_xsan_process_visibledisks (self, req, disksArray); }
      }
      
      /* Visible Disks (Xsan 2) */
      xmlNodePtr disksArray = v_plist_node_from_dict (req->plist, node, "visibleLUNs");
      if (disksArray)
      { v_data_xsan_process_visibledisks (self, req, disksArray); }
    }
  }

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}

int v_data_xsan_process_visibledisks (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  xmlNodePtr dictNode;

  char *block_device = NULL;

  for (dictNode = arrayNode->children; dictNode; dictNode = dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "key") == 0)
    {
      char *str = (char *) xmlNodeListGetString (req->plist, dictNode->xmlChildrenNode, 1);
      if (block_device) 
      {
        free (block_device);
        block_device = NULL;
      }
      if (str) 
      {
        block_device = strdup (str);
        xmlFree (str);
      }
      else 
      {
        block_device = NULL;
      }
    }

    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Get Volume */
      char *name_str = NULL;
      if (v_xsan_version() == 1)
      { name_str = v_plist_data_from_dict (req->plist, dictNode, "name"); }
      else if (v_xsan_version() == 2)
      { name_str = v_plist_data_from_dict (req->plist, dictNode, "Label"); }
      if (!name_str) continue;
      v_xsanvisdisk_item *visdisk = v_xsanvisdisk_get (name_str);
      if (!visdisk)
      {
        visdisk = v_xsanvisdisk_create (self, name_str);
        if (!visdisk)
        { 
          i_printf (1, "v_data_xsan_process_visibledisks failed to create visdisk %s", name_str); 
          free (name_str);
          continue; 
        }
      }
      v_xsannode_item *lunitem = v_xsannode_get_global (name_str);
      if (name_str) free (name_str);
      name_str = NULL;

      i_metric_value *val;

      /* WWN */
      char *wwn = NULL;
      if (v_xsan_version() == 1)
      { wwn = v_plist_data_from_dict (req->plist, dictNode, "serialNumber"); }
      else
      { wwn = v_plist_data_from_dict (req->plist, dictNode, "SerialNumber"); }
      if (wwn && strlen (wwn) >= 16)
      {
        /* Process WWN string */
        char *wwn_buf;
        asprintf (&wwn_buf, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c:%c%c:%c%c", 
         wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7], 
         wwn[8], wwn[9], wwn[10], wwn[11], wwn[12], wwn[13], wwn[14], wwn[15]);
        val = i_metric_value_create ();
        val->str = wwn_buf;
        i_metric_value_enqueue (self, visdisk->wwn, val);
        visdisk->wwn->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(visdisk->wwn));

        /* Add to node if known */
        if (v_xsan_version() == 2 && lunitem)
        {
          val = i_metric_value_create ();
          val->str = strdup(wwn_buf);
          i_metric_value_enqueue (self, lunitem->wwn, val);
          lunitem->wwn->refresh_result = REFRESULT_OK;
          i_entity_refresh_terminate (ENTITY(lunitem->wwn));
        }
      }
      if (wwn) free (wwn);
      wwn = NULL;

      /* Xsan 2 LUN Item Values */
      if (v_xsan_version() == 2 && lunitem)
      {
        /* Block Device */
        val = i_metric_value_create ();
        val->str = strdup (block_device);
        i_metric_value_enqueue (self, lunitem->blockdev, val);
        lunitem->blockdev->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(lunitem->blockdev));

        /* Host Number */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, dictNode, "Host");
        i_metric_value_enqueue (self, lunitem->hostnumber, val);
        lunitem->hostnumber->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(lunitem->hostnumber));

        /* Inquiry String */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, dictNode, "InquiryString");
        i_metric_value_enqueue (self, lunitem->hostinquiry, val);
        lunitem->hostinquiry->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(lunitem->hostinquiry));

        /* LUN */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, dictNode, "LUN");
        i_metric_value_enqueue (self, lunitem->lun, val);
        lunitem->lun->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(lunitem->lun));
      }

      if (block_device) free (block_device);
      block_device = NULL;
    }
  }

  return 0;
}
