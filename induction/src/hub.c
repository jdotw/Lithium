#include <stdlib.h>
#include <string.h>

#include "induction.h"

/*
 * Functions and code related to 'hubs'
 *
 * Hubs are logical entities that exist on lithium nodes
 * They perform much the same task as an old Ethernet hub. I.e a resource
 * can create a hub and others attach to it. All devices attached to the hub
 * all recieve the same info sent to the hub.
 *
 * For example, a resource may create a hub for real-time status messages.
 * Clients or other resources needing this realtime info all attach to the hub
 * and hence recieve all the realtime info as it happens 
 *
 * The functions here interact with the hub resource to perform the required
 * functions
 */

static i_list* subscription_list = NULL;

/* General Hub Functions - Create / Send */

int i_hub_create (i_resource *self, char *id_str)
{
  /* Send a create request to the hub controller resource to create
   * a new hub with an id string of id_str 
   *
   * Packet format for create request is as follows 
   *
   * int id_str_len
   * char *id_str
   * int customer_id_len
   * char *customer_id 
   *
   */

  int num;
  int datasize;
  i_message *msg;
  char *data;
  char *dataptr;
  
  if (!self || !id_str) return -1;

  datasize = 2*(sizeof(int));
  if (id_str) datasize += strlen (id_str)+1;
  if (self->customer_id) datasize += strlen (self->customer_id)+1;
    
  data = (char *) malloc (datasize);
  if (!data) 
  {
    i_printf (1, "i_hub_create unable to malloc data (%i bytes)", datasize);
    return -1;
  }
  dataptr = data;

  dataptr = i_data_add_string (data, dataptr, datasize, id_str);
  if (!dataptr)
  { i_printf (1, "i_hub_create failed to add hub id_str to data"); free (data); return -1; }
  
  dataptr = i_data_add_string (data, dataptr, datasize, self->customer_id);
  if (!dataptr)
  { i_printf (1, "i_hub_create failed to add hub customer_id to data"); free (data); return -1; }
 
  msg = i_message_create (MSG_HUB_CREATE, data, datasize);
  free (data);
  if (!msg)
  {
    i_printf (1, "i_hub_create unable to create msg");
    return -1;
  }

  i_hub_address_message (self, msg);
  
  num = i_message_send (self, msg);
  i_message_free (msg);
  if (num != 0)
  {
    i_printf (1, "i_hub_create unable to send msg");
    return -1;
  }

  msg = i_message_recv_type (self, MSG_HUB_CREATE);
  if (!msg)
  {
    i_printf (1, "i_hub_create failed to get response");
    return -1;
  }

  if (msg->type != MSG_OK && msg->type != MSG_HUB_CREATE)
  {
    /* Error occurred */

    switch (msg->type)
    {
      default: i_printf (1, "i_hub_create failed. error message %i returned");
    }

    i_message_free (msg);
    return -1;
  }

  /* Successfully Created */

  i_printf (2, "i_hub_create successfully created hub %s", id_str);
  i_message_free (msg);
  return 0;
}

int i_hub_send_data (i_resource *self, i_resource_address *owner, char *id_str, int type, void *hub_data, int hub_datasize, char *module_name, char *decode_func_str)
{
  int num;
  i_hub_msg *hub_msg;
 
  hub_msg = i_hub_msg_create (self, owner, id_str, type, hub_data, hub_datasize, module_name, decode_func_str);
  if (!hub_msg)
  {
    i_printf (1, "i_hub_send_data unable to create hub_msg");
    return -1;
  }

  num = i_hub_msg_send (self, hub_msg);
  i_hub_free_msg (hub_msg);
  if (num != 0)
  {
    i_printf (1, "i_hub_send_data failed to send hub_msg");
    return -1;
  }

  return 0;
}

int i_hub_send_string (i_resource *self, i_resource_address *owner, char *id_str, char *str)
{
  if (!self || !id_str || !str) return -1;

  return i_hub_send_data (self, owner, id_str, HUB_MSG_STRING, str, strlen(str), NULL, NULL);
}

