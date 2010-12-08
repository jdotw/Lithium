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
#include <induction/adminstate.h>
#include <lithium/snmp.h>

#include "osx_server.h"
#include "xsan.h"
#include "xsanvol.h"
#include "xsansp.h"
#include "xsannode.h"
#include "network.h"
#include "plist.h"
#include "data.h"

/*
 * Mac OS X "Xsan Volume getState"
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_xsan_volume_refresh (i_resource *self, i_object *obj, int opcode)
{
  /* Refresh the raw data source for the data_sysinfo data */
  char *url;
  char *command1 = NULL;
  char *command2 = NULL;
  if (v_xsan_version() == 1)
  {
    command1 = "commands/servermgr_nomadvol?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetState%3C%2Fstring%3E%0D%0A%09%3Ckey%3Ename%3C%2Fkey%3E%0D%0A%09%3Cstring%3E";
    command2 = "%3C%2Fstring%3E%0D%0A%09%3Ckey%3Evariant%3C%2Fkey%3E%0D%0A%09%3Cstring%3EwithDetails%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command";
  }
  else if (v_xsan_version() == 2)
  {
    command1 = "/commands/servermgr_xsan?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetVolumeState%3C%2Fstring%3E%0D%0A%09%3Ckey%3Ename%3C%2Fkey%3E%0D%0A%09%3Cstring%3E";
    command2 = "%3C%2Fstring%3E%0D%0A%09%3Ckey%3Evariant%3C%2Fkey%3E%0D%0A%09%3Cstring%3EwithDetails%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command";
  }
  v_xsanvol_item *vol = obj->itemptr;
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      if (v_powerstate()) 
      {
        asprintf (&url, "https://%s:311/%s%s%s", v_plist_ip(), command1, vol->raw_volume_name, command2);
        static_req = v_plist_get (self, url, v_data_xsan_volume_plistcb, obj);
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

int v_data_xsan_volume_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_object *obj = passdata;
  v_xsanvol_item *vol = obj->itemptr;

  /* Clear req */
  static_req = NULL;
    
  /* Check result */
  if (!req || !vol)
  { 
    obj->refresh_result = REFRESULT_TOTAL_FAIL;
    i_entity_refresh_terminate (ENTITY(obj));
    return -1; 
  }

  /* Parse XML plist */
  xmlNodePtr node;
  for (node = req->root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      i_metric_value *val;
      
      /* State */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, node, "state");
      i_metric_value_enqueue (self, vol->state, val);
      vol->state->refresh_result = REFRESULT_OK;
      
      /* VOlume Info */
      xmlNodePtr volNode = v_plist_node_from_dict (req->plist, node, "volumeInfo");
      if (volNode) v_data_xsan_process_volumeinfo (self, req, volNode, vol);
    }
  }

  /* Set result and terminate */
  obj->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(obj));
  
  return 0;
}

int v_data_xsan_process_volumeinfo (i_resource *self, v_plist_req *req, xmlNodePtr volNode, v_xsanvol_item *vol)
{
  /* VolumeStats */
  xmlNodePtr statsNode = v_plist_node_from_dict (req->plist, volNode, "VolumeStats");
  if (statsNode)
  { v_data_xsan_process_volumestats (self, req, statsNode, vol); }

  /* Stripe Groups */
  xmlNodePtr groupsNode = v_plist_node_from_dict (req->plist, volNode, "StripeGroups");
  if (groupsNode)
  { v_data_xsan_process_stripegroups (self, req, groupsNode, vol); }

  return 0;
}

