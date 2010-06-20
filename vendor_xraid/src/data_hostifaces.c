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
#include <induction/lunregistry.h>
#include <lithium/snmp.h>

#include "plist.h"
#include "arrays.h"
#include "hostifaces.h"
#include "sysinfo.h"
#include "setup.h"
#include "data.h"

/*
 * Xraid Host Interfaces Data Refresh
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_hostifaces_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_hostifaces data */
  char *action;
  char *plistout;
  v_data_item *item = v_data_static_item ();
  char *acp_password = v_plist_acp_crypt (self->hierarchy->dev->password_str);
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      asprintf (&action, "POST /cgi-bin/rsp-action HTTP/1.1\r\n");
      if (met == item->hostifaces_top)
      {
        asprintf (&plistout, "Content-Length: 240\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: top\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>get-page</key>\n\t<dict>\n\t\t<key>page-id</key>\n\t\t<integer>0</integer>\n\t</dict>\n</dict>\n</plist>\n", acp_password);
      }
      else
      {
        asprintf (&plistout, "Content-Length: 240\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: bottom\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>get-page</key>\n\t<dict>\n\t\t<key>page-id</key>\n\t\t<integer>0</integer>\n\t</dict>\n</dict>\n</plist>\n", acp_password);
      }
      static_req = v_plist_get (self, action, plistout, v_data_hostifaces_plistcb, met);
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

int v_data_hostifaces_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  v_data_item *dataitem = v_data_static_item ();
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
  xmlNodePtr dictNode;
  for (dictNode = req->root_node->children->next->children; dictNode; dictNode = dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      v_setup_item *setup;
      v_sysinfo_item *sysinfo;
      v_hostifaces_item *iface;
      i_metric_value *val;
      xmlDocPtr plist = req->plist;
      
      /* Host Interfaces */
      xmlNodePtr ifacesNode = v_plist_node_from_dict (req->plist, dictNode, "host-interfaces");
      if (ifacesNode)
      { v_data_hostifaces_process_hostifaces (self, datamet, req->plist, ifacesNode); }

      /* Get items */
      if (datamet == dataitem->hostifaces_top)
      {
        setup = v_setup_static_top_item ();
        sysinfo = v_sysinfo_static_item ();
        iface = v_hostifaces_get_item (0, 1);
      }
      else
      {
        setup = v_setup_static_bottom_item ();
        sysinfo = v_sysinfo_static_item ();
        iface = v_hostifaces_get_item (1, 1);
      }
      if (!iface)
      { i_printf (2, "v_data_hostifaces_plistcb failed to locate interface"); continue; }
      
      /* Stripe size */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "stripe-size");
      i_metric_value_enqueue (self, setup->stripesize, val);
      setup->stripesize->refresh_result = REFRESULT_OK;
      
      /* Write Buffer */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (plist, dictNode, "write-buffer-enabled");
      i_metric_value_enqueue (self, setup->writebuffer_enabled, val);
      setup->writebuffer_enabled->refresh_result = REFRESULT_OK;
      
      /* Memory size */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "memory-size");
      i_metric_value_enqueue (self, setup->memorysize, val);
      setup->memorysize->refresh_result = REFRESULT_OK;
      
      /* Firmware */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "firmware-version");
      if (datamet == dataitem->hostifaces_top)
      {
        i_metric_value_enqueue (self, sysinfo->firmware_top, val);
        sysinfo->firmware_top->refresh_result = REFRESULT_OK;
      }
      else
      {
        i_metric_value_enqueue (self, sysinfo->firmware_bottom, val);
        sysinfo->firmware_bottom->refresh_result = REFRESULT_OK;
      }

      /* Serial */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "serial-number");
      i_metric_value_enqueue (self, sysinfo->serial, val);
      sysinfo->serial->refresh_result = REFRESULT_OK;

      /* Fibre Channel MAC */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "fibre-channel-mac-address");
      i_metric_value_enqueue (self, iface->wwn, val);
      iface->wwn->refresh_result = REFRESULT_OK;
      
      /* Access Mode */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "access-mode");
      i_metric_value_enqueue (self, setup->accessmode, val);
      setup->accessmode->refresh_result = REFRESULT_OK;
      
      /* Prefetch size */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "prefetch-size");
      i_metric_value_enqueue (self, setup->prefetchsize, val);
      setup->prefetchsize->refresh_result = REFRESULT_OK;
      
      /* Auto-rebuild */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (plist, dictNode, "auto-rebuild-enabled");
      i_metric_value_enqueue (self, setup->autorebuild_enabled, val);
      setup->autorebuild_enabled->refresh_result = REFRESULT_OK;
      
      /* Rebuild rate */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "rebuild-rate");
      i_metric_value_enqueue (self, setup->rebuildrate, val);
      setup->rebuildrate->refresh_result = REFRESULT_OK;
      
      /* Check Data */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (plist, dictNode, "check-data-generation-enabled");
      i_metric_value_enqueue (self, setup->checkdata_enabled, val);
      setup->checkdata_enabled->refresh_result = REFRESULT_OK;
      
      /* LUN Mask */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (plist, dictNode, "lun-mask-enabled");
      i_metric_value_enqueue (self, setup->lunmask_enabled, val);
      setup->lunmask_enabled->refresh_result = REFRESULT_OK;
      
      /* JBOD Mask */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (plist, dictNode, "jbod-mask-enabled");
      i_metric_value_enqueue (self, setup->jbodmask_enabled, val);
      setup->jbodmask_enabled->refresh_result = REFRESULT_OK;

      /* Link Failure Count */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "link-failure-count");
      i_metric_value_enqueue (self, iface->linkfailure_count, val);
      iface->linkfailure_count->refresh_result = REFRESULT_OK;
      
      /* Loss of Sync Count */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "loss-of-sync-count");
      i_metric_value_enqueue (self, iface->syncloss_count, val);
      iface->syncloss_count->refresh_result = REFRESULT_OK;
      
      /* Loss of signal count */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "loss-of-signal-count");
      i_metric_value_enqueue (self, iface->signalloss_count, val);
      iface->signalloss_count->refresh_result = REFRESULT_OK;
      
      /* Protocol Error Count */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "protocol-error-count");
      i_metric_value_enqueue (self, iface->protocolerror_count, val);
      iface->protocolerror_count->refresh_result = REFRESULT_OK;
      
      /* Invalid Word  Count */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "invalid-transmission-word-count");
      i_metric_value_enqueue (self, iface->invalidword_count, val);
      iface->invalidword_count->refresh_result = REFRESULT_OK;
      
      /* CRC Error Count */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "invalid-crc-count");
      i_metric_value_enqueue (self, iface->invalidcrc_count, val);
      iface->invalidcrc_count->refresh_result = REFRESULT_OK;
    }
  }

  static_req = NULL;

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}

