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
#include "device/snmp.h"

#include "osx_server.h"
#include "xsan.h"
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

int v_data_xsan_process_volumesettings (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode);

/* Data Source Refrsesh */

int v_data_xsan_settings_refresh (i_resource *self, i_metric *met, int opcode)
{
  /* Refresh the raw data source for the data_sysinfo data */
  char *url;
  char *command = NULL;
  if (v_xsan_version() == 1)
  { command = "/commands/servermgr_nomadfss?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EreadSettings%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command"; }
  else if (v_xsan_version() == 2)
  { command = "/commands/servermgr_xsan?input=%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0D%0A%3Cplist+version%3D%220.9%22%3E%0D%0A%3Cdict%3E%0D%0A%09%3Ckey%3Ecommand%3C%2Fkey%3E%0D%0A%09%3Cstring%3EgetVolumes%3C%2Fstring%3E%0D%0A%3C%2Fdict%3E%0D%0A%3C%2Fplist%3E%0D%0A&send=Send+Command"; }
  
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
  if (!req)
  { return -1; }

  /* Parse XML plist */
  xmlNodePtr node;
  for (node = req->root_node->children; node; node = node->next)
  {
    if (strcmp((char *)node->name, "dict") == 0)
    {
      /* Get config node */
      xmlNodePtr configNode = v_plist_node_from_dict (req->plist, node, "configuration");
      if (!configNode && v_xsan_version() == 1) continue;

      /* Computers Array */
//      xmlNodePtr computersArray = v_plist_node_from_dict (req->plist, configNode, "Computers");
//      if (computersArray)
//      { v_data_xsan_process_computers (self, req, computersArray); }

      /* Volumes */
      xmlNodePtr volumesArray = NULL;
      if (v_xsan_version() == 1 && configNode)
      { volumesArray = v_plist_node_from_dict (req->plist, configNode, "Volumes"); }
      else if (v_xsan_version() == 2)
      { volumesArray = v_plist_node_from_dict (req->plist, node, "volumes"); }
      if (volumesArray)
      { v_data_xsan_process_volumesettings (self, req, volumesArray); }
      
      /* Disks Array */
      if (configNode)
      {
        xmlNodePtr disksArray = v_plist_node_from_dict (req->plist, configNode, "Disks");
        if (disksArray)
        { v_data_xsan_process_disks (self, req, disksArray); }
      }
    }
  }

  /* Set result and terminate */
  datamet->refresh_result = REFRESULT_OK;
  i_entity_refresh_terminate (ENTITY(datamet));
  
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
      if (!name_str) continue;
      v_xsanvol_item *volume = v_xsanvol_get (name_str);
      if (!volume)
      {
        volume = v_xsanvol_create (self, name_str);
        if (!volume)
        { i_printf (1, "v_data_xsan_process_volumesettings failed to create volume %s", name_str); continue; }
      }
      if (name_str) free (name_str);
      name_str = NULL;

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
            char *spname_str = v_plist_data_from_dict (req->plist, groupNode, "StripeGroup");
            if (!spname_str) continue;
            asprintf (&name_str, "%s %s", volume->obj->desc_str, spname_str);
            free (spname_str);
            spname_str = NULL;
            v_xsansp_item *sp = v_xsansp_get (volume->sp_cnt, name_str);
            if (!sp)
            {
              sp = v_xsansp_create (self, volume, name_str);
              if (!sp)
              { i_printf (1, "v_data_xsan_process_volumesettings failed to create storage pool %s", name_str); continue; }
            }
            if (name_str) free (name_str);
            name_str = NULL;
            
            /* Exclusive */
            val = i_metric_value_create ();
            val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Exclusive"); 
            i_metric_value_enqueue (self, sp->exclusive, val);
            sp->exclusive->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->exclusive));

            /* Affinity */
            val = i_metric_value_create ();
            val->str = v_plist_data_from_dict (req->plist, groupNode, "Affinity"); 
            i_metric_value_enqueue (self, sp->affinity, val);
            sp->affinity->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->affinity));

            /* Journal */
            val = i_metric_value_create ();
            val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Journal"); 
            i_metric_value_enqueue (self, sp->journal, val);
            sp->journal->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->journal));

            /* Metadata */
            val = i_metric_value_create ();
            val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Metadata"); 
            i_metric_value_enqueue (self, sp->metadata, val);
            sp->metadata->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->metadata));

            /* Multipath Method */
            val = i_metric_value_create ();
            val->str = v_plist_data_from_dict (req->plist, groupNode, "MultiPathMethod"); 
            i_metric_value_enqueue (self, sp->multipathmethod, val);
            sp->multipathmethod->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->multipathmethod));

            /* Read */
            val = i_metric_value_create ();
            val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Read"); 
            i_metric_value_enqueue (self, sp->read, val);
            sp->read->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->read));

            /* Write */
            val = i_metric_value_create ();
            val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Write"); 
            i_metric_value_enqueue (self, sp->write, val);
            sp->write->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->write));

            /* Status */
            val = i_metric_value_create ();
            val->integer = v_plist_bool_from_dict (req->plist, groupNode, "Status"); 
            i_metric_value_enqueue (self, sp->status, val);
            sp->status->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->status));

            /* Stripe Breadth */
            val = i_metric_value_create ();
            val->integer = v_plist_int_from_dict (req->plist, groupNode, "StripeBreadth"); 
            i_metric_value_enqueue (self, sp->stripebreadth, val);
            sp->stripebreadth->refresh_result = REFRESULT_OK;
            i_entity_refresh_terminate (ENTITY(sp->stripebreadth));

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
                  v_xsannode_item *node_item = v_xsannode_get (sp->node_cnt, disk_name);
                  if (!node_item)
                  {
                    node_item = v_xsannode_create (self, sp, disk_name);
                  }
                  if (disk_name) free (disk_name);
                  disk_name = NULL;

                  /* Rotiation Index */
                  val = i_metric_value_create ();
                  val->integer = v_plist_int_from_dict (req->plist, nodeNode, "NodeRotationIndex"); 
                  i_metric_value_enqueue (self, node_item->noderotationindex, val);
                  node_item->noderotationindex->refresh_result = REFRESULT_OK;
                  i_entity_refresh_terminate (ENTITY(node_item->noderotationindex));
                }
              }
            }
            /* End of Node Processing */
          }
        }
      }

      /* Disks */
      xmlNodePtr disksArray = v_plist_node_from_dict (req->plist, dictNode, "Disks");
      if (disksArray)
      { v_data_xsan_process_disks (self, req, disksArray); }

      /* Disk Types */
      xmlNodePtr diskTypesArray = v_plist_node_from_dict (req->plist, dictNode, "DiskTypes");
      if (diskTypesArray)
      { v_data_xsan_process_disks (self, req, diskTypesArray); }
    }
  }

  return 0;
}

