#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/inventory.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>

#include "record.h"

/* Initialise SQL */

int l_record_initsql (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  /* Connect to SQL db */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_record_initsql failed to connect to postgres database"); return -1; }

  /* Check record rules SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'record_recrules' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    result = PQexec (pgconn, "CREATE TABLE record_recrules (id serial, cnt_name varchar, site_name varchar, site_desc varchar, dev_name varchar, dev_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, recordflag integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_record_initsql failed to create record rules table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