int v_data_hostifaces_process_hostifaces (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr arrayNode)
{
  v_data_item *dataitem = v_data_static_item ();

  /* Parse dict */
  xmlNodePtr dictNode;
  for (dictNode = arrayNode->children; dictNode; dictNode=dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Process each array */
      i_metric_value *val;

      /* Locate array item */
      v_hostifaces_item *item;
      int id = v_plist_int_from_dict (plist, dictNode, "id");
      if (id > 1) continue;
      int controller;
      if (datamet == dataitem->hostifaces_top)
      { controller = 0; }
      else
      { controller = 1; }
      item = v_hostifaces_get_item (controller, id); 
      if (!item)
      {
        /* Create the array item */
        item = v_hostifaces_create (self, controller);
      }
      
      /* Hard loop id */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "hard-loop-id");
      i_metric_value_enqueue (self, item->hardloopid, val);
      item->hardloopid->refresh_result = REFRESULT_OK;
      
      /* Topology */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "topology");
      i_metric_value_enqueue (self, item->topology, val);
      item->topology->refresh_result = REFRESULT_OK;
      
      /* Actual Topology */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "actual-topology");
      i_metric_value_enqueue (self, item->actual_topology, val);
      item->actual_topology->refresh_result = REFRESULT_OK;
      
      /* Speed */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "speed");
      i_metric_value_enqueue (self, item->speed, val);
      item->speed->refresh_result = REFRESULT_OK;
      
      /* Hard loop ID enabled */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (plist, dictNode, "hard-loop-id-enabled");
      i_metric_value_enqueue (self, item->hardloopid_enabled, val);
      item->hardloopid_enabled->refresh_result = REFRESULT_OK;
      
      /* Type */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "type");
      i_metric_value_enqueue (self, item->type, val);
      item->type->refresh_result = REFRESULT_OK;
      
      /* Max-speed */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "max-speed");
      i_metric_value_enqueue (self, item->max_speed, val);
      item->max_speed->refresh_result = REFRESULT_OK;
      
      /* Actual Speed */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "actual-speed");
      i_metric_value_enqueue (self, item->actual_speed, val);
      item->actual_speed->refresh_result = REFRESULT_OK;
      
      /* Members */
      xmlNodePtr lunNode = v_plist_node_from_dict (plist, dictNode, "lun-map");
      if (lunNode) v_data_hostifaces_process_luns (self, item, datamet, plist, lunNode);
    }
  }
  
  return 0;
}

