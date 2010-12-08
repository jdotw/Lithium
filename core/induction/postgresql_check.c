#include <stdlib.h>

#include "induction.h"
#include "postgresql.h"

/* Database existence check */

int i_pg_check_db (i_resource *self, char *dbname)
{
  int retval;
  char *command;
  PGconn *conn;
  PGresult *res;

  if (!self || !dbname) return -1;

  conn = i_pg_connect (self, "lithium");
  if (!conn)
  { i_printf (1, "i_pg_check_db failed to connect to database"); return -1; }

  asprintf (&command, "SELECT datname FROM pg_database WHERE datname='%s'", dbname);
  res = PQexec (conn, command);
  free (command);
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || (PQntuples(res)) < 1)
  { retval = -1; }
  else
  { retval = 0; }

  PQclear(res);
  i_pg_close (conn);

  return retval;
}



/* Database check/create */

int i_pg_checkcreate_db (i_resource *self, char *dbname)
{
  int num;

  num = i_pg_check_db (self, dbname);
  if (num != 0)
  {
    /* DB not present */
    return i_pg_create_db (self, dbname);
  }

  return 0;
}
