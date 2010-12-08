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

#include "xsanregistry.h"

static int static_enabled = 0;
static i_hashtable *static_device_table = NULL;
static i_hashtable *static_obj_table = NULL;
static i_msgproc_handler *static_register_handler = NULL;
static i_msgproc_handler *static_deregister_handler = NULL;

/* Enable / Disable */

int l_xsanregistry_enable (i_resource *self)
{
  if (static_enabled == 1)
  { i_printf (1, "l_xsanregistry_enable warning, xsanregistry already enabled"); return 0; }

  static_enabled = 1;
  
  static_device_table = i_hashtable_create (DEFAULT_DEVICE_TABLE_SIZE);
  if (!static_device_table)
  { i_printf (1, "l_xsanregistry_enable failed to create static_device_table"); l_xsanregistry_disable (self); return -1; }

  static_obj_table = i_hashtable_create (DEFAULT_OBJECT_TABLE_SIZE);
  if (!static_obj_table)
  { i_printf (1, "l_xsanregistry_enable failed to create static_obj_table"); l_xsanregistry_disable (self); return -1; }

  static_register_handler = i_msgproc_handler_add (self, self->core_socket, MSG_XSAN_REGISTER, l_xsanregistry_handler_register, NULL);
  if (!static_register_handler)
  { i_printf (1, "l_xsanregistry_enable failed to add handler for MSG_XSAN_REGISTER"); l_xsanregistry_disable (self); return -1; }

  static_deregister_handler = i_msgproc_handler_add (self, self->core_socket, MSG_XSAN_DEREGISTER, l_xsanregistry_handler_deregister, NULL);
  if (!static_deregister_handler)
  { i_printf (1, "l_xsanregistry_enable failed to add handler for MSG_XSAN_DEREGISTER"); l_xsanregistry_disable (self); return -1; }

  return 0;
}

int l_xsanregistry_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "l_xsanregistry_disable warning, xsanregistry already disabled"); return 0; }

  static_enabled = 0;

  if (static_device_table)
  { i_hashtable_free (static_device_table); static_device_table = NULL; }
  if (static_obj_table)
  { i_hashtable_free (static_obj_table); static_obj_table = NULL; }
  if (static_register_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_XSAN_REGISTER); static_register_handler = NULL; }
  if (static_deregister_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_XSAN_DEREGISTER); static_deregister_handler = NULL; }

  return 0;
}

/* Pointer fetching */

i_hashtable* l_xsanregistry_table_device ()
{ 
  if (static_enabled == 0) return NULL;
  return static_device_table; 
}

i_hashtable* l_xsanregistry_table_object ()
{ 
  if (static_enabled == 0) return NULL;
  return static_obj_table; 
}


