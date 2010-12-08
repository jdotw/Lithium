#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "case.h"

/*
 * Case System
 */

static int static_enabled = 0;

l_case* l_case_create ()
{
  l_case *cas;

  cas = (l_case *) malloc (sizeof(l_case));
  if (!cas) return NULL;
  memset (cas, 0, sizeof(l_case));

  cas->ent_list = i_list_create ();
  i_list_set_destructor (cas->ent_list, i_entity_descriptor_free);

  cas->log_list = i_list_create ();
  i_list_set_destructor (cas->log_list, l_case_logentry_free);

  return cas;
}

void l_case_free (void *caseptr)
{ 
  l_case *cas = caseptr;

  if (cas->hline_str) free (cas->hline_str);
  if (cas->owner_str) free (cas->owner_str);
  if (cas->requester_str) free (cas->requester_str);
  if (cas->ent_list) i_list_free (cas->ent_list);
  if (cas->log_list) i_list_free (cas->log_list);

  free (cas);
}

/* Enable/Disable */

int l_case_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  /* Set state */
  static_enabled = 1;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_case_enable failed to connect to postgres database"); return -1; }

  /* Check cases table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'cases' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE cases (id serial, state integer, start_sec integer, end_sec integer, hline varchar, owner varchar, requester varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_case_enable failed to create cases table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check case_logentries table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'case_logentries' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE case_logentries (id serial, caseid integer, case_state integer, type integer, tstamp integer, timespent_sec integer, author varchar, entry varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_case_enable failed to create logentries table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check case_entities table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'case_entities' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE case_entities (id serial, caseid integer, case_state integer, type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar, tstamp integer)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_case_enable failed to create logentries table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);
  
  /* Get next case ID */
  pgres = PQexec (pgconn, "SELECT nextval('cases_id_seq')");
  if (pgres && PQresultStatus(pgres) == PGRES_TUPLES_OK && PQntuples(pgres) > 0)
  {
    char *curid_str = PQgetvalue (pgres, 0, 0);
    if (curid_str) l_case_id_setcurrent (atol(curid_str)); 
  }
  else
  { i_printf (1, "l_case_enable warning, failed to retrieve next case ID from SQL (%s)", PQresultErrorMessage (pgres)); }
  PQclear (pgres);
  
  /* Close SQL */
  i_pg_close (pgconn);

  return 0;
}