int i_hub_relay (i_resource *self, i_resource_address *owner, char *id_str, i_hub_msg *hub_msg)
{
  /* Take the given msg and relay it out the specified hub */

  int num;
  i_hub_msg *dup;

  if (!self || !hub_msg || !id_str) return -1;

  dup = i_hub_msg_duplicate (hub_msg);
  if (!dup)
  { i_printf (1, "i_hub_relay failed to create duplicate of hub_msg"); return -1; }

  if (dup->id_str) free (dup->id_str);
  dup->id_str = strdup (id_str);

  if (dup->owner_str) free (dup->owner_str);
  dup->owner_str = i_resource_address_struct_to_string (owner);
  if (!dup->owner_str)
  {
    i_printf (1, "i_hub_relay failed to convert owner to dup->owner_str");
    i_hub_free_msg (dup);
    return -1;
  }

  num = i_hub_msg_send (self, dup);
  i_hub_free_msg (dup);
  if (num != 0)
  {
    i_printf (1, "i_hub_relay failed to send dup msg");
    return -1;
  }

  return 0;
}

/* Hub message functions */

void i_hub_free_msg (i_hub_msg *msg)
{
  if (!msg) return;

  if (msg->data) free (msg->data);

  if (msg->src) i_resource_free_address (msg->src);
  if (msg->owner_str) free (msg->owner_str);
  if (msg->customer_id) free (msg->customer_id);
  if (msg->id_str) free (msg->id_str);

  if (msg->module_name) free (msg->module_name);
  if (msg->decode_func_str) free (msg->decode_func_str);

  free (msg);
}

void i_hub_address_message (i_resource *self, i_message *msg)
{
  i_resource_address *addr;

  addr = i_resource_address_string_to_struct ("::6::");
  if (!addr) { i_printf (1, "i_hub_address_message unable to create dst resource address"); return; }

  i_message_address (msg, RES_ADDR(self), addr);

  i_resource_free_address (addr);
}

