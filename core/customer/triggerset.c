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

#include "triggerset.h"

/* Initialise SQL */

int l_triggerset_initsql (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  /* Connect to SQL db */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_triggerset_initsql failed to connect to postgres database"); return -1; }

  /* Check apprules SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'triggerset_apprules' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    result = PQexec (pgconn, "CREATE TABLE triggerset_apprules (id serial, cnt_name varchar, cnt_desc varchar, tset_name varchar, tset_desc varchar, site_name varchar, site_desc varchar, dev_name varchar, dev_desc varchar, obj_name varchar, obj_desc varchar, applyflag integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_triggerset_initsql failed to create triggersets apprules table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Check valrules SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'triggerset_valrules' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    result = PQexec (pgconn, "CREATE TABLE triggerset_valrules (id serial, cnt_name varchar, cnt_desc varchar, tset_name varchar, tset_desc varchar, trg_name varchar, trg_desc varchar, site_name varchar, site_desc varchar, dev_name varchar, dev_desc varchar, obj_name varchar, obj_desc varchar, trgtype integer, xval varchar, yval varchar, duration integer, adminstate integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_triggerset_initsql failed to create triggersets valrules table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

