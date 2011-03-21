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

#include "xsanregistry.h"

int l_xsanregistry_handler_register (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */

  int num;
  int offset;
  char *objentdesc_data;
  int objentdesc_datasize;
  char *deventdesc_data;
  int deventdesc_datasize;
  char *dataptr;
  l_xsanregistry_entry *entry;

  if (!msg) return 0;
  dataptr = msg->data;

  /* Create entry */
  entry = l_xsanregistry_entry_create ();
  if (!entry)
  { i_printf (1, "l_xsanregistry_handler_register failed to create entry struct"); return 0; }

  /* Get Volume Descriptor Data */
  objentdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &objentdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_xsanregistry_handler_register failed to get objentdesc_data from msg->data"); return 0; }
  dataptr += offset;
  
  /* Get Device Descriptor Data */
  deventdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &deventdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_xsanregistry_handler_register failed to get deventdesc_data from msg->data"); return 0; }
  dataptr += offset;
  
  /* Get type */
  entry->devtype = i_data_get_int (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_xsanregistry_handler_register failed to get type from msg->data"); return 0; }
  dataptr += offset;

  /* Convert data to entity descriptor struct */
  entry->devent = i_entity_descriptor_struct (deventdesc_data, deventdesc_datasize);
  free (deventdesc_data);
  if (!entry->devent)
  { i_printf (1, "l_xsanregistry_handler_register failed to convert deventdesc_data to entry->devent struct"); l_xsanregistry_entry_free (entry); return 0; }
    
  /* Check for an object */
  if (objentdesc_datasize)
  {
    /* Convert data to entity descriptor struct */
    entry->objent = i_entity_descriptor_struct (objentdesc_data, objentdesc_datasize);
    free (objentdesc_data);
    if (!entry->objent)
    { i_printf (1, "l_xsanregistry_handler_register failed to convert objentdesc_data to entry->objent struct"); l_xsanregistry_entry_free (entry); return 0; }
  }
 
  /* Device Address */
  entry->devaddr = i_resource_address_duplicate (msg->src);
  if (!entry->devaddr)
  { i_printf (1, "l_xsanregistry_handler_register failed to duplicate msg->src entry->devaddr struct"); l_xsanregistry_entry_free (entry); return 0; }

  /* Announce registration */
  if (entry->objent)
  {
    i_printf (2, "l_xsanregistry_handler_register recvd Xsan VOLUME registration of %s registered to device %s at site %s", 
      entry->objent->desc_str, entry->objent->dev_name, entry->objent->site_name);
  }
  else
  {
    i_printf (2, "l_xsanregistry_handler_register recvd Xsan COMPUTER registration of type %i registered to device %s at site %s", 
      entry->devtype, entry->devent->name_str, entry->devent->site_name);
  }

  /* Set registration time */
  gettimeofday (&entry->register_time, NULL);

  /* Add to registry */
  num = l_xsanregistry_add (self, entry);
  if (num != 0)
  { 
    /* if num is -1 an error occurred, if num = 1 the entry was ignored.
     * either way, it needs to be freed as it didnt go into
     * the table
     */
    l_xsanregistry_entry_free (entry);
    if (num == -1)
    { i_printf (1, "l_xsanregistry_handler_register failed to call l_xsanregistry_add to add recvd IP registry entry"); }
  }

  return 0;
}

int l_xsanregistry_handler_deregister (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* ALWAYS RETURN 0 to keep the handler alive */
  int num;
  int offset;
  int objentdesc_datasize;
  char *objentdesc_data;
  char *devaddr_str;
  char *dataptr;
  i_entity_descriptor *objentdesc;
  i_resource_address *devaddr;

  dataptr = msg->data;

  /* Get Device Address Data */
  devaddr_str = i_data_get_string (msg->data, dataptr, msg->datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_xsanregistry_handler_deregister failed to get devaddr_str from msg->data"); return 0; }
  dataptr += offset;

  /* Convert address data to struct */
  devaddr = i_resource_address_string_to_struct (devaddr_str);
  free (devaddr_str);
  if (!devaddr)
  { i_printf (1, "l_xsanregistry_handler_deregister failed to convert devaddr_str to devaddr struct"); return 0; }

  /* Get IP Entity Descriptor data */
  objentdesc_data = i_data_get_chunk (msg->data, dataptr, msg->datasize, &objentdesc_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "l_xsanregistry_handler_deregister failed to get objentdesc_data from msg->data"); return 0; }
  dataptr += offset;

  /* Convert IP Entity descriptor if present */
  if (objentdesc_data)
  {
    objentdesc = i_entity_descriptor_struct (objentdesc_data, objentdesc_datasize);
    free (objentdesc_data);
    if (!objentdesc)
    { i_printf (1, "l_xsanregistry_hanadler_deregister failed to convert objentdesc_data to objentdesc struct"); i_resource_address_free (devaddr); return 0; }
  }
  else
  { objentdesc = NULL; }
  
  /* Announce deregistration from registry */
  if (objentdesc)
  { i_printf (2, "l_xsanregistry_handler_deregister recvd IP deregistration for %s registered to device %s at %s", objentdesc->desc_str, objentdesc->dev_name, objentdesc->site_name); }
  else
  { i_printf (2, "l_xsanregistry_handler_deregister recvd IP deregistration for all IPs registered to device %s", devaddr->ident_str); }

  /* Remove from registry */
  num = l_xsanregistry_remove (self, devaddr, objentdesc);
  if (objentdesc) i_entity_descriptor_free (objentdesc);
  i_resource_address_free (devaddr);
  if (num != 0)
  { i_printf (1, "l_xsanregistry_handler_deregister failed to remove IP entry"); return 0; }

  return 0;
}