char* i_hub_msg_struct_to_data (i_hub_msg *msg, int *datasizeptr)
{
  int datasize;
  char *data;
  char *dataptr;
  char *src_str;

  /* Data format for the hub struct is as follows :
   *
   * int type 
   * 
   * int datasize               (Size of data chunk)
   * void* data                 (Data chunk)
   *
   * int src_str_size
   * char *src_str
   * 
   * int owner_str_size
   * char owner_str
   * int customer_id_size
   * char customer_id
   * int id_str_size
   * char id_str
   * 
   * int module_name_size       (String size incl \0)
   * char module_name           (Null terminated string)
   * int decode_func_str_size   (String size incl \0)
   * char decode_func_str       (Null terminated string)
   */

  if (!datasizeptr) { i_printf (1, "i_hub_msg_struct_to_data called with NULL datasizeptr"); return NULL; }
  memset (datasizeptr, 0, sizeof(int));
  if (!msg || !msg->owner_str || !msg->id_str) return NULL;

  src_str = i_resource_address_struct_to_string (msg->src);
  
  datasize = 8 * sizeof(int);
  if (msg->data) datasize += msg->datasize;
  if (src_str) datasize += strlen (src_str)+1;
  if (msg->owner_str) datasize += strlen (msg->owner_str) + 1;
  if (msg->customer_id) datasize += strlen (msg->customer_id) + 1;
  if (msg->id_str) datasize += strlen (msg->id_str) + 1;
  if (msg->module_name) datasize += strlen(msg->module_name)+1;
  if (msg->decode_func_str) datasize += strlen(msg->decode_func_str)+1;

  data = (char *) malloc (datasize);
  if (!data)
  {
    i_printf (1, "i_hub_msg_struct_to_data unable to malloc data (%i bytes)", datasize);
    return NULL;
  }
  memset (data, 0, datasize);
  dataptr = data;

  dataptr = i_data_add_int (data, dataptr, datasize, &msg->type);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add type int to data"); free (data); return NULL; }

  dataptr = i_data_add_chunk (data, dataptr, datasize, msg->data, msg->datasize);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add data chunk to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, src_str);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add src_str to data"); free (data); return NULL; }
  if (src_str) free (src_str);

  dataptr = i_data_add_string (data, dataptr, datasize, msg->owner_str);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add owner_str to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, msg->customer_id);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add customer_id to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, msg->id_str);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add id_str to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, msg->module_name);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add module_name to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, msg->decode_func_str);
  if (!dataptr) { i_printf (1, "i_hub_msg_struct_to_data failed to add decode_func_str to data"); free (data); return NULL; }

  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_hub_msg* i_hub_msg_data_to_struct (char *data, int datasize)
{
  int offset;
  char *dataptr;
  char *src_str;
  i_hub_msg *hub_msg;

  if (!data || datasize <1) return NULL;

  hub_msg = (i_hub_msg *) malloc (sizeof(i_hub_msg));
  if (!hub_msg)
  {
    i_printf (1, "i_hub_msg_data_to_struct unable to malloc hub_msg");
    return NULL;
  }
  memset (hub_msg, 0, sizeof(hub_msg));

  dataptr = data;

  hub_msg->type = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get type int"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;

  hub_msg->data = i_data_get_chunk (data, dataptr, datasize, &hub_msg->datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get data chunk"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;

  src_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get src_str string"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;
  if (src_str) { hub_msg->src = i_resource_address_string_to_struct (src_str); free (src_str); }

  hub_msg->owner_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get owner_str string"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;

  hub_msg->customer_id = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get customer_id string"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;
        
  hub_msg->id_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get id_str string"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;
        
  hub_msg->module_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get module name string"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;

  hub_msg->decode_func_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_printf (1, "i_hub_msg_data_to_struct unable to get decode func string"); i_hub_free_msg (hub_msg); return NULL; }
  dataptr += offset;

  return hub_msg;
}

i_hub_msg* i_hub_msg_duplicate (i_hub_msg *msg)
{
  /* Duplicates the given msg by doing a struct->data->struct conversion */

  int datasize;
  char *data;
  i_hub_msg *dup;

  data = i_hub_msg_struct_to_data (msg, &datasize);
  if (!data)
  { i_printf (1, "i_hub_msg_duplicate failed to convert msg to data"); return NULL; }

  dup = i_hub_msg_data_to_struct (data, datasize);
  free (data);
  if (!dup)
  { i_printf (1, "i_hub_msg_duplicate failed to convert msg to dup"); return NULL; }

  return dup;
}

i_hub_msg* i_hub_msg_create (i_resource *self, i_resource_address *owner, char *id_str, int type, void *hub_data, int hub_datasize, char *module_name, char *decode_func_str)
{
  /* Create a hub msg */

  int datasize;
  i_hub_msg *hub_msg;
  char *data;

  /* Create the hub message struct */

  hub_msg = (i_hub_msg *) malloc (sizeof(i_hub_msg));
  if (!hub_msg)
  {
    i_printf (1, "i_hub_create_msg unable to malloc hub_msg");
    return NULL;
  }
  memset (hub_msg, 0, sizeof(i_hub_msg));

  /* Fill out the hub msg struct out, all data is malloced/copied to ensure free()ing */

  hub_msg->type = type;

  if (hub_data && hub_datasize > 0)
  {
    hub_msg->data = malloc (hub_datasize);
    if (!hub_msg)
    { i_printf (1, "i_hub_create_msg failed to malloc hub_msg->data (%i bytes)", hub_datasize); i_hub_free_msg (hub_msg); return NULL; }
    memcpy (hub_msg->data, hub_data, hub_datasize);
    hub_msg->datasize = hub_datasize;
  }

  hub_msg->src = i_resource_address_duplicate (RES_ADDR(self));
  if (!hub_msg->src)
  {
    i_printf (1, "i_hub_create_msg failed to create duplicate RES_ADDR(self) to hub_msg->src");
    i_hub_free_msg (hub_msg);
    return NULL;
  } 

  hub_msg->owner_str = i_resource_address_struct_to_string (owner);
  if (!hub_msg->owner_str)
  {
    i_printf (1, "i_hub_create_msg unable to convert owner address struct to string");
    i_hub_free_msg (hub_msg);
    return NULL;
  }

  if (self->customer_id) hub_msg->customer_id = strdup (self->customer_id);
  
  if (id_str) hub_msg->id_str = strdup (id_str);

  if (module_name) hub_msg->module_name = strdup (module_name);
  if (decode_func_str) hub_msg->decode_func_str = strdup (decode_func_str);

  /* Return hub message */

  return hub_msg;
}

