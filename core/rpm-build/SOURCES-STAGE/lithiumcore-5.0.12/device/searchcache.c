#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "searchcache.h"

/* 
 * Device-Specific Search Cache Functions
 *
 * The search cache is a table in the PostgreSQL database
 * that contains a row per entity down to the object level
 * for the purpose of allowing for searching via the
 * customer process for entities
 *
 * The init function for the device scrubs the table
 * of any rows that belong to this device using a match 
 * on the dev_name column
 *
 */

/* Initialization */

int l_searchcache_init (i_resource *self)
{
  /* Connect to SQL db (sync) */
  PGconn *pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_searchcache_init failed to connect to postgres database"); return -1; }

  /* Scrub the table (sync) */
  char *delete_query;
  asprintf(&delete_query, "DELETE from searchcache WHERE dev_name='%s'", self->hierarchy->dev->name_str);
  PGresult *result = PQexec (pgconn, delete_query);
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}


