#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/callback.h>
#include <induction/postgresql.h>

#include "case.h"

/*
 * Case System -- Log Entry SQL Functions
 */

/* Insert */

int l_case_logentry_sql_insert (i_resource *self, long caseid, l_case_logentry *log)
{
  int num;
  char *entry_esc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_logentry_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  entry_esc = i_postgres_escape (log->entry_str);
  asprintf (&query, "INSERT INTO case_logentries (caseid, case_state, type, tstamp, timespent_sec, author, entry) VALUES ('%li', '%i', '%u', '%li', '%li', '%s', '%s');",
    caseid, CASE_STATE_OPEN, log->type, log->tstamp.tv_sec, log->timespent_sec, log->author_str, entry_esc);
  free (entry_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_logentry_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_logentry_sql_insert failed to execute INSERT for case %li", caseid); return -1; }

  return 0;
}

/* SQL Callback */

int l_case_logentry_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_case_logentry_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Load */

i_callback* l_case_logentry_sql_list (i_resource *self, long caseid, int (*cbfunc) (), void *passdata)
{
  /* Retrieve log entries for specified caseid */
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
  { i_printf (1, "l_case_logentry_sql_list failed to open SQL database conn"); return NULL; }

  /* Query */
  asprintf (&query, "SELECT id, caseid, case_state, type, tstamp, timespent_sec, author, entry FROM case_logentries WHERE caseid='%li' ORDER BY tstamp DESC", caseid);
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_logentry_sql_list_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_logentry_sql_list failed to execute SELECT for case %li", caseid); i_callback_free (cb); return NULL; }

  return cb;
}

int l_case_logentry_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
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
    i_printf (1, "l_case_logentry_sql_list_cb failed to execute query"); 
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
    char *caseid_str;
    char *casestate_str;
    char *type_str;
    char *tstamp_str;
    char *tspent_str;
    char *author_str;
    char *entry_str;
    l_case_logentry *log;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    caseid_str = PQgetvalue (res, row, 1);
    casestate_str = PQgetvalue (res, row, 2);
    type_str = PQgetvalue (res, row, 3);
    tstamp_str = PQgetvalue (res, row, 4);
    tspent_str = PQgetvalue (res, row, 5);
    author_str = PQgetvalue (res, row, 6);
    entry_str = PQgetvalue (res, row, 7);

    /* Create log entry */
    log = l_case_logentry_create ();
    log->id = atol (id_str);
    log->caseid = atol (caseid_str);
    log->case_state = atoi (casestate_str);
    log->type = atoi (type_str);
    log->tstamp.tv_sec = atol (tstamp_str);
    log->timespent_sec = atol (tspent_str);
    log->author_str = strdup (author_str);
    log->entry_str = strdup (entry_str);

    /* Enqueue */
    i_list_enqueue (list, log);
  }

  /* Run callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0) 
    {
      i_list_set_destructor (list, l_case_logentry_free);
      i_list_free (list);
    }
  }
  else 
  {
    i_list_set_destructor (list, l_case_logentry_free);
    i_list_free (list);
  }

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Case State */

int l_case_logentry_sql_update_casestate (i_resource *self, long caseid, unsigned short state)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_logentry_sql_update_casestate failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "UPDATE case_logentries SET case_state='%i' WHERE caseid='%li'", state, caseid);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_logentry_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_logentry_sql_update_casestate failed to execute INSERT for case %li", caseid); return -1; }

  return 0;

}
