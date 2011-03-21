#include <stdlib.h>

#include <induction.h>
#include <induction/postgresql.h>
#include <induction/timer.h>

#include "pgsql_maint.h"

/* PostgreSQL Maintenance */

#define PGSQL_MAINT_SEC 43200 /* 12 Hours */

static int static_enabled = 0;
static i_timer *static_maint_timer = NULL;

/* Enable / Disable */

int l_pgsql_maint_enable (i_resource *self)
{
  if (static_enabled == 1)
  { i_printf (1, "l_pgsql_maint_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  /* Install timer */
  static_maint_timer = i_timer_add (self, PGSQL_MAINT_SEC, 0, l_pgsql_maint_timercb, NULL);
  if (!static_maint_timer)
  { i_printf (1, "l_pgsql_maint_enable failed to install static_maint_timer"); l_pgsql_maint_disable (self); return -1; }

  /* Call initial maint */
//  l_pgsql_maint_timercb (self, NULL, NULL);

  return 0;
}

int l_pgsql_maint_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "l_pgsql_maint_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;

  if (static_maint_timer)
  { i_timer_remove (static_maint_timer); static_maint_timer = NULL; }

  return 0;
}

/* Timer callback */

int l_pgsql_maint_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Always return 0 */

  int num;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_pgsql_maint_timercb failed to connect to SQL database"); return -1; }

  /* Execute VACUUM FULL */
  num = i_pg_async_query_exec (self, conn, "VACUUM FULL", 0, l_pgsql_maint_vacuumcb, conn);
  if (num != 0)
  {
    i_printf (1, "l_pgsql_maint_timercb failed to execute VACUUM command");
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

/* SQL Callbacks */

int l_pgsql_maint_vacuumcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  int num;

  /* Check Result */
  if (!result)
  {
    i_printf (1, "l_pgsql_maint_callback VACUUM command failed (NULL result received)");
    i_pg_async_conn_close (conn);
    return -1;
  }
  if (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK)
  { 
    char *str;
    i_printf (1, "l_pgsql_maint_callback VACUUM command failed (%s)", PQresultErrorMessage (result));
    asprintf (&str, "VACUUM command failed (%s)", PQresultErrorMessage(result));
    free (str);
  }

  /* Execute ANALYZE */
  num = i_pg_async_query_exec (self, conn, "ANALYZE", 0, l_pgsql_maint_analyzecb, conn);
  if (num != 0)
  {
    i_printf (1, "l_pgsql_maint_timercb failed to execute VACUUM command");
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

int l_pgsql_maint_analyzecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check Result */
  if (!result)
  {
    i_printf (1, "l_pgsql_maint_callback ANALYZE command failed (NULL result received)");
    i_pg_async_conn_close (conn);
    return -1;
  }
  if (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK)
  { 
    char *str;
    i_printf (1, "l_pgsql_maint_callback ANALYZE command failed (%s)", PQresultErrorMessage (result));
    asprintf (&str, "ANALYZE command failed (%s)", PQresultErrorMessage(result));
    free (str);
  }

  /* Close Connection */
  i_pg_async_conn_close (conn);

  return 0;
}

