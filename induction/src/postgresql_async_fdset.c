#include <stdlib.h>

#include "induction.h"
#include "postgresql.h"
#include "list.h"

int i_pg_async_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  int highestsockfd = 0;
  i_list *list;
  i_pg_async_conn *conn;

  list = i_pg_async_conn_list ();
  if (!list)
  { return 0; }

  for (i_list_move_head(list); (conn=i_list_restore(list))!=NULL; (i_list_move_next(list)))
  {
    int fd;
    int num;

    if (!conn->pgconn)
    {
      /* Connection has been closed, delete from list */
      i_list_delete (list);
      continue;
    }

    if (conn->query)
    {
      /* A query is in progress */

      fd = PQsocket (conn->pgconn);
      if (fd == -1) 
      { 
        i_printf (1, "i_pg_async_fdset_preprocessor got fd of -1 for connection %p", conn);        
        i_pg_async_result_process (self, conn, NULL);
        continue; 
      }

      /* Flush the buffer */

      num = PQflush (conn->pgconn);
      switch (num)
      {
        case 0: FD_SET (fd, read_fdset);      /* Buffer flushed, wait for response */
                if (fd > highestsockfd) highestsockfd = fd;
                break;
        case 1: FD_SET (fd, write_fdset);     /* Buffer not flushed, wait to write */
                if (fd > highestsockfd) highestsockfd = fd;
                break;
        default: i_printf (1, "i_pg_async_fdset_preprocessor failed to PQflush connection %p", conn);
                 i_pg_async_result_process (self, conn, NULL);
      }
    }
  }

  return highestsockfd;
}

int i_pg_async_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  i_list *list;
  i_pg_async_conn *conn;

  if (select_num < 1)
  { return 0; }

  list = i_pg_async_conn_list ();
  if (!list)
  { return 0; }            

  for (i_list_move_head(list); (conn=i_list_restore(list))!=NULL; (i_list_move_next(list)))
  {
    /* Loop through the connections */

    int fd;
    int num;

    /* Consume the data */
    
    fd = PQsocket (conn->pgconn);
    if (fd == -1)
    {
      i_printf (1, "i_pg_async_fdset_postprocessor get fd of -1 for connection %p", conn);
      i_pg_async_result_process (self, conn, NULL);
      continue;
    }

    if (FD_ISSET(fd, read_fdset))
    {
      /* Data waiting to be read */
      num = PQconsumeInput (conn->pgconn);
      if (num != 1)
      { 
        i_printf (1, "i_pg_async_fdset_postprocessor failed to consume input for connection %p", conn);
        i_pg_async_result_process (self, conn, NULL);
        continue;
      }
    }

    if (FD_ISSET(fd, write_fdset))
    {
      /* Data can be written to backend, flush the buffer */
      PQflush (conn->pgconn);
    }

    /* Check for result */

    num = PQisBusy(conn->pgconn);
    if (num == 0)
    {
      /* Result is present, get and process */
      i_list *result_list;
      PGresult *result;

      result_list = i_list_create ();
      if (!result_list)
      { i_printf (1, "i_pg_async_fdset_postprocessor failed to create result_list"); continue; }
      i_list_set_destructor (result_list, i_postgres_pqclear_wrapper);
      
      while ((result = PQgetResult (conn->pgconn)) != NULL)
      { i_list_enqueue (result_list, result); }

      for (i_list_move_head(result_list); (result=i_list_restore(result_list))!=NULL; i_list_move_next(result_list))
      {
        num = i_pg_async_result_process (self, conn, result);
        if (num != 0)
        { i_printf (1, "i_pg_async_fdset_postprocessor failed to process result from conn %p", conn); }
      }

      i_list_free (result_list);
    }

    /* Finished per-connection iteration */
  }

  return 0;
}
  