int v_data_xsan_process_volumestats (i_resource *self, v_plist_req *req, xmlNodePtr dictNode, v_xsanvol_item *volume)
{
  i_metric_value *val;

  /* Bandwidth Management */
  val = i_metric_value_create ();
  val->integer = v_plist_bool_from_dict (req->plist, dictNode, "BandwidthManagementEnabled"); 
  i_metric_value_enqueue (self, volume->bandwidth_management, val);
  volume->bandwidth_management->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->bandwidth_management));
  
  /* Creation Time */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (req->plist, dictNode, "CreationTime"); 
  i_metric_value_enqueue (self, volume->creation_time, val);
  volume->creation_time->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->creation_time));
  
  /* Has Valid License */
  val = i_metric_value_create ();
  val->integer = v_plist_bool_from_dict (req->plist, dictNode, "HasValidLicense"); 
  i_metric_value_enqueue (self, volume->license_valid, val);
  volume->license_valid->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->license_valid));
  
  /* Host */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (req->plist, dictNode, "Host"); 
  i_metric_value_enqueue (self, volume->host, val);
  volume->host->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->host));
  
  /* Host IP */
  val = i_metric_value_create ();
  val->str = v_plist_data_from_dict (req->plist, dictNode, "IPAddress"); 
  i_metric_value_enqueue (self, volume->host_ip, val);
  volume->host_ip->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->host_ip));
  
  /* Message Buffer Size */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (req->plist, dictNode, "MessageBufferSize"); 
  i_metric_value_enqueue (self, volume->msgbuf_size, val);
  volume->msgbuf_size->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->msgbuf_size));

  /* Connected Clients */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, dictNode, "NumConnectedClients"); 
  i_metric_value_enqueue (self, volume->connected_clients, val);
  volume->connected_clients->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->connected_clients));
  
  /* Num Devices */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, dictNode, "NumDevices"); 
  i_metric_value_enqueue (self, volume->devices, val);
  volume->devices->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->devices));
  
  /* Stripe Groups */
  val = i_metric_value_create ();
  val->gauge = v_plist_int_from_dict (req->plist, dictNode, "NumStripeGroups"); 
  i_metric_value_enqueue (self, volume->stripe_groups, val);
  volume->stripe_groups->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->stripe_groups));
  
  /* Port */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (req->plist, dictNode, "Port"); 
  i_metric_value_enqueue (self, volume->port, val);
  volume->port->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->port));
  
  /* Priority */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (req->plist, dictNode, "Priority"); 
  i_metric_value_enqueue (self, volume->priority, val);
  volume->priority->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->priority));
  
  /* ProcessID */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (req->plist, dictNode, "ProcessID"); 
  i_metric_value_enqueue (self, volume->pid, val);
  volume->pid->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->pid));
  
  /* QOS */
  val = i_metric_value_create ();
  val->integer = v_plist_bool_from_dict (req->plist, dictNode, "QOSEnabled"); 
  i_metric_value_enqueue (self, volume->qos, val);
  volume->qos->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->qos));
  
  /* Quotas */
  val = i_metric_value_create ();
  val->integer = v_plist_bool_from_dict (req->plist, dictNode, "Quotas"); 
  i_metric_value_enqueue (self, volume->quotas, val);
  volume->quotas->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->quotas));
  
  /* State */
  val = i_metric_value_create ();
  val->integer = v_plist_int_from_dict (req->plist, dictNode, "State"); 
  i_metric_value_enqueue (self, volume->state, val);
  volume->state->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->state));
  
  /* Remaining Size */
  char *str = v_plist_data_from_dict (req->plist, dictNode, "RemainingSize");
  if (str)
  {
    i_metric_value *val;
    double flt;
    flt = atof (str) / (1024 * 1024);
    free (str);
    str = NULL;
    val = i_metric_value_create ();
    val->gauge = (unsigned int) flt;
    i_metric_value_enqueue (self, volume->bytes_free, val);
    volume->bytes_free->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(volume->bytes_free));
  }
  else
  { volume->bytes_free->refresh_result = REFRESULT_TOTAL_FAIL; }

  /* Total Size */
  str = v_plist_data_from_dict (req->plist, dictNode, "TotalSize");
  if (str)
  {
    i_metric_value *val;
    double flt;
    flt = atof (str) / (1024 * 1024);
    free (str);
    str = NULL;
    val = i_metric_value_create ();
    val->gauge = (unsigned int) flt;
    i_metric_value_enqueue (self, volume->bytes_total, val);
    volume->bytes_total->refresh_result = REFRESULT_OK;
    i_entity_refresh_terminate (ENTITY(volume->bytes_total));
  }
  else
  { volume->bytes_total->refresh_result = REFRESULT_TOTAL_FAIL; }

  /*
   * MDC Host Checking
   *
   * These routines check to see if:
   *
   * a) We are the MDC hosting this volume
   * b) WHether or not we recently lost the role of MDC
   */

  /* Check to see if we are the active MDC */
  i_container *netcnt = v_network_cnt();
  v_network_item *iface = NULL;
  for (i_list_move_head(netcnt->item_list); (iface=i_list_restore(netcnt->item_list))!=NULL; i_list_move_next(netcnt->item_list))
  {
    char *iface_ip_str = i_metric_valstr (iface->ip, NULL);
    char *mdc_ip_str = i_metric_valstr (volume->host_ip, NULL);
    if (iface_ip_str && mdc_ip_str && strcmp(iface_ip_str, mdc_ip_str) == 0)
    {
      /* 
       * MDC IP Address Match Found 
       *
       * This device is the MDC hosting this volume.
       */
      if (volume->host_flag != 2)
      {
        /* THis host has recently become the MDC -- Enable Triggers */
        i_adminstate_change (self, ENTITY(volume->host_state), ENTADMIN_ENABLED); 
      }
      volume->host_flag = 2;      /* Set Is-MDC flag */
      if (iface_ip_str) free (iface_ip_str);
      if (mdc_ip_str) free (mdc_ip_str);
      iface_ip_str = NULL;
      mdc_ip_str = NULL;
      break;
    }
    
    if (iface_ip_str) free (iface_ip_str);
    if (mdc_ip_str) free (mdc_ip_str);
    iface_ip_str = NULL;
    mdc_ip_str = NULL;
  }
  if (!iface)
  {
    /* 
     * This device is NOT the MDC hosting this volume
     */
    if (volume->host_flag == 2)
    {
      /* This device recently lost the MDC role -- Incident will be raised */
    }
    volume->host_flag = 1;      /* Set Is-Not-MDC flag */
  }

  /* Enqueue new host state value */
  val = i_metric_value_create ();
  val->integer = volume->host_flag; 
  i_metric_value_enqueue (self, volume->host_state, val);
  volume->host_state->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(volume->host_state));

  return 0;  
}

