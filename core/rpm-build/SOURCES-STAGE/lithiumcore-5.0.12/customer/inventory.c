#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>

#include "inventory.h"

static int static_enabled = 0;
static i_hashtable *static_vendor_table = NULL;
static i_hashtable *static_device_table = NULL;
static i_hashtable *static_site_table = NULL;
static i_msgproc_handler *static_register_handler = NULL;
static i_msgproc_handler *static_deregister_handler = NULL;

/* Enable/Disable */

int l_inventory_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  if (static_enabled == 1)
  { i_printf (1, "l_inventory_enable warning, l_inventory already enabled"); return 0; }

  static_enabled = 1;

  /* Connect to SQL db */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_inventory_enable failed to connect to postgres database"); l_inventory_enable (self); return -1; }

  /* Check inventory_item_history SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'inventory_item_history' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* inventory_item_history table not in database */
    result = PQexec (pgconn, "CREATE TABLE inventory_item_history (site_id varchar, device_id varchar, type integer, vendor varchar, product varchar, version varchar, serial varchar, platform varchar, feature varchar, v_major integer, v_minor integer, v_micro integer, v_patch integer, v_build integer, flags integer, reg_time integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_inventory_enable failed to create inventory_items table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Check inventory_item_history SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'inventory_static_items' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* inventory_static_items table not in database */
    result = PQexec (pgconn, "CREATE TABLE inventory_static_items (site_id varchar, device_id varchar, type integer, vendor varchar, product varchar, version varchar, serial varchar, platform varchar, feature varchar, v_major integer, v_minor integer, v_micro integer, v_patch integer, v_build integer, flags integer, reg_time integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_inventory_enable failed to create inventory_items table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Close SQL conn */
  i_pg_close (pgconn);  

  /* Create hashtables */
  
  static_vendor_table = i_hashtable_create (DEFAULT_VENDOR_TABLE_SIZE);
  if (!static_vendor_table)
  { i_printf (1, "l_inventory_enable failed to create static_vendor_table"); l_inventory_disable (self); return -1; }
  i_hashtable_set_destructor (static_vendor_table, i_hashtable_free);

  static_device_table = i_hashtable_create (DEFAULT_DEVICE_TABLE_SIZE); 
  if (!static_device_table)
  { i_printf (1, "l_inventory_enable failed to create static_device_table"); l_inventory_disable (self); return -1; }

  static_site_table = i_hashtable_create (DEFAULT_SITE_TABLE_SIZE);
  if (!static_site_table)
  { i_printf (1, "l_inventory_enable failed to create static_site_table"); l_inventory_disable (self); return -1; }

  /* Add message handlers */

  static_register_handler = i_msgproc_handler_add (self, self->core_socket, MSG_INV_REGISTER, l_inventory_handler_register, NULL);
  if (!static_register_handler)
  { i_printf (1, "l_inventory_enable failed to add MSG_INV_REGISTER handler"); l_inventory_disable (self); return -1; }

  static_deregister_handler = i_msgproc_handler_add (self, self->core_socket, MSG_INV_DEREGISTER, l_inventory_handler_deregister, NULL);
  if (!static_deregister_handler)
  { i_printf (1, "l_inventory_enable failed to add MSG_INV_DEREGISTER handler"); l_inventory_disable (self); return -1; }

  return 0;
}

int l_inventory_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "l_inventory_disable warning, l_inventory_disable already enabled"); return 0; }

  static_enabled = 0;

  if (static_vendor_table)
  { i_hashtable_free (static_vendor_table); static_vendor_table = NULL; }
  if (static_device_table)
  { i_hashtable_free (static_device_table); static_device_table = NULL; }
  if (static_site_table)
  { i_hashtable_free (static_site_table); static_site_table = NULL; }
  if (static_register_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_INV_REGISTER); static_register_handler = NULL; }
  if (static_deregister_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_INV_DEREGISTER); static_deregister_handler = NULL; }

  return 0;
}

/* Pointer fetching */

i_hashtable* l_inventory_table_device ()
{ if (static_enabled == 0) return NULL; return static_device_table; }

i_hashtable* l_inventory_table_vendor ()
{ if (static_enabled == 0) return NULL; return static_vendor_table; }

i_hashtable* l_inventory_table_site ()
{ if (static_enabled == 0) return NULL; return static_site_table; }

