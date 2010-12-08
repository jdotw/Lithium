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

#include "services.h"
#include "xsanvol.h"
#include "xsansp.h"
#include "xsannode.h"
#include "plist.h"
#include "data.h"

/*
 * Mac OS X "xsan getState"
 */

static v_plist_req *static_req = NULL;

/* Data Source Refrsesh */

int v_data_xsan_settings_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysinfo data */
  char *url;
  char *command = "/commands/servermgr_nomadfss?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EreadSettings%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command";
  
  switch (opcode & REFOP_ALL)
  {
    case REFOP_REFRESH:     /* Begin the refresh processa */
      if (v_powerstate()) 
      {
        asprintf (&url, "https://%s:311/%s", v_plist_ip(), command);
        static_req = v_plist_get (self, url, v_data_xsan_settings_plistcb, met);
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

int v_data_xsan_settings_plistcb (i_resource *self, v_plist_req *req, void *passdata)
{
  i_metric *datamet = passdata;

  /* Clear req */
  static_req = NULL;
    
  /* Check result */
  if (!req || !service)
  { return -1; }

  /* Parse XML plist */
  xmlNodePtr node;
  for (node = req->root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get config node */
//      xmlNodePtr configNode = v_plist_node_from_dict (req->plist, node, "configuration");
//      i_printf (1, "v_data_xsan_settings_plistcb got confg node as %p", configNode);
//      if (!configNode) continue;

      /* Computers Array */
//      xmlNodePtr computersArray = v_plist_node_from_dict (req->plist, configNode, "Computers");
//      if (computersArray)
//      { v_data_xsan_process_computers (self, req, computersArray); }

      /* Disks Array */
//      xmlNodePtr disksArray = v_plist_node_from_dict (req->plist, configNode, "Disks");
//      if (disksArray)
//      { v_data_xsan_process_disks (self, req, disksArray); }

      /* Volumes */
      xmlNodePtr volumesArray = v_plist_node_from_dict (req->plist, configNode, "Volumes");
      if (volumesArray)
      { v_data_xsan_process_volumesettings (self, req, volumesArray); }
    }
  }

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
  return 0;
}

int v_data_xsan_process_computers (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  xmlNodePtr compNode;

  for (compNode = arrayNode->children; compNode; compNode = compNode->next)
  {
    if (strcmp((char *)compNode->name, "dict") == 0)
    {
      /* Get Properties */
      xmlNodePtr dictNode = v_plist_node_from_dict (req->plist, compNode, "properties");
      i_printf (1, "v_data_xsan_process_computers properties node is %p", dictNode);
      if (!dictNode) continue;
      
      /* Get Computer */
      char *name_str = v_plist_data_from_dict (req->plist, dictNode, "computerName");
      i_printf (0, "v_data_xsan_process_computers processing computer %s", name_str);
      if (!name_str) continue;
      v_xsancomputer_item *comp = v_xsancomputer_get (name_str);
      if (!comp)
      {
        comp = v_xsancomputer_create (self, name_str);
        if (!comp)
        { i_printf (1, "v_data_xsan_process_computers failed to create computer %s", name_str); continue; }
      }

      i_metric_value *val;

      /* IP */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, compNode, "ipAddress"); 
      i_metric_value_enqueue (self, comp->ip, val);
      comp->ip->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->ip));

      /* Client FS Version */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, dictNode, "clientFileSystemVersion"); 
      i_metric_value_enqueue (self, comp->cfsversion, val);
      comp->cfsversion->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->cfsversion));

      /* Server FS Version */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, dictNode, "serverFileSystemVersion"); 
      i_metric_value_enqueue (self, comp->sfsversion, val);
      comp->sfsversion->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->sfsversion));

      /* ComputerName */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, dictNode, "computerName"); 
      i_metric_value_enqueue (self, comp->name, val);
      comp->name->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->name));

      /* Host Name */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, dictNode, "hostName"); 
      i_metric_value_enqueue (self, comp->hostname, val);
      comp->hostname->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->hostname));

      /* Rendezvous */
      val = i_metric_value_create ();
      val->gauge = v_plist_int_from_dict (req->plist, dictNode, "rendezvousName"); 
      comp->rendezname->refresh_result = REFRESULT_OK;
      i_metric_value_enqueue (self, comp->rendezname, val);
      i_entity_refresh_terminate (ENTITY(comp->rendezname));

      /* FailOver Priority */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (req->plist, dictNode, "failoverPriority"); 
      i_metric_value_enqueue (self, comp->failoverpriority, val);
      comp->failoverpriority->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->failoverpriority));

      /* fsmpm Running */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (req->plist, dictNode, "fsmpmRunning"); 
      i_metric_value_enqueue (self, comp->fsmpmrunning, val);
      comp->fsmpmrunning->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->fsmpmrunning));

      /* fsnameservers */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, dictNode, "fsnameserversContents"); 
      i_metric_value_enqueue (self, comp->fsnameservers, val);
      comp->fsnameservers->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->fsnameservers));

      /* Hardware */
      xmlNodePtr hwNode = v_plist_node_from_dict (req->plist, dictNode, "hardwareDescription");
      if (hwNode)
      {
        /* CPU Count */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, hwNode, "cpuCount"); 
        i_metric_value_enqueue (self, comp->cpucount, val);
        comp->cpucount->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(comp->cpucount));

        /* CPU Speed */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, hwNode, "cpuHz"); 
        i_metric_value_enqueue (self, comp->cpuspeed, val);
        comp->cpuspeed->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(comp->cpuspeed));

        /* CPU Type */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, hwNode, "cpuKind"); 
        i_metric_value_enqueue (self, comp->cpukind, val);
        comp->cpukind->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(comp->cpukind));

        /* RAM Bytes */
        val = i_metric_value_create ();
        val->gauge = v_plist_int_from_dict (req->plist, hwNode, "ramBytes"); 
        i_metric_value_enqueue (self, comp->ram, val);
        comp->ram->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(comp->ram));
      }
      
      /* License */
      val = i_metric_value_create ();
      val->integer = v_plist_int_from_dict (req->plist, dictNode, "licenseIsInvalid"); 
      i_metric_value_enqueue (self, comp->licensevalid, val);
      comp->licensevalid->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->licensevalid));

      /* Role */
      val = i_metric_value_create ();
      val->str = v_plist_data_from_dict (req->plist, dictNode, "role"); 
      i_metric_value_enqueue (self, comp->role, val);
      comp->role->refresh_result = REFRESULT_OK;
      i_entity_refresh_terminate (ENTITY(comp->role));
    }
  }

  return 0;

}

