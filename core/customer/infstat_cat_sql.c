#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/callback.h>

#include "infstat.h"

static int static_issueload_count = 0;

/* SQL Init */

int l_infstat_cat_sql_init (i_resource *self)
{
  PGresult *res;
  PGconn *pgconn;
      
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_infstat_cat_sql_init failed to connect to postgresql database"); return -1; }
      
  i_pg_begin (pgconn);
  res = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'infstat_categories' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || (PQntuples(res)) < 1)
  {   
    /* infstat_categories table doesnt exist */
    res = PQexec (pgconn, "CREATE TABLE infstat_categories (name varchar, descr varchar)");
    if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
    { i_printf (1, "l_infstat_cat_sql_init failed to create infstat_categories table (%s)", PQresultErrorMessage (res)); PQclear(res); return -1; }
  }   PQclear(res);
    
  i_pg_close (pgconn);
      
  return 0;
}

/* SQL Load */

int l_infstat_cat_sql_load (i_resource *self, i_list *list, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata)
{
  int num;
  i_pg_async_conn *conn;
  i_callback *callback;

  /* Reset issueload counter */
  static_issueload_count = 0;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_infstat_cat_sql_load failed to connect to SQL database"); return -1; }

  /* Create callback struct */
  callback = i_callback_create ();
  if (!callback)
  { i_printf (1, "l_infstat_cat_sql_load failed to create callback struct"); i_pg_async_conn_close (conn); return -1; }
  callback->func = callback_func;
  callback->data = list;
  callback->passdata = passdata;

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, "SELECT * FROM infstat_categories", 0, l_infstat_cat_sql_load_sqlcb, callback);
  if (num != 0)
  { 
    i_printf (1, "l_infstat_cat_sql_load failed to execute SELECT command"); 
    i_pg_async_conn_close (conn); 
    i_callback_free (callback);
    return -1; 
  }

  /* Finished */

  return 0;
}

