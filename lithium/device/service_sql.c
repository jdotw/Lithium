#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/callback.h>
#include <induction/message.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/incident.h>
#include <induction/postgresql.h>

#include "service.h"

/* Action SQL Operations */

int l_service_sql_insert (i_resource *self, l_service *service)
{
  char *desc_esc;
  char *script_esc;
  char *query;

  /* Connect to SQL */
  PGconn *pgconn;
  PGresult *pgres;
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_service_sql_insert failed to connect to postgres database"); return -1; }

  /* Create query */
  desc_esc = i_postgres_escape (service->desc_str);
  script_esc = i_postgres_escape (service->script_file);
  asprintf (&query, "INSERT INTO services (descr, enabled, script, site, device) VALUES ('%s', '%i', '%s', '%s', '%s');",
    service->desc_str, service->enabled, script_esc, 
    self->hierarchy->site->name_str, self->hierarchy->dev->name_str);
  free (desc_esc);
  free (script_esc);

  /* Exec query */
  pgres = PQexec (pgconn, query);
  free (query);
  if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
  {
    /* Failed */
    i_printf (1, "l_service_sql_insert failed to insert service record"); 
    return -1;
  }
  PQclear (pgres);

  /* Get ID number just allocated */ 
  pgres = PQexec (pgconn, "SELECT lastval()");
  if (pgres && PQresultStatus(pgres) == PGRES_TUPLES_OK && PQntuples(pgres) > 0)
  {
    char *curid_str = PQgetvalue (pgres, 0, 0);
    if (curid_str)
    { service->id = strtoul (curid_str, NULL, 10); }
    else
    { i_printf (1, "l_service_sql_insert nextval did not return a valid string"); return -1; }
  } 
  else
  { i_printf (1, "l_service_sql_insert warning, failed to retrieve last service ID from SQL (%s)", PQresultErrorMessage (pgres)); return -1; }   
  PQclear (pgres);
  i_pg_close (pgconn);

  i_printf (1, "l_service_sql_insert successfully added service %li", service->id);
  
  return 0;
}

int l_service_sql_update (i_resource *self, l_service *service)
{
  int num;
  char *query;
  char *script_esc;
  char *desc_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  desc_esc = i_postgres_escape (service->desc_str);
  script_esc = i_postgres_escape (service->script_file);
  asprintf (&query, "UPDATE services SET descr='%s', enabled='%i', script='%s' WHERE id='%li'",
    desc_esc, service->enabled, script_esc, service->id);
  free (desc_esc);
  free (script_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_service_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_service_sql_update failed to execute UPDATE for service %li", service->id); return -1; }

  return 0;
}

int l_service_sql_delete (i_resource *self, int service_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM services WHERE id='%li'", service_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_service_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_service_sql_update failed to execute DELETE for service %li", service_id); return -1; }

  return 0;
}

i_callback* l_service_sql_load_list (i_resource *self, char *id_str, char *site_name, char *dev_name, int (*cbfunc) (), void *passdata)
{
  /* Load list of all services */
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_sql_load_list failed to open SQL database connection"); return NULL; }

  /* Create query */
  if (id_str)
  {
    asprintf (&query, "SELECT id, descr, enabled, site, device, script FROM services WHERE id='%s'", id_str);
  }
  else if (site_name && !dev_name)
  {
    asprintf (&query, "SELECT id, descr, enabled, site, device, script FROM services WHERE site='%s'", id_str);
  }
  else if (site_name && dev_name)
  {
    asprintf (&query, "SELECT id, descr, enabled, site, device, script FROM services WHERE site='%s' AND device='%s'",
      site_name, dev_name);
  }
  else
  {
    asprintf (&query, "SELECT id, descr, enabled, site, device, script FROM services");
  }

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_service_sql_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_service_sql_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_service_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_service_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *descr_str;
    char *enabled_str;
    char *site_str;
    char *dev_str;
    char *script_str;
    l_service *service;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    descr_str = PQgetvalue (res, row, 1);
    enabled_str = PQgetvalue (res, row, 2);
    site_str = PQgetvalue (res, row, 3);
    dev_str = PQgetvalue (res, row, 4);
    script_str = PQgetvalue (res, row, 5);

    /* Create service */
    service = l_service_create ();
    if (id_str) service->id = atol (id_str);
    if (descr_str) service->desc_str = strdup (descr_str);
    if (enabled_str) service->enabled = atoi (enabled_str);
    if (script_str) service->script_file = strdup (script_str);

    /* Enqueue */
    i_list_enqueue (list, service);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0)
    {
      /* Returning !=0 causes the list to be freed */
      i_list_set_destructor (list, l_service_free);
      i_list_free (list);
    }
  }
  else
  { 
    i_list_set_destructor (list, l_service_free);
    i_list_free (list); 
  }

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Config variables */

