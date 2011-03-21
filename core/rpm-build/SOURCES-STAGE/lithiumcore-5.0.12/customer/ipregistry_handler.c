#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/hierarchy.h>
#include <induction/data.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

int l_ipregistry_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */

  int offset;
  int ipentdesc_datasize;
  char *dataptr;
  char *ipentdesc_data;
  i_entity_descriptor *ipent;
  i_resource_address *devaddr;
  struct in_addr ipaddr;

  if (!msg) return 0;
  dataptr = msg->data;

  /*
   * Interpret message 
   */
  
  /* Get IP Entity Descriptor Data */
  ipentdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &ipentdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_ipregistry_handler_register failed to get ipentdesc_data from msg->data"); return 0; }
  dataptr += offset;

  /* Convert data to entity descriptor struct */
  ipent = i_entity_descriptor_struct (ipentdesc_data, ipentdesc_datasize);
  free (ipentdesc_data);
  if (!ipent)
  { i_printf (1, "l_ipregistry_handler_register failed to convert entry->ipent struct"); return 0; }
 
  /* Set IP Address in_addr Struct */
  inet_aton (ipent->desc_str, &ipaddr);

  /* Device Address */
  devaddr = i_resource_address_duplicate (msg->src);
  if (!devaddr)
  { i_printf (1, "l_ipregistry_handler_register failed to duplicate msg->src devaddr struct"); return 0; }

  /*
   * Locate IP entry
   */
  
  l_ipregistry_entry *entry = l_ipregistry_get_ip_str (self, ipent->desc_str);
  if (!entry)
  {
    /* New entry */
    entry = l_ipregistry_add (self, ipent->desc_str, NULL);
    if (!entry)
    { i_printf (1, "l_ipregistry_handler_register failed to create entry for %s", ipent->desc_str); return 0; }
  }

  /* Find/Create device entry */
  l_ipregistry_entry_device *devip = l_ipregistry_get_ip_device (self, entry, devaddr);
  if (!devip)
  {
    /* First time this device is being associated with this IP */
    devip = l_ipregistry_entry_device_create ();
    i_hashtable_key *key = i_hashtable_key_resource (devaddr, entry->device_table->size);
    i_hashtable_put (entry->device_table, key, devip);
    i_hashtable_key_free (key);
  }

  /* Set devip->ipent */
  if (devip->ipent) i_entity_descriptor_free (devip->ipent);
  devip->ipent = i_entity_descriptor_duplicate (ipent);

  /* 
   * Locate device table list 
   */

  i_list *list = l_ipregistry_get_device (self, devaddr);
  if (!list)
  {
    list = i_list_create ();
    i_hashtable *device_table = l_ipregistry_table_device ();
    i_hashtable_key *key = i_hashtable_key_resource (devaddr, device_table->size);
    i_hashtable_put (device_table, key, list);
    i_hashtable_key_free (key);
  }

  /* Enqueue entry */
  if (i_list_search(list, entry) != 0)
  { i_list_enqueue(list, entry); }

  /* Set entry's bound flag */
  entry->bound = 1;

  /* Announce registration */
  i_printf (2, "l_ipregistry_handler_register recvd IP registration of %s registered to device %s at site %s", 
    ipent->desc_str, ipent->dev_name, ipent->site_name);

  /* Cleanup */
  i_entity_descriptor_free (ipent);
  i_resource_address_free (devaddr);

  return 0;
}

int l_ipregistry_handler_setnetmask (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */

  int offset;
  int ipentdesc_datasize;
  char *dataptr;
  char *mask_str;
  char *ipentdesc_data;
  i_entity_descriptor *ipent;

  if (!msg) return 0;
  dataptr = msg->data;

  /*
   * Interpret message 
   */
  
  /* Get IP Entity Descriptor Data */
  ipentdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &ipentdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_ipregistry_handler_setnetmask failed to get ipentdesc_data from msg->data"); return 0; }
  dataptr += offset;
  
  /* Convert data to entity descriptor struct */
  ipent = i_entity_descriptor_struct (ipentdesc_data, ipentdesc_datasize);
  free (ipentdesc_data);
  if (!ipent)
  { i_printf (1, "l_ipregistry_handler_setnetmask failed to convert ipentdesc_data to ipent struct"); return 0; }

  /* Get mask string from data */
  mask_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_ipregistry_handler_setnetmask failed to get mask_str from msg->data"); return 0; }
  dataptr += offset;

  /* 
   * Set netmask 
   */

  l_ipregistry_set_netmask (self, ipent->desc_str, mask_str);
  i_entity_descriptor_free (ipent);
  free (mask_str);

  return 0;
}

