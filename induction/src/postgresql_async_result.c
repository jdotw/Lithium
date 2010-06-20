#include <stdlib.h>

#include "induction.h"
#include "postgresql.h"
#include "list.h"

int i_pg_async_result_process (i_resource *self, i_pg_async_conn *conn, PGresult *result)
{
  /* Process a result recvd from the db backend */

  int num;
  i_pg_async_query_op *op;

  if (!self || !conn)
  { i_printf (1, "i_pg_async_result_process called with NULL self and/or conn"); return -1; }

  if (!conn->query)
  { i_printf (1, "i_pg_async_result_process called with NULL conn->query"); return -1; }

  /* Run the callback */

  if (conn->query->callback_func)
  { num = conn->query->callback_func (self, conn, conn->query->current_op, result, conn->query->passdata); }
  else
  { num = 0; }

  i_pg_async_resptime_record (self, conn);
  gettimeofday (&conn->query->last_result, NULL);

  /* Find next command */

  i_list_move_head (conn->query->operations);
  op = i_list_restore (conn->query->operations);
  if (op)
  {
    /* Next operation */
    conn->query->current_op = op->code;
    num = PQsendQuery (conn->pgconn, op->command);
    if (num != 1)
    { 
      /* Failed to send query */
      i_printf (1, "i_pg_async_result_process failed to send query (%s)", PQerrorMessage (conn->pgconn));
      if (conn->query->callback_func)
      { conn->query->callback_func (self, conn, conn->query->current_op, NULL, conn->query->passdata); }
      i_pg_async_query_free (conn->query);
      conn->query = NULL;
      return 0;
    }
    i_list_delete (conn->query->operations);      /* Remove the operation from the list */
  }
  else
  {
    /* Query is finished */
    i_pg_async_query_free (conn->query);
    conn->query = NULL;

    /* Find next query (if it exists) */

    for (i_list_move_head(conn->query_q); (conn->query=i_list_restore(conn->query_q))!=NULL; i_list_move_next(conn->query_q))
    {
      /* We only really want to first query_q entry normally, 
       * but just incase there's a problem executing that
       * query, the for loop is setup for retrying
       */

      /* Dequeue the query */
      i_list_delete (conn->query_q);
      if (conn->query_q && conn->query_q->size < 1)
      { i_list_free (conn->query_q); conn->query_q = NULL; }

      /* Find first op */
      i_list_move_head (conn->query->operations);
      op = i_list_restore (conn->query->operations);
      if (op)
      {
        /* Send the first query operation */
        conn->query->current_op = op->code;
        num = PQsendQuery (conn->pgconn, op->command);
        if (num != 1)
        { i_printf (2, "i_pg_async_result_process failed send first operation (op code %i -- '%s')", op->code, op->command); i_pg_async_query_free (conn->query); continue; }
        i_list_delete (conn->query->operations);  /* Dequeue */
      }
      else
      {
        /* Nothing to send! */
        i_printf (1, "i_pg_async_query_exec found no operations to perform");
        i_pg_async_query_free (conn->query);
        continue;
      }

      break;      /* First query has been successfully run, exit the for */
    }
  }

  return 0;
}