int i_hub_msg_send (i_resource *self, i_hub_msg *hub_msg)
{
  int num;
  int datasize;
  char *data;
  i_message *msg;
  
  data = i_hub_msg_struct_to_data (hub_msg, &datasize);
  if (!data || datasize < 1)
  {
    i_printf (1, "i_hub_msg_send_data failed to convert hub_msg to data");
    return -1;
  }

  msg = i_message_create (MSG_HUB_MSG, data, datasize);
  free (data);
  if (!msg)
  {
    i_printf (1, "i_hub_msg_send_data failed to create msg");
    return -1;
  }

  i_hub_address_message (self, msg);

  num = i_message_send (self, msg);
  i_message_free (msg);
  if (num != 0)
  {
    i_printf (1, "i_hub_msg_send failed to send msg %i", num);
    return -1;
  }

  msg = i_message_recv_type (self, MSG_HUB_MSG);
  if (!msg)
  {
    i_printf (1, "i_hub_msg_send failed to recv response");
    return -1;
  }

  if (msg->type != MSG_OK && msg->type != MSG_HUB_MSG)
  {
    i_printf (1, "i_hub_msg_send received error response %i", msg->type);
    i_message_print_error_type (msg->type);
    i_message_free (msg);
    return -1;
  }

  i_message_free (msg);

  i_printf (2, "i_hub_msg_send successfully sent hub message");
  
  return 0;
}

/* 
 * Functions relating to hub descriptions
 */

void i_hub_free_desc (i_hub_desc *desc)
{
  if (!desc) return;

  if (desc->owner_str) free (desc->owner_str);
  if (desc->id_str) free (desc->id_str);
  if (desc->customer_id) free (desc->customer_id);

  free (desc);
}

i_hub_desc* i_hub_desc_data_to_struct (char *data, int datasize)
{
  char *dataptr;
  int offset;
  i_hub_desc *desc;

  desc = (i_hub_desc *) malloc (sizeof(i_hub_desc));
  if (!desc) 
  {
    i_printf (1, "i_hub_desc_data_to_struct unable to malloc data");
    return NULL;
  }

  dataptr = data;
  offset = 0;
  
  desc->owner_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (!desc->owner_str || offset < 1)
  {
    i_printf (1, "i_hub_desc_data_to_struct unable to get owner_str from data");
    i_hub_free_desc (desc);
    return NULL;
  }
  dataptr += offset;

  desc->id_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (!desc->owner_str || offset < 1)
  {
    i_printf (1, "i_hub_desc_data_to_struct unable to get id_str from data");
    i_hub_free_desc (desc);
    return NULL;
  } 
  dataptr += offset;

  desc->customer_id = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  {
    i_printf (1, "i_hub_desc_data_to_struct unable to get customer_id from data");
    i_hub_free_desc (desc);
    return NULL;
  } 
  dataptr += offset;

  return desc;
}

/* 
 * Functions related to hub subscriptions 
 */

void i_hub_free_subscription (void *hub_sub_ptr)
{
  i_hub_subscription *hub_sub = hub_sub_ptr;
  
  if (!hub_sub) return;

  if (hub_sub->owner) i_resource_free_address (hub_sub->owner);
  if (hub_sub->id_str) free (hub_sub->id_str);

  free (hub_sub);
}

