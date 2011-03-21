#include <stdlib.h>

#include <induction.h>

#include "hwregistry.h"

static int static_enabled = 0;
static i_hashtable *static_vendor_table = NULL;
static i_hashtable *static_device_table = NULL;
static i_hashtable *static_site_table = NULL;
static i_msgproc_handler *static_register_handler = NULL;
static i_msgproc_handler *static_deregister_handler = NULL;

/* Enable/Disable */

int l_hwregistry_enable (i_resource *self)
{
  int num;
  PGconn *pgconn;
  PGresult *result;

  if (static_enabled == 1)
  { i_printf (1, "l_hwregistry_enable warning, l_hwregistry already enabled"); return 0; }

  static_enabled = 1;

  /* Check the SQL data tables */

  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_hwregistry_enable failed to connect to postgres database"); l_hwregistry_enable (self); return -1; }

  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'hwregistry_items' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* hwregistry_items table not in database */
    result = PQexec (pgconn, "CREATE TABLE hwregistry_items (site_id varchar, device_id varchar, type integer, vendor varchar, product varchar, hw_rev_str varchar, hw_rev_int integer, fw_rev_str varchar, fw_rev_int integer, location varchar, reg_time integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_hwregistry_enable failed to create hwregistry_items table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  i_pg_close (pgconn);  

  /* Create hashtables */
  
  static_vendor_table = i_hashtable_create (DEFAULT_VENDOR_TABLE_SIZE);
  if (!static_vendor_table)
  { i_printf (1, "l_hwregistry_enable failed to create static_vendor_table"); l_hwregistry_disable (self); return -1; }
  i_hashtable_set_destructor (static_vendor_table, i_hashtable_free);

  static_device_table = i_hashtable_create (DEFAULT_DEVICE_TABLE_SIZE); 
  if (!static_device_table)
  { i_printf (1, "l_hwregistry_enable failed to create static_device_table"); l_hwregistry_disable (self); return -1; }

  static_site_table = i_hashtable_create (DEFAULT_SITE_TABLE_SIZE);
  if (!static_site_table)
  { i_printf (1, "l_hwregistry_enable failed to create static_site_table"); l_hwregistry_disable (self); return -1; }

  static_register_handler = i_msgproc_handler_add (self, self->core_socket, MSG_HW_REGISTER, l_hwregistry_handler_register, NULL);
  if (!static_register_handler)
  { i_printf (1, "l_hwregistry_enable failed to add MSG_HW_REGISTER handler"); l_hwregistry_disable (self); return -1; }

  static_deregister_handler = i_msgproc_handler_add (self, self->core_socket, MSG_HW_DEREGISTER, l_hwregistry_handler_deregister, NULL);
  if (!static_deregister_handler)
  { i_printf (1, "l_hwregistry_enable failed to add MSG_HW_DEREGISTER handler"); l_hwregistry_disable (self); return -1; }

  num = i_navform_link_add (self, "hwregistry_main", "Hardware Registry", 20, AUTH_LEVEL_CLIENT, NULL, "hwregistry_main", NULL, 0);
  if (num != 0)
  { i_printf (1, "l_hwregistry_enable warning, failed to add link to navform"); }
      
  return 0;
}

int l_hwregistry_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "l_hwregistry_disable warning, l_hwregistry_disable already enabled"); return 0; }

  static_enabled = 0;

  if (static_vendor_table)
  { i_hashtable_free (static_vendor_table); static_vendor_table = NULL; }
  if (static_device_table)
  { i_hashtable_free (static_device_table); static_device_table = NULL; }
  if (static_site_table)
  { i_hashtable_free (static_site_table); static_site_table = NULL; }
  if (static_register_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_HW_REGISTER); static_register_handler = NULL; }
  if (static_deregister_handler)
  { i_msgproc_handler_remove_by_type (self, self->core_socket, MSG_HW_DEREGISTER); static_deregister_handler = NULL; }

  i_navform_link_remove (self, "hwregistry_main");
  
  return 0;
}

/* Pointer fetching */

i_hashtable* l_hwregistry_table_device ()
{ if (static_enabled == 0) return NULL; return static_device_table; }

i_hashtable* l_hwregistry_table_vendor ()
{ if (static_enabled == 0) return NULL; return static_vendor_table; }

i_hashtable* l_hwregistry_table_site ()
{ if (static_enabled == 0) return NULL; return static_site_table; }