int v_data_xsan_process_disks (i_resource *self, v_plist_req *req, xmlNodePtr arrayNode)
{
  int diskTypeInfo = 0;
  xmlNodePtr dictNode;

  for (dictNode = arrayNode->children; dictNode; dictNode = dictNode->next)
  {
    if (strcmp((char *)dictNode->name, "dict") == 0)
    {
      /* Get Node */
      char *name_str = NULL;
      if (v_xsan_version() == 1)
      {
        name_str = v_plist_data_from_dict (req->plist, dictNode, "name");
      }
      else if (v_xsan_version() == 2)
      {
        name_str = v_plist_data_from_dict (req->plist, dictNode, "Disk");
        if (!name_str)
        {
          name_str = v_plist_data_from_dict (req->plist, dictNode, "DiskType");
          if (name_str)
          {
            char *type_str = strstr (name_str, "Type");
            if (type_str)
            { *type_str = '\0'; }
            diskTypeInfo = 1;
          }
        }
      }
  
      if (!name_str) continue;
      v_xsannode_item *node = v_xsannode_get_global (name_str);
      free (name_str);
      name_str = NULL;
      if (!node)
      { continue; }

      i_metric_value *val;

      /* Block Device */
      if (v_xsan_version() == 1)
      {
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, dictNode, "blkDevName");
        i_metric_value_enqueue (self, node->blockdev, val);
        node->blockdev->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->blockdev));
      }
      
      /* Host Number */
      if (v_xsan_version() == 1)
      {
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, dictNode, "hostNum");
        i_metric_value_enqueue (self, node->hostnumber, val);
        node->hostnumber->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->hostnumber));
      }
      
      /* Host Inquiry String */
      if (v_xsan_version() == 1)
      {
        val = i_metric_value_create ();
        val->str = v_plist_data_from_dict (req->plist, dictNode, "inquiryString");
        i_metric_value_enqueue (self, node->hostinquiry, val);
        node->hostinquiry->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->hostinquiry));
      }
      
      /* LUN */
      if (v_xsan_version() == 1)
      {
        val = i_metric_value_create ();
        val->integer = v_plist_int_from_dict (req->plist, dictNode, "lunID");
        i_metric_value_enqueue (self, node->lun, val);
        node->lun->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->lun));
      }
      
      /* Max Sectors */
      if (v_xsan_version() == 1 || diskTypeInfo == 1)
      {
        val = i_metric_value_create ();
        if (v_xsan_version() == 1)
        { val->gauge = v_plist_int_from_dict (req->plist, dictNode, "maxSectors"); }
        else if (v_xsan_version() == 2)
        { val->gauge = v_plist_int_from_dict (req->plist, dictNode, "Sectors"); }
        i_metric_value_enqueue (self, node->maxsectors, val);
        node->maxsectors->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->maxsectors));
      }
      
      /* Sector Size */
      if (v_xsan_version() == 1 || diskTypeInfo == 1)
      {
        val = i_metric_value_create ();
        if (v_xsan_version() == 1)
        { val->gauge = v_plist_int_from_dict (req->plist, dictNode, "sectorSize"); }
        else if (v_xsan_version() == 2)
        { val->gauge = v_plist_int_from_dict (req->plist, dictNode, "SectorSize"); }
        i_metric_value_enqueue (self, node->sectorsize, val);
        node->sectorsize->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->sectorsize));
      }
      
      /* WWN */
      char *wwn = v_plist_data_from_dict (req->plist, dictNode, "serialNumber");
      if (wwn && strlen (wwn) >= 16)
      {
        /* Process WWN string */
        char *wwn_buf;
        asprintf (&wwn_buf, "%c%c:%c%c:%c%c:%c%c:%c%c:%c%c:%c%c:%c%c", 
         wwn[0], wwn[1], wwn[2], wwn[3], wwn[4], wwn[5], wwn[6], wwn[7], 
         wwn[8], wwn[9], wwn[10], wwn[11], wwn[12], wwn[13], wwn[14], wwn[15]);
        val = i_metric_value_create ();
        val->str = wwn_buf;
        i_metric_value_enqueue (self, node->wwn, val);
        node->wwn->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->wwn));

        if (node->lun_registered == 0)
        { i_lunregistry_register (self, 2, node->sp_item->obj, wwn_buf, 0); node->lun_registered = 1; }
      }
      if (wwn) free (wwn);
      wwn = NULL;

      /* Size */
      if (v_xsan_version () == 1 || diskTypeInfo == 1)
      {
        val = i_metric_value_create ();
        if (v_xsan_version() == 1)
        { val->gauge = v_plist_int_from_dict (req->plist, dictNode, "size"); }
        else if (v_xsan_version() == 2)
        { val->gauge = v_plist_int_from_dict (req->plist, dictNode, "Sectors"); }
        i_metric_value_enqueue (self, node->size, val);
        node->size->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->size));
      }

      /* Unlabeled */
      if (v_xsan_version () == 1)
      {
        val = i_metric_value_create ();
        val->integer = v_plist_bool_from_dict (req->plist, dictNode, "unlabeled");
        i_metric_value_enqueue (self, node->unlabeled, val);
        node->unlabeled->refresh_result = REFRESULT_OK;
        i_entity_refresh_terminate (ENTITY(node->unlabeled));
      }
    }
  }

  return 0;
}