int l_ipregistry_handler_setiface (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */

  int offset;
  int ipentdesc_datasize;
  int ifentdesc_datasize;
  char *dataptr;
  char *ipentdesc_data;
  char *ifentdesc_data;
  i_entity_descriptor *ipent;
  i_entity_descriptor *ifent;

  if (!msg) return 0;
  dataptr = msg->data;

  /* 
   * Interpret message
   */

  /* Get IP Entity Descriptor Data */
  ipentdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &ipentdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_ipregistry_handler_setiface failed to get ipentdesc_data from msg->data"); return 0; }
  dataptr += offset;

  /* Convert data to IP entity descriptor struct */
  ipent = i_entity_descriptor_struct (ipentdesc_data, ipentdesc_datasize);
  free (ipentdesc_data);
  if (!ipent)
  { i_printf (1, "l_ipregistry_handler_setiface failed to convert ipentdesc_data to ipent struct"); return 0; }

  /* Get Interface Entity Descriptor Data */
  ifentdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &ifentdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_ipregistry_handler_setiface failed to get ifentdesc_data from msg->data"); return 0; }
  dataptr += offset;

  /* Convert data to interface entity descriptor struct */
  if (ifentdesc_data)
  {
    ifent = i_entity_descriptor_struct (ifentdesc_data, ifentdesc_datasize);
    free (ifentdesc_data);
    if (!ifent)
    { i_printf (1, "l_ipregistry_handler_setiface failed to convert ifentdesc_data to ifent struct"); return 0; }
  }
  else
  { ifent = NULL; }

  /* 
   * Set interface 
   */

  /* Find entry */
  l_ipregistry_entry *entry = l_ipregistry_get_ip_str (self, ipent->desc_str);
  if (!entry)
  { 
    i_printf (1, "l_ipregistry_handler_setiface failed to find entry for %s", ipent->desc_str); 
    i_entity_descriptor_free (ipent); 
    return 0; 
  }
  i_entity_descriptor_free (ipent);

  /* Find device entry */
  i_resource_address *devaddr = i_resource_address_duplicate (msg->src);
  l_ipregistry_entry_device *devip = l_ipregistry_get_ip_device (self, entry, devaddr);
  if (!devip)
  { 
    devip = l_ipregistry_entry_device_create ();
    i_hashtable_key *key = i_hashtable_key_resource (devaddr, entry->device_table->size);
    i_hashtable_put (entry->device_table, key, devip);
    i_hashtable_key_free (key);
  }
  i_resource_address_free (devaddr);
  
  /* Set iface */
  if (devip->iface) 
  { 
    i_entity_descriptor_free (devip->iface);
    devip->iface = NULL;
  }
  if (ifent)
  { devip->iface = i_entity_descriptor_duplicate (ifent); }

  /* Cleanup */
  if (ifent) i_entity_descriptor_free (ifent);

  return 0;
}

int l_ipregistry_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */
  int offset;
  int ipentdesc_datasize;
  char *ipentdesc_data;
  char *devaddr_str;
  char *dataptr;
  i_entity_descriptor *ipentdesc;
  i_resource_address *devaddr;

  dataptr = msg->data;

  /*
   * Interpret message
   */

  /* Get Device Address Data */
  devaddr_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_ipregistry_handler_deregister failed to get devaddr_str from msg->data"); return 0; }
  dataptr += offset;

  /* Convert address data to struct */
  devaddr = i_resource_address_string_to_struct (devaddr_str);
  free (devaddr_str);
  if (!devaddr)
  { i_printf (1, "l_ipregistry_handler_deregister failed to convert devaddr_str to devaddr struct"); return 0; }

  /* Get IP Entity Descriptor data */
  ipentdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &ipentdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_ipregistry_handler_deregister failed to get ipentdesc_data from msg->data"); return 0; }
  dataptr += offset;

  /* Convert IP Entity descriptor if present */
  if (ipentdesc_data)
  {
    ipentdesc = i_entity_descriptor_struct (ipentdesc_data, ipentdesc_datasize);
    free (ipentdesc_data);
    if (!ipentdesc)
    { i_printf (1, "l_ipregistry_hanadler_deregister failed to convert ipentdesc_data to ipentdesc struct"); i_resource_address_free (devaddr); return 0; }
  }
  else
  { ipentdesc = NULL; }

  /*
   * Find list of entryies in device table
   */

  i_hashtable *device_table = l_ipregistry_table_device ();
  i_hashtable_key *key = i_hashtable_key_resource (devaddr, device_table->size);
  i_list *list = i_hashtable_get (device_table, key);
  i_hashtable_key_free (key);
  
  /* Loop through each associated IP entry */
  l_ipregistry_entry *entry;
  for (i_list_move_head(list); (entry=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    /* Skip if a specific IP is given and this doesnt match */
    if (ipentdesc && strcmp(entry->ip_str, ipentdesc->desc_str) != 0) continue;

    /* Remove IP entry's device_table item */
    i_hashtable_key *key = i_hashtable_key_resource (devaddr, entry->device_table->size);
    i_hashtable_remove (entry->device_table, key);  /* Destructor frees */
    i_hashtable_key_free (key);

    /* Check if the entry is still bound */
    if (entry->device_table->inuse == 0)
    { entry->bound = 0; }

    /* Remove from list of entries */
    i_list_delete (list);
  }

  /* Check to see if there's still any entries for the device */
  if (list && list->size == 0)
  {
    /* Remove list of associated entries from device_table */
    key = i_hashtable_key_resource (devaddr, device_table->size);
    i_hashtable_remove (device_table, key);
    i_hashtable_key_free (key);
  }


  
  /* Announce deregistration from registry */
  if (ipentdesc)
  { 
    i_printf (2, "l_ipregistry_handler_deregister recvd IP deregistration for %s registered to device %s at %s", ipentdesc->desc_str, ipentdesc->dev_name, ipentdesc->site_name);
    i_entity_descriptor_free (ipentdesc);
  }
  else
  { i_printf (2, "l_ipregistry_handler_deregister recvd IP deregistration for all IPs registered to device %s", devaddr->ident_str); }

  return 0;
}
