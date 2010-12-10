#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "postgresql.h"
#include "list.h"

/* Struct Manipulation */

i_pg_async_query* i_pg_async_query_create ()
{
  i_pg_async_query *query;

  query = (i_pg_async_query *) malloc (sizeof(i_pg_async_query));
  if (!query)
  { i_printf (1, "i_pg_async_query_create failed to malloc query struct"); return NULL; }
  memset (query, 0, sizeof(i_pg_async_query));

  query->operations = i_list_create ();
  if (!query->operations) 
  { i_pg_async_query_free (query); return NULL; }
  i_list_set_destructor (query->operations, i_pg_async_query_op_free);

  return query;
}

void i_pg_async_query_free (void *queryptr)
{
  i_pg_async_query *query = queryptr;

  if (!query) return;
  
  if (query->operations) 
  { i_list_free (query->operations); }

  free (query);
}

i_pg_async_query_op* i_pg_async_query_op_create ()
{
  i_pg_async_query_op *op;

  op = (i_pg_async_query_op *) malloc (sizeof(i_pg_async_query_op));
  if (!op)
  { i_printf (1, "i_pg_async_query_op_create failed to malloc op struct"); return NULL; }
  memset (op, 0, sizeof(i_pg_async_query_op));

  return op;
}

void i_pg_async_query_op_free (void *opptr)
{
  i_pg_async_query_op *op = opptr;

  if (!op) return;

  if (op->command) free (op->command);

  free (op);
}

/* Query Execution */

int i_pg_async_query_exec (i_resource *self, i_pg_async_conn *conn, char *query_str, int flags, int (*callback_func) (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata), void *passdata)
{
  /* Execute a query */

  int num;
  i_pg_async_query *query;
  i_pg_async_query_op *op;

  if (!self || !conn) return -1;

  query = i_pg_async_query_create ();
  if (!query)
  { i_printf (1, "i_pg_async_query_exec failed to create query struct"); return -1; }
  i_list_move_head (query->operations);

  query->callback_func = callback_func;
  query->passdata = passdata;
  gettimeofday (&query->start, NULL);
  gettimeofday (&query->last_result, NULL);

  if (query)
  {
    op = i_pg_async_query_op_create ();
    if (!op)
    { i_printf (1, "i_pg_async_query_exec failed to create query_op struct"); i_pg_async_query_free (query); return -1; }
    op->code = PG_FLAG_QUERY;
    op->command = strdup (query_str);
    num = i_list_enqueue (query->operations, op);
    if (num != 0)
    { i_printf (1, "i_pg_async_query_exec failed to enqueue query operation"); i_pg_async_query_free (query); return -1; }
  }

  if (!conn->query)
  {
    /* No query in progress, kick it off */
    i_list_move_head (query->operations);
    op = i_list_restore (query->operations);
    if (op)
    {
      /* Send the first query operation */
      query->current_op = op->code;
      num = PQsendQuery (conn->pgconn, op->command);
      if (num != 1)
      { i_printf (1, "i_pg_async_query_exec failed send first operation (op code %i)", op->code); i_pg_async_query_free (query); return -1; }
      i_list_delete (query->operations);  /* Dequeue */
    }
    else
    {
      /* Nothing to send! */
      i_printf (1, "i_pg_async_query_exec found no operations to perform");
      i_pg_async_query_free (query);
      return -1;
    }

    conn->query = query;
  }
  else
  {
    /* Query in progress, queue this one */
    if (conn->query_q && conn->query_q->size == PGASYNC_QUERY_MAXQ)
    { i_printf (1, "i_pg_async_query_exec failed to enqueue query, maximum query_q length (%i) reached", PGASYNC_QUERY_MAXQ); i_pg_async_query_free (query); return -1; }

    if (!conn->query_q)
    { 
      conn->query_q = i_list_create ();
      if (!conn->query_q)
      { i_printf (1, "i_pg_async_query_exec failed to create conn->query_q"); i_pg_async_query_free (query); return -1; }
    }

    num = i_list_enqueue (conn->query_q, query);
    if (num != 0)
    { i_printf (1, "i_pg_async_query_exec failed to enqueue query"); i_pg_async_query_free (query); return -1; }
  }
  
  return 0;
}
