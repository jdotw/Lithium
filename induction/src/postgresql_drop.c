#include <stdlib.h>

#include "induction.h"
#include "postgresql.h"

/* Database dropping */

int i_pg_drop_db (i_resource *self, char *dbname)
{
  char *command;
  PGconn *conn;
  PGresult *res;

  if (!self || !dbname) return -1;

  conn = i_pg_connect (self, "lithium");
  if (!conn)
  { i_printf (1, "i_pg_drop_db failed to connect to NULL database to call DROP DATABASE"); return -1; }

  asprintf (&command, "DROP DATABASE %s", dbname);
  res = PQexec (conn, command);
  free (command);
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
  { 
    i_printf (1, "i_pg_drop_db failed to execute DROP DATABASE %s (%s)", PQresultErrorMessage (res), dbname); 
    i_pg_close (conn); 
    return -1; 
  }
  PQclear(res);

  i_pg_close (conn);

  return 0;
}