int v_data_xsan_process_stripegroups (i_resource *self, v_plist_req *req, xmlNodePtr groupsNode, v_xsanvol_item *volume)
{
  /* Parse XML plist */
  xmlNodePtr groupNode;
  for (groupNode = groupsNode->children; groupNode; groupNode = groupNode->next)
  {
    if (strcmp((char *)groupNode->name, "dict") == 0)
    {
      i_metric_value *val;

      /* Process a stripe group */
      /* Get Stripe Group */
      char *spname_str = v_plist_data_from_dict (req->plist, groupNode, "Name");
      if (!spname_str) continue;
      char *name_str;
      asprintf (&name_str,  "%s %s", volume->obj->desc_str, spname_str);
      free (spname_str);
      v_xsansp_item *sp = v_xsansp_get (volume->sp_cnt, name_str);
      if (!sp)
      {
        sp = v_xsansp_create (self, volume, name_str);
        if (!sp)
        { i_printf (1, "v_data_xsan_process_stripegroups failed to create storage pool %s", name_str); continue; }
      }
      if (name_str) free (name_str);
      name_str = NULL;
      
      /* Enabled */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Enabled");
      i_metric_value_enqueue (self, sp->enabled, val);
      sp->enabled->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(sp->enabled));

      /* Mirror Index */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (req->plist, groupNode, "MirrorIndex");
      i_metric_value_enqueue (self, sp->mirrorindex, val);
      sp->mirrorindex->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(sp->mirrorindex));

      /* Native Key Value */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (req->plist, groupNode, "NativeKeyValue");
      i_metric_value_enqueue (self, sp->nativekeyvalue, val);
      sp->nativekeyvalue->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(sp->nativekeyvalue));

      /* Devices */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (req->plist, groupNode, "NumDevices");
      i_metric_value_enqueue (self, sp->devices, val);
      sp->devices->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(sp->devices));

      /* Realtime */
      val = i_metric_value_create ();
      val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Realtime");
      i_metric_value_enqueue (self, sp->realtime, val);
      sp->realtime->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(sp->realtime));

      /* Remaining Size */
      char *str = v_plist_data_from_dict (req->plist, groupNode, "RemainingSize");
      if (str)
      { 
        i_metric_value *val;
        double flt;
        flt = atof (str) / (1024 * 1024);
        free (str);
        str = NULL;
        val = i_metric_value_create ();
        val->gauge = (unsigned int) flt;
        i_metric_value_enqueue (self, sp->bytes_free, val);
        sp->bytes_free->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(sp->bytes_free));
      }   
      else  
      { sp->bytes_free->refresh_result = REFRESULT_TOTAL_FAIL; }
        
      /* Total Size */
      str = v_plist_data_from_dict (req->plist, groupNode, "TotalSize");
      if (str)
      {   
        i_metric_value *val;
        double flt;
        flt = atof (str) / (1024 * 1024);
        free (str);
        str = NULL;
        val = i_metric_value_create ();
        val->gauge = (unsigned int) flt;
        i_metric_value_enqueue (self, sp->bytes_total, val);
        sp->bytes_total->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(sp->bytes_total));
      }   
      else
      { sp->bytes_total->refresh_result = REFRESULT_TOTAL_FAIL; }
    }
  }

  return 0;
}