int v_data_xsan_process_volumesettings (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  xmlNodePtr dictNode;

  for (dictNode = arrayNode->children; dictNode; dictNode = dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Get Volume */
      char *name_str = v_plist_data_from_dict (req->plist, dictNode, "Name");
      i_printf (0, "v_data_xsan_process_volumesettings  processing volume settings %s", name_str);
      if (!name_str) continue;
      v_xsanvol_item *volume = v_xsanvol_get (name_str);
      if (!volume)
      {
        volume = v_xsanvol_create (self, name_str);
        if (!volume)
        { i_printf (1, "v_data_xsan_process_volumesettings failed to create volume %s", name_str); continue; }
      }

      i_metric_value *val;

      /* Get Globals Dict */
      xmlNodePtr globalsNode = v_plist_node_from_dict (req->plist, dictNode, "Globals");
      if (globalsNode)
      {
        /* Alloc Strat */
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, globalsNode, "AllocationStrategy"); 
        i_metric_value_enqueue (self, volume->allocation_strategy, val);
        volume->allocation_strategy->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->allocation_strategy));

        /* Buffer Cache */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "BufferCacheSize"); 
        i_metric_value_enqueue (self, volume->buffer_cache_size, val);
        volume->buffer_cache_size->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->buffer_cache_size));

        /* Debug */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "Debug"); 
        i_metric_value_enqueue (self, volume->debug, val);
        volume->debug->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->debug));

        /* Enforce ACLs */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, globalsNode, "EnforceACLs"); 
        i_metric_value_enqueue (self, volume->enforce_acls, val);
        volume->enforce_acls->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->enforce_acls));

        /* File locks */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, globalsNode, "File Locks"); 
        i_metric_value_enqueue (self, volume->file_locks, val);
        volume->file_locks->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->file_locks));

        /* Force Stripe Align */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, globalsNode, "ForceStripeAlignment"); 
        i_metric_value_enqueue (self, volume->force_stripe_alignment, val);
        volume->force_stripe_alignment->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->force_stripe_alignment));

        /* FS Block Size */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "FsBlockSize"); 
        i_metric_value_enqueue (self, volume->fs_block_size, val);
        volume->fs_block_size->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->fs_block_size));

        /* Global Super User */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, globalsNode, "GlobalSuperUser"); 
        i_metric_value_enqueue (self, volume->global_super_user, val);
        volume->global_super_user->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->global_super_user));

        /* InodeCacheSize */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "InodeCacheSize"); 
        i_metric_value_enqueue (self, volume->inode_cache_size, val);
        volume->inode_cache_size->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->inode_cache_size));

        /* Inode Expand Inc */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "InodeExpandInc"); 
        i_metric_value_enqueue (self, volume->inode_expand_inc, val);
        volume->inode_expand_inc->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->inode_expand_inc));

        /* Inode Expand Max */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "InodeExpandMax"); 
        i_metric_value_enqueue (self, volume->inode_expand_max, val);
        volume->inode_expand_max->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->inode_expand_max));

        /* Inode Expand Min */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "InodeExpandMin"); 
        i_metric_value_enqueue (self, volume->inode_expand_min, val);
        volume->inode_expand_min->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->inode_expand_min));

        /* JournalSize */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "JournalSize"); 
        i_metric_value_enqueue (self, volume->journal_size, val);
        volume->journal_size->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->journal_size));

        /* MaxConnections */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "MaxConnections"); 
        i_metric_value_enqueue (self, volume->max_connections, val);
        volume->max_connections->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->max_connections));

        /* MaxLogSize */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "MaxLogSize"); 
        i_metric_value_enqueue (self, volume->max_logsize, val);
        volume->max_logsize->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->max_logsize));

        /* Quotas */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, globalsNode, "Quotas"); 
        i_metric_value_enqueue (self, volume->quotas, val);
        volume->quotas->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->quotas));

        /* ThreadPoolSize */
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, globalsNode, "ThreadPoolSize"); 
        i_metric_value_enqueue (self, volume->threadpool_size, val);
        volume->threadpool_size->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->threadpool_size));

        /* UnixIdFabricationOnWindows */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, globalsNode, "UnixIdFabricationOnWindows"); 
        i_metric_value_enqueue (self, volume->unix_fab_window, val);
        volume->unix_fab_window->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->unix_fab_window));

        /* WindowsSecurity */
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, globalsNode, "WindowsSecurity"); 
        i_metric_value_enqueue (self, volume->windows_security, val);
        volume->windows_security->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(volume->windows_security));

        /* FIX Incomplete */
      }

      /* Get StripeGroups */
      xmlNodePtr stripeGroups = v_plist_node_from_dict (req->plist, dictNode, "StripeGroups");
      if (stripeGroups)
      {
        xmlNodePtr groupNode;
        for (groupNode = stripeGroups->children; groupNode; groupNode = groupNode->next)
        {
          if (strcmp((char *)dictNode->name, "dict") == 0)
          {
            /* Process a stripe group */
            /* Get Stripe Group */
            name_str = v_plist_data_from_dict (req->plist, groupNode, "StripeGroup");
            i_printf (0, "v_data_xsan_process_volumesettings processing sp %s", name_str);
            if (!name_str) continue;
            v_xsansp_item *sp = v_xsansp_get (volume->sp_cnt, name_str);
            if (!sp)
            {
              sp = v_xsansp_create (self, volume, name_str);
              if (!sp)
              { i_printf (1, "v_data_xsan_process_volumesettings failed to create storage pool %s", name_str); continue; }
            }
            
            /* Exclusive */
            val = i_metric_value_create ();
            val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Exclusive"); 
            i_metric_value_enqueue (self, sp->exclusive, val);
            sp->exclusive->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->exclusive));

            /* Get Nodes */
            xmlNodePtr nodesArray = v_plist_node_from_dict (req->plist, groupNode, "Nodes");
            if (nodesArray)
            {
              xmlNodePtr nodeNode;
              for (nodeNode = nodesArray->children; nodeNode; nodeNode = nodeNode->next)
              {
                if (strcmp((char *)dictNode->name, "dict") == 0)
                {
                  /* Process a node (disk) */
                  /* Get name */
                  char *disk_name = v_plist_data_from_dict (req->plist, nodeNode, "NodeDiskDevice");
                  if (!disk_name) continue;
                  v_xsannode_item *node_item = v_xsannode_get_node (sp->node_cnt, disk_name);
                  if (!node_item)
                  {
                    node_item = v_xsanspdetail_create (self, sp, disk_name);
                  }
                }
              }
            }
            /* End of Node Processing */
          }
        }
      }
    }
  }

  return 0;
}

