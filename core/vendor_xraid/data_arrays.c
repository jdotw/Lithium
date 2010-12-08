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
#include "arrays.h"
#include "data.h"

/*
 * Xraid System Status Data Refresh
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_arrays_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_arrays data */
  char *action;
  char *plistout;
  v_data_item *item = v_data_static_item ();
  char *acp_password = v_plist_acp_crypt (self->hierarchy->dev->password_str);
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      asprintf (&action, "POST /cgi-bin/rsp-action HTTP/1.1\r\n");
      if (met == item->arrays_top)
      {
        asprintf (&plistout, "Content-Length: 240\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: top\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>get-page</key>\n\t<dict>\n\t\t<key>page-id</key>\n\t\t<integer>1</integer>\n\t</dict>\n</dict>\n</plist>\n", acp_password);
      }
      else
      {
        asprintf (&plistout, "Content-Length: 240\r\nUser-Agent: Apple-Xserve_RAID_Admin/1.3.0\r\nHost: 10.1.1.66\r\nContent-Type: application/xml\r\nACP-User: guest\r\nApple-Xsync: bottom\r\nACP-Password: %s\r\n\r\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist SYSTEM \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n\t<key>get-page</key>\n\t<dict>\n\t\t<key>page-id</key>\n\t\t<integer>1</integer>\n\t</dict>\n</dict>\n</plist>\n", acp_password);
      }
      static_req = v_plist_get (self, action, plistout, v_data_arrays_plistcb, met);
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

int v_data_arrays_plistcb (i_resource *self, v_plist_req *req, void *passdata)
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

      /* Process the dictionary */      
      for (dictNode = node->children; dictNode; dictNode = dictNode->next)
      {
        if (strcmp((char *) dictNode->name, "key") == 0)
        {
          char *key = NULL;
          key = (char *) xmlNodeListGetString (req->plist, dictNode->xmlChildrenNode, 1); 

          /* Arrays */
          if (strcmp(key, "arrays") == 0)
          { v_data_arrays_process_arrays (self, datamet, req->plist, dictNode->next->next); }
      
          /* Slots */
          if (strcmp(key, "slots") == 0)
          { v_data_arrays_process_slots (self, datamet, req->plist, dictNode->next->next); }
          
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

int v_data_arrays_process_arrays (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr arrayNode)
{
  v_data_item *dataitem = v_data_static_item ();

  /* Parse dict */
  xmlNodePtr dictNode;
  for (dictNode = arrayNode->children; dictNode; dictNode=dictNode->next)
  {
    if (strcmp((char *) dictNode->name, "dict") == 0)
    {
      /* Process each array */
      char *str;
      i_metric_value *val;

      /* Locate array item */
      v_arrays_item *item;
      int id = v_plist_int_from_dict (plist, dictNode, "id");
      int controller;
      if (datamet == dataitem->arrays_top)
      { controller = 0; }
      else
      { controller = 1; }
      item = v_arrays_get_item (controller, id); 
      if (!item)
      {
        /* Create the array item */
        item = v_arrays_create (self, controller);
      }
      
      /* Stripe size */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "stripe-size");
      i_metric_value_enqueue (self, item->stripesize, val);
      item->stripesize->refresh_result = REFRESULT_OK;
      
      /* RAID level */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "raid-level");
      i_metric_value_enqueue (self, item->raidlevel, val);
      item->raidlevel->refresh_result = REFRESULT_OK;
      
      /* Member count */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (plist, dictNode, "member-count");
      i_metric_value_enqueue (self, item->membercount, val);
      item->membercount->refresh_result = REFRESULT_OK;
      
      /* RAID Type */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "raid-type");
      i_metric_value_enqueue (self, item->raidtype, val);
      item->raidtype->refresh_result = REFRESULT_OK;
      
      /* RAID Status */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (plist, dictNode, "raid-status");
      i_metric_value_enqueue (self, item->raidstatus, val);
      item->raidstatus->refresh_result = REFRESULT_OK;
      
      /* Sector capacity */
      str = v_plist_data_from_dict (plist, dictNode, "sector-capacity");
      if (str)
      {
        val = i_metric_value_create ();
        val->gauge = strtoul (str, NULL, 10);
        free (str);
        i_metric_value_enqueue (self, item->sectorcapacity, val);
        item->sectorcapacity->refresh_result = REFRESULT_OK;
      }
      
      /* Sector capacity */
      str = v_plist_data_from_dict (plist, dictNode, "sector-capacity");
      if (str)
      {
        /* Set value */
        val = i_metric_value_create ();
        val->gauge = strtoul (str, NULL, 10);
        free (str);
        i_metric_value_enqueue (self, item->capacity, val);
        item->capacity->refresh_result = REFRESULT_OK;

        /* Set description */
        char *size_str = i_metric_valstr (item->capacity, val);
        if (size_str)
        {
          char *new_desc;
          asprintf (&new_desc, "Array %i %s", item->index, size_str);
          free (item->cnt->desc_str);
          item->cnt->desc_str = new_desc;
          free (size_str);
        }
      }

      /* Slices */
      xmlNodePtr slicesNode = v_plist_node_from_dict (plist, dictNode, "slices");
      if (slicesNode) v_data_arrays_process_slices (self, item, datamet, plist, slicesNode);

      /* Progress */
      xmlNodePtr progressNode = v_plist_node_from_dict (plist, dictNode, "progress"); 
      if (progressNode) v_data_arrays_process_progress (self, item, datamet, plist, progressNode);

      /* Members */
      xmlNodePtr membersNode = v_plist_node_from_dict (plist, dictNode, "members");
      if (membersNode) v_data_arrays_process_members (self, item, datamet, plist, membersNode);
    }
  }
  
  return 0;
}