i_hub_subscription* i_hub_subscribe (i_resource *self, i_resource_address *owner, char *id_str, int (*hub_callback) (), void *callback_data)
{
  /* Subscrube to the specified hub, and add it to the local subscription
   * list. The callback func will be called and callback_data passed to it
   * when a hub msg is recieved from that hub
   */

  int num;
  i_hub_subscription *hub_sub;
  char *owner_str;
  char *data;
  char *dataptr;
  int datasize;
  i_message *msg;
  i_resource_address hub_addr;

  if (!self || !owner || !id_str || !hub_callback) return NULL;
  
  /* Check if the sub list exists, if not, create it */
  
  if (!subscription_list) 
  {
    subscription_list = i_list_create ();
    if (!subscription_list) 
    {
      i_printf (1, "i_hub_subscribe unable to create subscription list");
      return NULL;
    }
    i_list_set_destructor (subscription_list, i_hub_free_subscription);
  }

  /* Check for duplicate subscription */

  for (i_list_move_head(subscription_list); (hub_sub=i_list_restore(subscription_list))!=NULL; i_list_move_next(subscription_list))
  {
    if (!strcmp(hub_sub->id_str, id_str))
    {
      /* ID matches, now check the owner */
      if ((i_resource_address_compare(hub_sub->owner, owner))==0)
      {
        /* ID and Owner match. Subscription already present */
        return hub_sub;
      }
    }
  }

  /* Create the hubsub struct */
  
  hub_sub = (i_hub_subscription *) malloc (sizeof(i_hub_subscription));
  if (!hub_sub)
  {
    i_printf (1, "i_hub_subscribe unable to malloc hub_sub");
    return NULL;
  }
  memset (hub_sub, 0, sizeof(i_hub_subscription));

  hub_sub->owner = i_resource_address_duplicate (owner);
  if (!hub_sub->owner)
  {
    i_printf (1, "i_hub_subscribe unable to duplicate owner address");
    i_hub_free_subscription (hub_sub);
    return NULL;
  }
  hub_sub->id_str = strdup (id_str);
  hub_sub->hub_callback = hub_callback;
  hub_sub->callback_data = callback_data;

  /* Send the subscription request to the hub controller */

  owner_str = i_resource_address_struct_to_string (owner);
  if (!owner_str)
  {
    i_printf (1, "i_hub_subscribe unable to create owner_str");
    i_hub_free_subscription (hub_sub);
    return NULL;
  }

  datasize = strlen (owner_str) + strlen (id_str) + (2*(sizeof(int))) + 2;
  data = (char *) malloc (datasize);
  if (!data)
  {
    i_printf (1, "i_hub_subscribe unable to malloc data");
    i_hub_free_subscription (hub_sub);
    free (owner_str);
    return NULL;
  }
  memset (data, 0, datasize);
  dataptr = data;
  
  dataptr = i_data_add_string (data, dataptr, datasize, owner_str);
  free (owner_str);
  if (!dataptr) { i_printf (1, "i_hub_subscribe unable to add owner_str to data"); i_hub_free_subscription (hub_sub); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, id_str);
  if (!dataptr) { i_printf (1, "i_hub_subscribe unable to add id_str to data"); i_hub_free_subscription (hub_sub); return NULL; }

  msg = i_message_create (MSG_HUB_SUBSCRIBE, data, datasize);
  free (data);
  if (!msg)
  {
    i_printf (1, "i_hub_subscribe unable to create msg");
    i_hub_free_subscription (hub_sub);
    return NULL;
  }

  i_hub_address_message (self, msg);
  
  num = i_message_send (self, msg);
  i_message_free (msg);
  if (num != 0)
  {
    i_printf (1, "i_hub_subscribe unable to send msg to hub controller");
    i_hub_free_subscription (hub_sub);
    return NULL;
  }
  
  msg = i_message_recv_type (self, MSG_HUB_SUBSCRIBE);
  if (!msg)
  {
    i_printf (1, "i_hub_subscribe unable to recv response from hub controller");
    i_hub_free_subscription (hub_sub);
    return NULL;
  }
  if (msg->type != MSG_HUB_SUBSCRIBE && msg->type != MSG_OK)
  {
    i_printf (1, "i_hub_subscribe recieved error message back from hub controller");
    i_hub_free_subscription (hub_sub);
    i_message_free (msg);
    return NULL;
  }
  
  i_message_free (msg);

  /* Place hubsub struct in the subscriotion list */
  
  i_list_enqueue (subscription_list, hub_sub);

  /* FInished */
  
  i_printf (2, "i_hub_subscribe successfully subscribe to hub %s:%s:%i:%i:%s:%s", 
    owner->plexus, owner->node, owner->type, owner->ident_int, owner->ident_str, id_str);

  return hub_sub;
}