int l_infstat_cat_sql_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Callback for the result of SELECTing category records
   *
   * This callback is only called once 
   */

  int num;
  int y;
  int row_count;
  i_list *list;
  i_callback *callback = passdata;

  list = (i_list *) callback->data;
  
  if (!result)
  { 
    i_printf (1, "l_infstat_cat_sql_load_sqlcb query failed, recvd NULL result during operation code %i", operation);
    callback->func (self, 0, callback->passdata);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  if (PQresultStatus(result) != PGRES_TUPLES_OK)
  {
    i_printf (1, "l_infstat_cat_sql_load_sqlcb query failed, at operation %i (%s)", operation, PQresultErrorMessage (result));
    callback->func (self, 0, callback->passdata);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  /* Interpret result */
  row_count = PQntuples (result);
  for (y=0; y < row_count; y++)
  {
    int x;
    int field_count;
    l_infstat_cat *cat;

    cat = l_infstat_cat_create ();
    if (!cat)
    { i_printf (1, "l_infstat_cat_sql_load_sqlcb failed to create cat struct at row %i, continuing", y); continue; }

    field_count = PQnfields (result);
    for (x=0; x < field_count; x++)
    {
      char *field_name;
      char *value;

      field_name = PQfname (result, x);
      if (!field_name)
      { i_printf (1, "l_infstat_cat_sql_load_sqlcb failed to get field name for i=%i, continuing", x); continue; }

      value = PQgetvalue (result, y, x);

      if (!strcmp(field_name, "name"))
      { if (value) cat->name_str = strdup (value); }
      if (!strcmp(field_name, "descr"))
      { if (value) cat->desc_str = strdup (value); }
    }

    /* Enqueue the cat */
    num = i_list_enqueue (list, cat);
    if (num != 0)
    { i_printf (1, "l_infstat_cat_sql_load_sqlcb failed to enqueue cat %s at row %i, continuing", cat->name_str, x); continue; }

    /* Load the issues */
    num = l_infstat_issue_sql_load (self, cat, l_infstat_cat_sql_load_issuecb, callback);
    if (num != 0)
    { i_printf (1, "l_infstat_cat_sql_load_sqlcb failed to call l_infstat_issue_sql_load for cat %s, continuing", cat->name_str); continue; }
    else
    { static_issueload_count++; }
  }

  /* Cleanup 
   *
   * Free this connection. The callback etc should be called
   * from the l_infstat_cat_sql_load_issuecb once all the
   * issues have been loaded and/or an error occurs
   */
  
  i_pg_async_conn_close (conn);

  if (static_issueload_count == 0)
  { 
    /* No issue loads were requested, 
     * the category list must be empty and hence
     * l_instat_cat_sql_load_issuecb callback wont be called
     */
    callback->func (self, 1, callback->passdata);
    i_callback_free (callback);
  }

  return 0;
}

int l_infstat_cat_sql_load_issuecb (i_resource *self, int result, void *passdata)
{
  /* Callback for when the issue have been loaded for a particular category */
  i_callback *callback = passdata;

  static_issueload_count--;

  if (static_issueload_count == 0)
  {
    /* All issues have been loaded */
    callback->func (self, 1, callback->passdata);
    i_callback_free (callback);
  }

  return 0;
}

/* SQL Add */

int l_infstat_cat_sql_add (i_resource *self, l_infstat_cat *cat)
{
  int num;
  char *name_esc;
  char *desc_esc;
  char *command;
  char *action_str;
  i_pg_async_conn *conn;
 
  /* Connect */

  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_infstat_cat_sql_add failed to connect to SQL database to add category %s", cat->name_str); return -1; }

  /* Form command string */
  name_esc = i_postgres_escape (cat->name_str);
  desc_esc = i_postgres_escape (cat->desc_str);
  asprintf (&command, "INSERT INTO infstat_categories (name, descr) VALUES ('%s', '%s')", name_esc, desc_esc);
  if (name_esc) free (name_esc);
  if (desc_esc) free (desc_esc);

  /* Execute command */
  asprintf (&action_str, "Add category %s", cat->name_str);
  num = i_pg_async_query_exec (self, conn, command, 0, l_infstat_cat_sql_callback, action_str);
  free (command);  
  if (num != 0)
  { 
    i_printf (1, "l_infstat_cat_sql_add failed to execute INSERT command for category %s", cat->name_str);
    i_pg_async_conn_close (conn);
    free (action_str);
    return -1;
  }   
  
  return 0;
}

/* SQL Update */

int l_infstat_cat_sql_update (i_resource *self, l_infstat_cat *cat)
{
  int num;
  char *name_esc;
  char *desc_esc;
  char *command;
  char *action_str;
  i_pg_async_conn *conn;

  /* Connect */

  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_infstat_cat_sql_update failed to connect to SQL database to update category %s", cat->name_str); return -1; }

  /* Form command string */
  name_esc = i_postgres_escape (cat->name_str);
  desc_esc = i_postgres_escape (cat->desc_str);
  asprintf (&command, "UPDATE infstat_categories SET descr='%s' WHERE name='%s'", desc_esc, name_esc);
  if (name_esc) free (name_esc);
  if (desc_esc) free (desc_esc);

  /* Execute command */
  asprintf (&action_str, "Edit category %s", cat->name_str);
  num = i_pg_async_query_exec (self, conn, command, 0, l_infstat_cat_sql_callback, action_str);
  free (command);
  if (num != 0)
  {
    i_printf (1, "l_infstat_cat_sql_update failed to execute UPDATE command for category %s", cat->name_str);
    i_pg_async_conn_close (conn);
    free (action_str);
    return -1;
  }

  return 0;
}

/* SQL Delete */

int l_infstat_cat_sql_delete (i_resource *self, l_infstat_cat *cat)
{
  int num;
  char *name_esc;
  char *command;
  char *action_str;
  i_pg_async_conn *conn;

  /* Connect */

  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_infstat_cat_sql_delete failed to connect to SQL database to delete category %s", cat->name_str); return -1; }

  /* Form command string */
  name_esc = i_postgres_escape (cat->name_str);
  asprintf (&command, "DELETE FROM infstat_categories WHERE name='%s'", name_esc);
  if (name_esc) free (name_esc);

  /* Execute command */
  asprintf (&action_str, "Remove category %s", cat->name_str);
  num = i_pg_async_query_exec (self, conn, command, 0, l_infstat_cat_sql_callback, action_str);
  free (command);
  if (num != 0)
  {
    i_printf (1, "l_infstat_cat_sql_edit failed to execute DELETE command for category %s", cat->name_str);
    i_pg_async_conn_close (conn);
    free (action_str);
    return -1;
  }

  return 0;
}

/* SQL Callback */

int l_infstat_cat_sql_callback (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{ 
  char *action_str = passdata;
  
  if (!result)
  {
    i_printf (1, "l_infstat_cat_sql_callback query failed, recvd NULL result during operation code %i (%s)", operation, action_str);       
    i_pg_async_conn_close (conn);
    free (action_str);
    return -1;
  }

  if (PQresultStatus(result) != PGRES_COMMAND_OK)
  { i_printf (1, "l_infstat_cat_sql_callback query failed, at operation %i - %s (%s)", operation, action_str, PQresultErrorMessage (result)); }

  i_pg_async_conn_close (conn);
  free (action_str);

  return 0;
}