int v_data_arrays_process_slices (i_resource *self, v_arrays_item *item, i_metric *datamet, xmlDocPtr plist, xmlNodePtr arrayNode)
{
  /* Parse dict */
  xmlNodePtr dictNode;
  for (dictNode = arrayNode->children; dictNode; dictNode=dictNode->next)
  {
    if (strcmp((char *) dictNode->name, "dict") == 0)
    {
      /* Process each array */
      i_metric_value *val;
      i_metric *met = NULL;

      /* Locate array item */
      int id = v_plist_int_from_dict (plist, dictNode, "id");

      switch (id)
      {
        case 0:
          met = item->slice1_size;
          break;
        case 1:
          met = item->slice2_size;
          break;
        case 2:
          met = item->slice3_size;
          break;
        case 3:
          met = item->slice4_size;
          break;
        case 4:
          met = item->slice5_size;
          break;
        case 5:
          met = item->slice6_size;
          break;
        case 6:
          met = item->slice7_size;
          break;
        case 7:
          met = item->slice8_size;
          break;
      }

      if (met)
      {
        /* Slice size */
        char *str;
        str = v_plist_data_from_dict (plist, dictNode, "size");
        if (str) 
        { 
          val = i_metric_value_create ();
          val->gauge = strtoul (str, NULL, 10);
          free (str);
          if (val->gauge > 0)
          {
            i_metric_value_enqueue (self, met, val);
            met->refresh_result = REFRESULT_OK;
          }
          else 
          { i_metric_value_free (val); }
        }
      }
    }
  }
      
  return 0;
}

int v_data_arrays_process_progress (i_resource *self, v_arrays_item *item, i_metric *datamet, xmlDocPtr plist, xmlNodePtr dictNode)
{
  i_metric_value *val;

  /* Initialize */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (plist, dictNode, "initialize");
  i_metric_value_enqueue (self, item->initialize, val);
  item->initialize->refresh_result = REFRESULT_OK;
  
  /* Add Member */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (plist, dictNode, "add-member");
  i_metric_value_enqueue (self, item->addmember, val);
  item->addmember->refresh_result = REFRESULT_OK;
  
  /* Verify */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (plist, dictNode, "verify");
  i_metric_value_enqueue (self, item->verify, val);
  item->verify->refresh_result = REFRESULT_OK;
  
  /* Expand */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (plist, dictNode, "expand");
  i_metric_value_enqueue (self, item->expand, val);
  item->expand->refresh_result = REFRESULT_OK;
  
  return 0;
}

