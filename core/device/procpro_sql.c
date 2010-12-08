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

#include "procpro.h"

/* Process profile SQL Operations */

int l_procpro_sql_insert (i_resource *self, l_procpro *procpro)
{
  char *query;

  /* Connect to SQL */
  PGconn *pgconn;
  PGresult *pgres;
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_procpro_sql_insert failed to connect to postgres database"); return -1; }

  /* Create query */
  char *desc_esc = i_postgres_escape (procpro->desc_str);
  char *match_esc = i_postgres_escape (procpro->match_str);
  char *argmatch_esc = i_postgres_escape (procpro->argmatch_str);
  asprintf (&query, "INSERT INTO procpro (descr, match, site, device, argmatch) VALUES ('%s', '%s', '%s', '%s', '%s');",
    desc_esc, match_esc, self->hierarchy->site->name_str, self->hierarchy->dev->name_str, argmatch_esc);
  free (desc_esc);
  free (match_esc);
  free (argmatch_esc);

  /* Exec query */
  pgres = PQexec (pgconn, query);
  free (query);
  if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
  {
    /* Failed */
    i_printf (1, "l_procpro_sql_insert failed to insert procpro record"); 
    return -1;
  }
  PQclear (pgres);

  /* Get ID number just allocated */ 
  pgres = PQexec (pgconn, "SELECT lastval()");
  if (pgres && PQresultStatus(pgres) == PGRES_TUPLES_OK && PQntuples(pgres) > 0)
  {
    char *curid_str = PQgetvalue (pgres, 0, 0);
    if (curid_str)
    { procpro->id = strtoul (curid_str, NULL, 10); }
    else
    { i_printf (1, "l_procpro_sql_insert nextval did not return a valid string"); return -1; }
  } 
  else
  { i_printf (1, "l_procpro_sql_insert warning, failed to retrieve last procpro ID from SQL (%s)", PQresultErrorMessage (pgres)); return -1; }   
  PQclear (pgres);
  i_pg_close (pgconn);

  i_printf (1, "l_procpro_sql_insert successfully added procpro %li", procpro->id);
  
  return 0;
}

int l_procpro_sql_update (i_resource *self, l_procpro *procpro)
{
  int num;

  /* Open conn */
  i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_procpro_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  char *query;
  char *desc_esc = i_postgres_escape (procpro->desc_str);
  char *match_esc = i_postgres_escape (procpro->match_str);
  char *argmatch_esc = i_postgres_escape (procpro->argmatch_str);
  asprintf (&query, "UPDATE procpro SET descr='%s', match='%s', argmatch='%s' WHERE id='%li'",
    desc_esc, match_esc, argmatch_esc, procpro->id);
  free (desc_esc);
  free (match_esc);
  free (argmatch_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_procpro_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_procpro_sql_update failed to execute UPDATE for procpro %li", procpro->id); return -1; }

  return 0;
}

int l_procpro_sql_delete (i_resource *self, int procpro_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_procpro_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM procpro WHERE id='%i'", procpro_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_procpro_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_procpro_sql_update failed to execute DELETE for procpro %li", procpro_id); return -1; }

  return 0;
}

i_callback* l_procpro_sql_load_list (i_resource *self, char *id_str, char *site_name, char *dev_name, int (*cbfunc) (), void *passdata)
{
  /* Load list of all procpros */
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
  { i_printf (1, "l_procpro_sql_load_list failed to open SQL database connection"); return NULL; }

  /* Create query */
  if (id_str)
  {
    asprintf (&query, "SELECT id, descr, match, argmatch, site, device FROM procpro WHERE id='%s'", id_str);
  }
  else if (site_name && !dev_name)
  {
    asprintf (&query, "SELECT id, descr, match, argmatch, site, device FROM procpro WHERE site='%s'", id_str);
  }
  else if (site_name && dev_name)
  {
    asprintf (&query, "SELECT id, descr, match, argmatch, site, device FROM procpro WHERE site='%s' AND device='%s'",
      site_name, dev_name);
  }
  else
  {
    asprintf (&query, "SELECT id, descr, match, argmatch, site, device FROM procpro");
  }

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_procpro_sql_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_procpro_sql_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_procpro_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
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
    i_printf (1, "l_procpro_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
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
    char *match_str;
    char *argmatch_str;
    char *site_str;
    char *dev_str;
    l_procpro *procpro;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    descr_str = PQgetvalue (res, row, 1);
    match_str = PQgetvalue (res, row, 2);
    argmatch_str = PQgetvalue (res, row, 3);
    site_str = PQgetvalue (res, row, 4);
    dev_str = PQgetvalue (res, row, 5);

    /* Create procpro */
    procpro = l_procpro_create ();
    if (id_str) procpro->id = atol (id_str);
    if (descr_str) procpro->desc_str = strdup (descr_str);
    if (match_str) procpro->match_str = strdup (match_str);
    if (argmatch_str) procpro->argmatch_str = strdup (argmatch_str);

    /* Enqueue */
    i_list_enqueue (list, procpro);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num == 0)
    {
      /* Returning 0 causes the list to be freed */
      i_list_set_destructor (list, l_procpro_free);
      i_list_free (list);
    }
  }
  else
  { 
    i_list_set_destructor (list, l_procpro_free);
    i_list_free (list); 
  }

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* SQL Callback */

int l_procpro_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_procpro_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

