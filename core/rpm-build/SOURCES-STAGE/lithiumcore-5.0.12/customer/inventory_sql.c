#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>
#include <induction/callback.h>

#include "inventory.h"

/* SQL Retrieval */

int l_inventory_sql_get_list (i_resource *self, char *table_str, char *site_id, char *device_id, char *vendor_str, char *product_str, char *version_str, char *serial_str,int (*callback_func) (i_resource *self, i_list *list, void *passdata), void *passdata)
{
  /* Retrieve of inventory_items matching
   * the criteria provided from the specified table_str
   * A NULL value for either
   * vendor_str, product_str or version_str results
   * in a wildcard match for that criteria
   *
   * The list is passed to the callback function when it
   * has been retrieved. Any items left in the list after
   * the callback has been run will be freed
   *
   */

  int num;
  char *command;
  char *tempstr;
  char *site_esc;
  char *device_esc;
  char *esc_str;
  i_pg_async_conn *conn;
  i_callback *callback;

  site_esc = i_postgres_escape (site_id);
  device_esc = i_postgres_escape (device_id);
  asprintf (&command, "SELECT * FROM %s WHERE site_id='%s' AND device_id='%s'", table_str, site_esc, device_esc);
  free (site_esc);
  free (device_esc);

  if (vendor_str)
  { 
    esc_str = i_postgres_escape (vendor_str);
    tempstr = command; 
    asprintf (&command, "%s AND vendor='%s'", tempstr, esc_str); 
    free (tempstr); 
    free (esc_str);
  }
  if (product_str)
  { 
    esc_str = i_postgres_escape (product_str);
    tempstr = command; 
    asprintf (&command, "%s AND product='%s'", tempstr, esc_str); 
    free (tempstr); 
    free (esc_str);
  }
  if (version_str)
  { 
    esc_str = i_postgres_escape (version_str);
    tempstr = command; 
    asprintf (&command, "%s AND version='%s'", tempstr, esc_str); 
    free (tempstr); 
    free (esc_str);
  }
  if (serial_str)
  { 
    esc_str = i_postgres_escape (serial_str);
    tempstr = command; 
    asprintf (&command, "%s AND serial='%s'", tempstr, esc_str); 
    free (tempstr); 
    free (esc_str);
  }

  tempstr = command;
  asprintf (&command, "%s ORDER BY reg_time DESC", tempstr);
  free (tempstr);

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_inventory_sql_get_list failed to connect to SQL database"); return -1; }

  /* Create callback struct */
  callback = i_callback_create ();
  if (!callback)
  { i_printf (1, "l_inventory_sql_get_list failed to create callback struct"); free (command); i_pg_async_conn_close (conn); return -1; }
  callback->func = callback_func;
  callback->data = passdata;

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, command, 0, l_inventory_sql_get_list_sqlcb, callback);
  free (command);
  if (num != 0)
  {
    i_printf (1, "l_inventory_sql_get_list failed to execute SELECT command from %s for %s at %s (vnd:%s prd:%s ver:%s)", table_str, site_id, device_id, vendor_str, product_str, version_str);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  /* Finished */

  return 0;
}