int l_service_sql_configvar_insert (i_resource *self, long service_id, l_service_configvar *var)
{
  /* INSERT a config variable */
  
  int num;
  char *insert_query = NULL;
  char *name_esc;
  char *value_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_configvar_sql_insert failed to open SQL database connection"); return -1; }

  /* Create queries */
  name_esc = i_postgres_escape (var->name_str);
  if (var->value_str && strlen(var->value_str) > 0)
  {
    char *str = i_postgres_escape (var->value_str);
    asprintf (&value_esc, "'%s'", str); 
  }
  else
  { asprintf (&value_esc, "NULL"); }
  
  asprintf (&insert_query, "INSERT INTO service_configvars (service, name, value) VALUES ('%li', '%s', %s)",
    service_id, name_esc, value_esc);
  free (name_esc);
  free (value_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, insert_query, 0, l_service_sql_cb, NULL);
  free (insert_query);
  if (num != 0)
  { i_printf (1, "l_service_configvar_sql_insert failed to execute query for service %li", service_id); return -1; }

  return 0;
}

int l_service_sql_configvar_delete (i_resource *self, long service_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;
  
  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_configvar_sql_delete failed to open SQL database connection"); return -1; }
  
  /* Create query */
  asprintf (&query, "DELETE FROM service_configvars WHERE service='%li'", service_id);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_service_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_service_configvar_sql_delete failed to execute delete for service %li", service_id); return -1; }

  return 0;
}

i_callback* l_service_sql_configvar_load_list (i_resource *self, long service_id, int (*cbfunc) (), void *passdata)
{
  /* Load list of all services */
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_sql_configvar_load_list failed to open SQL database connection"); return NULL; }

  /* Create query */
  asprintf (&query, "SELECT id, name, value FROM service_configvars WHERE service='%li'", service_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_service_sql_configvar_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_service_sql_configvar_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_service_sql_configvar_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_aciton_sql_configvar_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_service_configvar_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *name_str;
    char *value_str;
    l_service_configvar *var;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    name_str = PQgetvalue (res, row, 1);
    value_str = PQgetvalue (res, row, 2);

    /* Create service */
    var = l_service_configvar_create ();
    if (id_str) var->id = atol (id_str);
    if (name_str) var->name_str = strdup (name_str);
    if (value_str) var->value_str = strdup (value_str);

    /* Enqueue */
    i_list_enqueue (list, var);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0)
    {
      /* Returning !=0 causes the list to be freed */
      i_list_free (list);
    }
  }
  else
  { i_list_free (list); }

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Log Entry */

int l_service_sql_log_insert (i_resource *self, long service_id, char *comment_str)
{
  int num;
  char *query;
  char *comment_esc;
  i_pg_async_conn *conn;
  struct timeval now;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_sql_log_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  gettimeofday (&now, NULL);
  if (comment_str) comment_esc = i_postgres_escape (comment_str);
  else comment_esc = strdup ("");
  asprintf (&query, "INSERT INTO service_history (service, timestamp, comments) VALUES ('%li', '%li', '%s');",
      service_id, now.tv_sec, comment_esc);
  free (comment_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_service_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_service_sql_log_insert failed to execute INSERT for service %li", service_id); return -1; }

  return 0;
}

i_callback* l_service_sql_log_load (i_resource *self, int service_id, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_service_sql_entity_load_list failed to open SQL database connection"); return NULL; }

  /* Query string */
  asprintf (&query, "SELECT id, timestamp, comments FROM service_history WHERE service=%li LIMIT 250", service_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_service_sql_log_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_service_sql_entity_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }
 
  return cb;
}

int l_service_sql_log_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_service_sql_log_load_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_service_log_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *time_str;
    char *comment_str;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    time_str = PQgetvalue (res, row, 1);
    comment_str = PQgetvalue (res, row, 2);

    /* Create log entry */
    l_service_log *log = l_service_log_create ();
    if (id_str) log->id = atol (id_str);
    if (time_str) log->timestamp = atol (time_str);
    if (comment_str) log->comment_str = strdup (comment_str);

    /* Enqueue */
    i_list_enqueue (list, log);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0) i_list_free (list);
  }
  else i_list_free (list);

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* SQL Callback */

int l_service_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_service_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

