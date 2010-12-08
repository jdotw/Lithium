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
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "document.h"

/* Document SQL Operations */

int l_document_sql_insert (i_resource *self, l_document *doc)
{
  int num;
  char *type_esc;
  char *desc_esc;
  char *editor_esc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_document_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  type_esc = i_postgres_escape (doc->type_str);
  desc_esc = i_postgres_escape (doc->desc_str);
  editor_esc = i_postgres_escape (doc->editor_str);
  asprintf (&query, "INSERT INTO documents (type, descr, state, editor, version) VALUES ('%s', '%s', '%i', '%s', '%li');",
    doc->type_str, doc->desc_str, doc->state, doc->editor_str, doc->version);
  free (type_esc);
  free (desc_esc);
  free (editor_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_document_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_document_sql_insert failed to execute INSERT for new doc"); return -1; }

  /* Set id */
  doc->id = l_document_id_assign ();

  return 0;
}

int l_document_sql_update (i_resource *self, l_document *doc)
{
  int num;
  char *query;
  char *desc_esc;
  char *editor_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_document_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  desc_esc = i_postgres_escape (doc->desc_str);
  editor_esc = i_postgres_escape (doc->editor_str);
  asprintf (&query, "UPDATE documents SET descr='%s', state='%i', editor='%s', version='%li' WHERE id='%i'",
    desc_esc, doc->state, doc->editor_str, doc->version, doc->id);
  free (desc_esc);
  free (editor_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_document_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_document_sql_update failed to execute UPDATE for doc %i", doc->id); return -1; }

  return 0;
}

int l_document_sql_update_editor (i_resource *self, int doc_id, int state, char *editor_str)
{
  int num;
  char *query;
  char *editor_esc = NULL;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_document_sql_update_editor failed to open SQL db connection"); return -1; }

  /* Create query */
  if (editor_str)
  { 
    char *esc_str = i_postgres_escape (editor_str); 
    asprintf (&editor_esc, "'%s'", esc_str);
    free (esc_str);
  }
  else
  { 
    editor_esc = strdup ("NULL");
  }
  asprintf (&query, "UPDATE documents SET state='%i', editor=%s WHERE id='%i'", state, editor_esc, doc_id);
  free (editor_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_document_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_document_sql_update_editor failed to execute UPDATE for doc %li", doc_id); return -1; }

  return 0;
}

int l_document_sql_delete (i_resource *self, int doc_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_document_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM documents WHERE id='%i'", doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_document_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_document_sql_update failed to execute DELETE for doc %i", doc_id); return -1; }

  return 0;
}

i_callback* l_document_sql_load_list (i_resource *self, char *type_str, int doc_id, int (*cbfunc) (), void *passdata)
{
  /* Load list of all documents */
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
  { i_printf (1, "l_document_sql_load_list failed to open SQL database connection"); return NULL; }

  /* Create query */
  if (doc_id > 0)
  { asprintf (&query, "SELECT id, type, descr, state, editor, version  FROM documents WHERE id='%i'", doc_id); }
  else if (type_str)
  { asprintf (&query, "SELECT id, type, descr, state, editor, version  FROM documents WHERE type='%s'", type_str); }
  else
  { asprintf (&query, "SELECT id, type, descr, state, editor, version  FROM documents"); }

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_document_sql_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_document_sql_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_document_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
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
    i_printf (1, "l_docment_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_document_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *type_str;
    char *descr_str;
    char *state_str;
    char *editor_str;
    char *version_str;
    l_document *doc;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    type_str = PQgetvalue (res, row, 1);
    descr_str = PQgetvalue (res, row, 2);
    state_str = PQgetvalue (res, row, 3);
    editor_str = PQgetvalue (res, row, 4);
    version_str = PQgetvalue (res, row, 5);

    /* Create document */
    doc = l_document_create ();
    if (id_str) doc->id = atoi (id_str);
    if (type_str) doc->type_str = strdup (type_str);
    if (descr_str) doc->desc_str = strdup (descr_str);
    if (state_str) doc->state = atoi (state_str);
    if (editor_str) doc->editor_str = strdup (editor_str);
    if (version_str) doc->version = atol (version_str);

    /* Enqueue */
    i_list_enqueue (list, doc);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num == 0)
    {
      /* Returning 0 causes the list to be freed */
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

/* SQL Callback */

int l_document_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_document_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

