#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/entity_xmlsync.h>
#include <induction/customer.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/construct.h>
#include <induction/list.h>
#include <induction/timer.h>

#include "device.h"

/* 
 * Device Resource Functions 
 */

static int static_restart_queue = 0;

int l_device_res_queue_depth ()
{ return static_restart_queue; }

/* 
 * Resource (Re)start 
 */

int l_device_res_restart (i_resource *self, i_device *dev)
{
  char *fsroot;
  char *entaddr_str;

  /* Confirm license */
  if (dev->licensed == 0)
  { 
    i_printf (1, "l_device_res_restart failed, devices %s is unlicensed", dev->name_str);
    return 0; 
  }

  /* Disable entity sync */
//  i_entity_xmlsync_disable(self, ENTITY(dev));

  /* Destroy existing resource */
  l_device_res_destroy (self, dev);

  /* Create fsroot */
  fsroot = i_entity_path (self, ENTITY(dev), 0, ENTPATH_ROOT|ENTPATH_CREATE);
  if (!fsroot)
  { i_printf (1, "l_device_res_restart failed to create fsroot for %s", dev->name_str); return -1; }

  /* Create entity address */
  entaddr_str = i_entity_address_string (ENTITY(dev), NULL);
  if (!entaddr_str)
  { i_printf (1, "l_device_res_restart failed to create entity address string for %s", dev->name_str); free (fsroot); return -1; }

  /* Create resource */
  dev->spawn_op = i_resource_spawn (self, RES_DEVICE, -1, dev->name_str, DEVICE_MODULE, fsroot, self->hierarchy->cust->name_str, (i_entity *) dev, l_device_res_restart_spawncb, entaddr_str);
  free (fsroot);
  if (!dev->spawn_op)
  { i_printf (1, "l_device_res_restart failed to create resource for device %s", dev->name_str); free (entaddr_str); return -1; }
  static_restart_queue++;

  return 0;
}

int l_device_res_restart_spawncb (i_resource *self, i_resource_address *addr, void *passdata)
{
  char *entaddr_str = passdata;
  i_device *dev;
  i_entity_address *entaddr;

  /* Convert entity address */
  entaddr = i_entity_address_struct (entaddr_str);
  free (entaddr_str);
  if (!entaddr)
  { i_printf (1, "l_device_res_restart_spawncb failed to convert entaddr_str to entity address struct"); return -1; }

  /* Retrieve device */
  dev = (i_device *) i_entity_local_get (self, entaddr);
  i_entity_address_free (entaddr);

  /* Process message */
  static_restart_queue--;
  if (dev && addr)
  {
    /* Spawned successfully */
    /* NULLify spawnop */
    dev->spawn_op = NULL;

    /* Set address */
    dev->resaddr = i_resource_address_duplicate (addr);

    /* Log */
    i_printf (1, "l_device_res_restart_spawncb device resource %s online at %s:%s:%i:%i:%s",
      dev->name_str, addr->plexus, addr->node, addr->type, addr->ident_int, addr->ident_str);
    
    /* Enable XML Sync */
//    i_entity_xmlsync_enable (self, ENTITY(dev), dev->refresh_interval, NULL);
  }
  else if (dev && !addr)
  {
    /* Spawn Failed -- Retry */
    i_printf (0, "l_device_res_restart_spawncb device resource %s failed to spawn -- retrying", dev->name_str);
    dev->spawn_op = NULL;
    l_device_res_restart (self, dev);
  }
  else
  {
    /* No device exists, destroy resource immediately */
    i_resource_destroy (self, addr, NULL, NULL);
  }

  return 0;
}

/* 
 * Resource Destroy 
 */

int l_device_res_destroy (i_resource *self, i_device *dev)
{
  /* Destroy existing resource */
  if (dev->resaddr)
  {
    i_resource_destroy_cbdata *data;
    
    data = i_resource_destroy (self, dev->resaddr, NULL, NULL);
    if (!data)
    { 
      i_printf (1, "l_device_res_destroy failed to destroy resource %s:%s:%i:%i:%s", 
        dev->resaddr->plexus, dev->resaddr->node, dev->resaddr->type, dev->resaddr->ident_int, dev->resaddr->ident_str);
    }

    i_resource_address_free (dev->resaddr);
    dev->resaddr = NULL;
  }

  return 0;
}