int v_data_arrays_process_members (i_resource *self, v_arrays_item *item, i_metric *datamet, xmlDocPtr plist, xmlNodePtr arrayNode)
{
  v_data_item *dataitem = v_data_static_item ();

  /* Parse dict */
  xmlNodePtr dictNode;
  for (dictNode = arrayNode->children; dictNode; dictNode=dictNode->next)
  {
    if (strcmp((char *) dictNode->name, "dict") == 0)
    { 
      /* Process each array */
      i_metric_value *val;
      i_metric *met = NULL;
      
      /* Locate array item */
      int id = v_plist_int_from_dict (plist, dictNode, "id");
        
      switch (id)
      { 
        case 1:
          met = item->member1_slot;
          break;
        case 2:
          met = item->member2_slot; 
          break;
        case 3:
          met = item->member3_slot;
          break;
        case 4:
          met = item->member4_slot;
          break;
        case 5:
          met = item->member5_slot;
          break; 
        case 6:
          met = item->member6_slot;
          break;
        case 7:
          met = item->member7_slot;
          break;
      }

      if (met)
      {
        /* Slot-id */
        int slot;
        slot = v_plist_int_from_dict (plist, dictNode, "slot-id");

        /* Check slot value and add controller offset */
        if (slot == 0) continue;
        if (datamet == dataitem->arrays_bottom)
        { slot += 7; }

        /* Set value */
        val = i_metric_value_create ();
        val->integer = slot;
        i_metric_value_enqueue (self, met, val);
        met->refresh_result = REFRESULT_OK;

        /* Set membership on drive item */
        v_drives_item *driveitem = v_drives_x (slot);
        if (driveitem)
        {
          val = i_metric_value_create ();
          val->integer = item->index;
          i_metric_value_enqueue (self, driveitem->arraynumber, val);
          driveitem->arraynumber->refresh_result = REFRESULT_OK;
        }
      }
    }
  }

  return 0;
}

int v_data_arrays_process_slots (i_resource *self, i_metric *datamet, xmlDocPtr plist, xmlNodePtr arrayNode)
{
  v_data_item *dataitem = v_data_static_item ();

  /* Parse dict */
  xmlNodePtr dictNode;
  for (dictNode = arrayNode->children; dictNode; dictNode=dictNode->next)
  {
    if (strcmp((char *) dictNode->name, "dict") == 0)
    { 
      /* Process each array */
      i_metric_value *val;
      
      /* Locate array item */
      int id = v_plist_int_from_dict (plist, dictNode, "id");
        
      /* Check slot value and add controller offset */
      if (datamet == dataitem->arrays_bottom)
      { id += 7; }

      /* Get status node */
      xmlNodePtr statusDictNode = v_plist_node_from_dict (plist, dictNode, "status");
      if (!statusDictNode) continue; 

      /* Get drive item */
      v_drives_item *driveitem = v_drives_x (id);
      if (driveitem)
      {
        /* Online */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (plist, statusDictNode, "online");
        i_metric_value_enqueue (self, driveitem->online, val);
        driveitem->online->refresh_result = REFRESULT_OK;
        
        /* Array Member */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (plist, statusDictNode, "array-member");
        i_metric_value_enqueue (self, driveitem->arraymember, val);
        driveitem->arraymember->refresh_result = REFRESULT_OK;

        /* Rebuilding */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (plist, statusDictNode, "rebuilding");
        i_metric_value_enqueue (self, driveitem->rebuilding, val);
        driveitem->rebuilding->refresh_result = REFRESULT_OK;

        /* Broken RAID Member */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (plist, statusDictNode, "broken-raid-member");
        i_metric_value_enqueue (self, driveitem->brokenraidmember, val);
        driveitem->brokenraidmember->refresh_result = REFRESULT_OK;
        
        /* Disk-cache-enabled */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (plist, statusDictNode, "disk-cache-enabled");
        i_metric_value_enqueue (self, driveitem->diskcache_enabled, val);
        driveitem->diskcache_enabled->refresh_result = REFRESULT_OK;
      }
    }
  }

  return 0;
}