int i_hub_unsubscribe (i_resource *self, i_hub_subscription *hub_sub)
{
  /* Unsubscribe from a hub */

  int num;

  num = i_list_search (subscription_list, hub_sub);
  if (num == 0) 
  {
    /* entry found */

    i_list_delete (subscription_list);
    return 0;
  }

  return -1;
}

int i_hub_unsubcribe_by_owner_id (i_resource *self, i_resource_address *owner, char *id_str)
{
  i_hub_subscription *hub_sub;
  char *owner_addr_str;

  if (!owner || !self || !id_str) return -1;
  
  owner_addr_str = i_resource_address_struct_to_string (owner);
  if (!owner_addr_str)
  {
    i_printf (1, "i_hub_unsubcribe_by_owner_id unable to convert owner address to string");
    return -1;
  }
    
  for (i_list_move_head(subscription_list); (hub_sub=i_list_restore(subscription_list)) != NULL; i_list_move_next(subscription_list))
  {
    char *sub_addr_str;

    sub_addr_str = i_resource_address_struct_to_string (hub_sub->owner);
    if (!sub_addr_str)
    {
      i_printf (1, "i_hub_unsubcribe_by_owner_id unable to convert hub_sub->owner address to string. continuing");
      continue;
    }

    if (!strcmp(sub_addr_str, owner_addr_str))
    {
      i_list_delete (subscription_list);
      free (sub_addr_str);
      free (owner_addr_str);
      return 0;   /* Found and removed */
    }

    free (sub_addr_str);
  }

  free (owner_addr_str);
  return -1; /* Not found */
}

void i_hub_process_broadcast (i_resource *self, i_message *msg, i_form *config_form)
{
  /* Called when a resource receives a hub broadcast message.
   * Here messages are decoded and the subscription list is searched for the
   * relevant subscription. If a subscription is found the callback func will
   * be called for that subscription. If a valid subscription isnt found, an
   * unsubscribe message will be sent back
   */

  i_hub_msg *hub_msg;
  i_hub_subscription *hub_sub;
  char *owner_str;

  if (!self || !msg) return;

  hub_msg = i_hub_msg_data_to_struct (msg->data, msg->datasize);
  if (!hub_msg)
  {
    i_printf (1, "i_hub_process_broadcast unable to convert data to hub_msg struct");
    return;
  }

  for (i_list_move_head(subscription_list); (hub_sub=i_list_restore(subscription_list)) != NULL; i_list_move_next(subscription_list))
  {
    char *sub_addr_str;

    sub_addr_str = i_resource_address_struct_to_string (hub_sub->owner);
    if (!sub_addr_str)
    {
      i_printf (1, "i_hub_process_broadcast unable to convert hub_sub->owner address to string. continuing");
      continue;
    }

    if (!strcmp(sub_addr_str, hub_msg->owner_str))
    {
      /* Matched, run the callback */
      if (hub_sub->hub_callback) hub_sub->hub_callback (self, hub_msg, config_form, hub_sub->callback_data);
    }
    free (sub_addr_str);
  }

  i_hub_free_msg (hub_msg);
}