int v_data_hostifaces_process_luns (i_resource *self, v_hostifaces_item *item, i_metric *datamet, xmlDocPtr plist, xmlNodePtr arrayNode)
{
  v_data_item *dataitem = v_data_static_item ();

  /* Parse dict */
  xmlNodePtr dictNode;
  for (dictNode = arrayNode->children; dictNode; dictNode=dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Process each array */
      i_metric_value *val;
      i_metric *array_met = NULL;
      i_metric *slice_met = NULL;

      /* Locate array item */
      int id = v_plist_int_from_dict (plist, dictNode, "id");

      switch (id)
      {
        case 0:
          array_met = item->lun0_array;
          slice_met = item->lun0_slice;
          break;
        case 1:
          array_met = item->lun1_array;
          slice_met = item->lun1_slice;
          break;
        case 2:
          array_met = item->lun2_array;
          slice_met = item->lun2_slice;
          break;
        case 3:
          array_met = item->lun3_array;
          slice_met = item->lun3_slice;
          break;
        case 4:
          array_met = item->lun4_array;
          slice_met = item->lun4_slice;
          break;
        case 5:
          array_met = item->lun5_array;
          slice_met = item->lun5_slice;
          break;
        case 6:
          array_met = item->lun6_array;
          slice_met = item->lun6_slice;
          break;
        case 7:
          array_met = item->lun7_array;
          slice_met = item->lun7_slice;
          break;
      }

      if (array_met)
      { 
        /* Array */
        int arrayid;
        arrayid = v_plist_int_from_dict (plist, dictNode, "array-id");
        
        /* Slice */
        int sliceid;
        sliceid = v_plist_int_from_dict (plist, dictNode, "slice-id");
        
        /* Check array value and add controller offset */
        if (arrayid == 15) continue;
        v_arrays_item *arrayitem;
        int controller;
        if (datamet == dataitem->hostifaces_top)
        { controller = 0; }
        else
        { controller = 1; }
        arrayitem = v_arrays_get_item (controller, arrayid);
        if (!arrayitem)
        {
          i_printf (1, "v_data_hostifaces_process_luns failed to retrieve array %i for controller %i", arrayid, controller); 
          continue;
        }
        
        /* Set arrayid */
        val = i_metric_value_create ();
        val->integer = arrayitem->index;
        i_metric_value_enqueue (self, array_met, val);
        array_met->refresh_result = REFRESULT_OK;
        
        /* Set sliceid */
        val = i_metric_value_create ();
        val->integer = sliceid;
        i_metric_value_enqueue (self, slice_met, val);
        slice_met->refresh_result = REFRESULT_OK;

        /* Register array object with LUN registry */
        if (arrayitem && arrayitem->lun_registered == 0)
        {
          char *wwn_str = i_metric_valstr (item->wwn, NULL);
          if (wwn_str)
          { 
            i_lunregistry_register (self, 1, arrayitem->obj, wwn_str, id); 
            arrayitem->lun_registered = 1;
            free (wwn_str);
          }
        }
      }
    }
  }

  return 0;
}

