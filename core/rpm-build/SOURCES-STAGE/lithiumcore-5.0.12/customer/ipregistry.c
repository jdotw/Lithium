#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/ip.h>
#include <induction/message.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/metric.h>
#include <induction/object.h>
#include <induction/ipregistry.h>

#include "ipregistry.h"

static int static_enabled = 0;
static i_hashtable *static_ip_table = NULL;               /* Keyed by IP address -- contains l_ipregistry_entry structs */
static i_hashtable *static_network_table = NULL;          /* Keyed by network/mask string -- contains l_ipregistry_network structs */
static i_hashtable *static_device_table = NULL;           /* Keyed by device address -- contains lists of l_ipregistry_entry structs */
static i_msgproc_handler *static_register_handler = NULL;
static i_msgproc_handler *static_setnetmask_handler = NULL;
static i_msgproc_handler *static_setiface_handler = NULL;
static i_msgproc_handler *static_deregister_handler = NULL;

/* Enable / Disable */

int l_ipregistry_enable (i_resource *self)
{
  if (static_enabled == 1)
  { i_printf (1, "l_ipregistry_enable warning, ipregistry already enabled"); return 0; }

  static_enabled = 1;
  
  static_ip_table = i_hashtable_create (DEFAULT_IP_TABLE_SIZE);
  if (!static_ip_table)
  { i_printf (1, "l_ipregistry_enable failed to create static_ip_table"); l_ipregistry_disable (self); return -1; }

  static_network_table = i_hashtable_create (DEFAULT_NETWORK_TABLE_SIZE);
  if (!static_network_table)
  { i_printf (1, "l_ipregistry_enable failed to create static_network_table"); l_ipregistry_disable (self); return -1; }

  static_device_table = i_hashtable_create (DEFAULT_DEVICE_TABLE_SIZE);
  if (!static_device_table)
  { i_printf (1, "l_ipregistry_enable failed to create static_device_table"); l_ipregistry_disable (self); return -1; }
  i_hashtable_set_destructor (static_device_table, i_list_free);

  static_register_handler = i_msgproc_handler_add (self, self->core_socket, MSG_IP_REGISTER, l_ipregistry_handler_register, NULL);
  if (!static_register_handler)
  { i_printf (1, "l_ipregistry_enable failed to add handler for MSG_IP_REGISTER"); l_ipregistry_disable (self); return -1; }

  static_setnetmask_handler = i_msgproc_handler_add (self, self->core_socket, MSG_IP_SETNETMASK, l_ipregistry_handler_setnetmask, NULL);
  if (!static_setnetmask_handler)
  { i_printf (1, "l_ipregistry_enable failed to add handler for MSG_IP_SETNETMASK"); l_ipregistry_disable (self); return -1; }

  static_setiface_handler = i_msgproc_handler_add (self, self->core_socket, MSG_IP_SETIFACE, l_ipregistry_handler_setiface, NULL);
  if (!static_setiface_handler)
  { i_printf (1, "l_ipregistry_enable failed to add handler for MSG_IP_SETIFACE"); l_ipregistry_disable (self); return -1; }

  static_deregister_handler = i_msgproc_handler_add (self, self->core_socket, MSG_IP_DEREGISTER, l_ipregistry_handler_deregister, NULL);
  if (!static_deregister_handler)
  { i_printf (1, "l_ipregistry_enable failed to add handler for MSG_IP_DEREGISTER"); l_ipregistry_disable (self); return -1; }

  return 0;
}

int l_ipregistry_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "l_ipregistry_disable warning, ipregistry already disabled"); return 0; }

  static_enabled = 0;

  if (static_ip_table)
  { i_hashtable_free (static_ip_table); static_ip_table = NULL; }
  if (static_network_table)
  { i_hashtable_free (static_network_table); static_network_table = NULL; }
  if (static_device_table)
  { i_hashtable_free (static_device_table); static_device_table = NULL; }
  if (static_register_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_IP_REGISTER); static_register_handler = NULL; }
  if (static_setnetmask_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_IP_SETNETMASK); static_setnetmask_handler = NULL; }
  if (static_setiface_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_IP_SETIFACE); static_setiface_handler = NULL; }
  if (static_deregister_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_IP_DEREGISTER); static_deregister_handler = NULL; }

  return 0;
}

/* Pointer fetching */

i_hashtable* l_ipregistry_table_ip ()
{ 
  if (static_enabled == 0) return NULL; 
  return static_ip_table;
}

i_hashtable* l_ipregistry_table_network ()
{ 
  if (static_enabled == 0) return NULL; 
  return static_network_table;
}

i_hashtable* l_ipregistry_table_device ()
{ 
  if (static_enabled == 0) return NULL;
  return static_device_table; 
}


