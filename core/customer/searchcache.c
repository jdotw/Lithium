#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/metric.h>
#include <induction/path.h>
#include <induction/postgresql.h>

#include "searchcache.h"

/* 
 * Cusromer-Specific Search Cache Functions
 *
 * The search cache is a table in the PostgreSQL database
 * that contains a row per entity down to the object level
 * for the purpose of allowing for searching via the
 * customer process for entities
 *
 */

/* Initialization */

int l_searchcache_init (i_resource *self)
{
  /* Connect to SQL db (sync) */
  PGconn *pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_searchcache_init failed to connect to postgres database"); return -1; }

  /* Check the table exists and create it if necessary */
  PGresult *result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'searchcache' AND relkind = 'r'");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* searchcache table not in database */
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "CREATE TABLE searchcache (ent_type integer, descr varchar, ent_address varchar, res_address varchar, dev_name varchar, cust_desc varchar, site_desc varchar, dev_desc varchar, cnt_desc varchar, obj_desc varchar)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_searchcache_init failed to create devices table (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "CREATE INDEX searchcache_descr_idx ON searchcache (descr varchar_pattern_ops)");
  }
  if (result) { PQclear(result); result = NULL; }

  /* Scrub the table (sync) */
  result = PQexec (pgconn, "DELETE from searchcache");
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

int l_searchcache_delete_device (i_resource *self, i_device *dev)
{
  /* Connect to SQL db (sync) */
  PGconn *pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_searchcache_init failed to connect to postgres database"); return -1; }

  /* Scrub the table (sync) */
  char *delete_query;
  asprintf(&delete_query, "DELETE from searchcache WHERE dev_name='%s'", dev->name_str);
  PGresult *result = PQexec (pgconn, delete_query);
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}