int l_inventory_sql_get_list_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  int num;
  int y;
  int row_count;
  i_list *list;
  i_callback *callback = passdata;
  
  if (!result)
  {
    i_printf (1, "l_inventory_sql_get_list_sqlcb query failed, recvd NULL result during operation code %i", operation);
    callback->func (self, NULL, callback->data);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  if (PQresultStatus(result) != PGRES_TUPLES_OK)
  {
    i_printf (1, "l_inventory_sql_get_list_sqlcb query failed, at operation %i (%s)", operation, PQresultErrorMessage (result)); 
    callback->func (self, NULL, callback->data);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  /* Create list */

  list = i_list_create ();
  if (!list)
  { 
    i_printf (1, "l_inventory_sql_get_list_sqlcb faild to create list");
    callback->func (self, NULL, callback->data);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  /* Create entries for result */

  row_count = PQntuples (result);
  for (y=0; y < row_count; y++)
  {
    int x;
    int field_count;
    i_inventory_item *item;

    item = i_inventory_item_create ();
    if (!item)
    { i_printf (1, "l_inventory_sql_get_list_sqlcb failed to create item at row %i, continuing", y); continue; }
    
    field_count = PQnfields (result);
    for (x=0; x < field_count; x++)
    {
      char *field_name;
      char *value;

      field_name = PQfname (result, x);
      if (!field_name)
      { i_printf (1, "l_inventory_sql_get_list_sqlcb failed to get field name for i=%i, continuing", x); continue; }

      value = PQgetvalue (result, y, x);

      if (!strcmp(field_name, "type"))
      { if (value) item->type = atoi (value); }
      if (!strcmp(field_name, "vendor"))
      { if (value) item->vendor_str = strdup (value); }
      if (!strcmp(field_name, "product"))
      { if (value) item->product_str = strdup (value); }
      if (!strcmp(field_name, "version"))
      { if (value) item->version_str = strdup (value); }
      if (!strcmp(field_name, "serial"))
      { if (value) item->serial_str = strdup (value); }
      if (!strcmp(field_name, "platform"))
      { if (value) item->platform_str = strdup (value); }
      if (!strcmp(field_name, "feature"))
      { if (value) item->feature_str = strdup (value); }
      if (!strcmp(field_name, "v_major"))
      { if (value) item->v_major = atoi (value); }
      if (!strcmp(field_name, "v_minor"))
      { if (value) item->v_minor = atoi (value); }
      if (!strcmp(field_name, "v_micro"))
      { if (value) item->v_micro = atoi (value); }
      if (!strcmp(field_name, "v_patch"))
      { if (value) item->v_patch = atoi (value); }
      if (!strcmp(field_name, "v_build"))
      { if (value) item->v_build = atoi (value); }
      if (!strcmp(field_name, "flags"))
      { if (value) item->flags = atoi (value); }
      if (!strcmp(field_name, "reg_time"))
      { if (value) item->register_time.tv_sec = atol (value); }
    }

    /* Enqueue the item */

    num = i_list_enqueue (list, item);
    if (num != 0)
    { i_printf (1, "l_inventory_sql_get_list_sqlcb failed to enqueue item at row %i, continuing", y); continue; }
  }

  /* Call the callback */

  callback->func (self, list, callback->data);
    
  /* Cleanup */

  i_list_set_destructor (list, i_inventory_item_free);
  i_list_free (list);
  i_callback_free (callback);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Insert */

int l_inventory_sql_insert (i_resource *self, char *table_str, i_inventory_item *item)
{
  /* Insert the given item into the
   * specified SQL table_str
   */

  int num;
  char *command;
  char *site_esc;
  char *device_esc;
  char *vendor_esc;
  char *product_esc;
  char *version_esc;
  char *serial_esc;
  char *platform_esc;
  char *feature_esc;
  i_pg_async_conn *conn;

  /* Connect */

  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_inventory_sql_insert failed to connect to SQL database for %s at %s (vdr:%s prd:%s ver:%s) [table=%s]", item->dev->site->name_str, item->dev->name_str, item->vendor_str, item->product_str, item->version_str, table_str); return -1; }

  site_esc = i_postgres_escape (item->dev->site->name_str);
  device_esc = i_postgres_escape (item->dev->name_str);
  vendor_esc = i_postgres_escape (item->vendor_str);
  product_esc = i_postgres_escape (item->product_str);
  version_esc = i_postgres_escape (item->version_str);
  serial_esc = i_postgres_escape (item->serial_str);
  platform_esc = i_postgres_escape (item->platform_str);
  feature_esc = i_postgres_escape (item->feature_str);
  asprintf (&command, "INSERT INTO %s (site_id, device_id, type, vendor, product, version, serial, platform, feature, v_major, v_minor, v_micro, v_patch, v_build, flags, reg_time) VALUES ('%s', '%s', '%i', '%s', '%s', '%s', '%s', '%s', '%s', '%i', '%i', '%i', '%i', '%i', '%i', '%li')", 
    table_str, site_esc, device_esc, item->type, vendor_esc, product_esc, version_esc, serial_esc, platform_esc, feature_esc, item->v_major, item->v_minor, item->v_micro, item->v_patch, item->v_build, item->flags, item->register_time.tv_sec);
  free (site_esc);
  free (device_esc);
  free (vendor_esc);
  free (product_esc);
  free (version_esc);
  free (serial_esc);
  free (platform_esc);
  free (feature_esc);

  /* Execute command */

  num = i_pg_async_query_exec (self, conn, command, 0, l_inventory_sql_callback, conn);
  free (command);
  if (num != 0)
  {
    i_printf (1, "l_inventory_sql_insert failed to execute INSERT command into %s for %s at %s (vdr:%s prd:%s ver:%s)", table_str, item->dev->site->name_str, item->dev->name_str, item->vendor_str, item->product_str, item->version_str);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Finished */

  return 0;
}

/* SQL Delete */

int l_inventory_sql_delete (i_resource *self, char *table_str, char *site_id, char *device_id, char *vendor_str, char *product_str, char *version_str, char *serial_str)
{ 
  /* Delete inventory items matching
   * the criteria provided from the specified table_str
   * A NULL value for either
   * vendor_str, product_str or version_str results
   * in a wildcard match for that criteria
   */
  
  int num;
  char *command;
  char *tempstr;
  char *site_esc;
  char *device_esc;
  char *esc_str;
  i_pg_async_conn *conn;
  site_esc = i_postgres_escape (site_id);
  device_esc = i_postgres_escape (device_id);
  asprintf (&command, "DELETE FROM %s WHERE site_id='%s' AND device_id='%s'", table_str, site_esc, device_esc);
  free (site_esc);
  free (device_esc);
  
  if (vendor_str)
  { 
    esc_str = i_postgres_escape (vendor_str);
    tempstr = command;
    asprintf (&command, "%s AND vendor='%s'", tempstr, esc_str); 
    free (tempstr);
    free (esc_str);
  }
  if (product_str)
  { 
    esc_str = i_postgres_escape (product_str);
    tempstr = command;
    asprintf (&command, "%s AND product='%s'", tempstr, esc_str);
    free (tempstr);
    free (esc_str);
  } 
  if (version_str) 
  { 
    esc_str = i_postgres_escape (version_str);
    tempstr = command;
    asprintf (&command, "%s AND version='%s'", tempstr, esc_str);
    free (tempstr);
    free (esc_str);
  }
  if (serial_str)
  { 
    esc_str = i_postgres_escape (serial_str);
    tempstr = command;
    asprintf (&command, "%s AND serial='%s'", tempstr, esc_str);
    free (tempstr);
    free (esc_str);
  }

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_inventory_sql_delete failed to connect to SQL database"); return -1; }

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, command, 0, l_inventory_sql_callback, conn);
  free (command);
  if (num != 0)
  {
    i_printf (1, "l_inventory_sql_delete failed to execute DELETE command from %s for %s at %s (vnd:%s prd:%s ver:%s)", table_str, site_id, device_id, vendor_str, product_str, version_str);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Finished */

  return 0;
}

/* Generic SQL Callback */

int l_inventory_sql_callback (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  if (!result)
  {
    i_printf (1, "l_inventory_sql_callback query failed, recvd NULL result during operation code %i", operation);
    i_pg_async_conn_close (conn);
    return -1;
  }

  if (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK)
  { i_printf (1, "l_inventory_sql_callback query failed, at operation %i (%s)", operation, PQresultErrorMessage (result)); }

  i_pg_async_conn_close (conn);

  return 0;
}

