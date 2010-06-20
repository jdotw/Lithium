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

#include "action.h"

/*
 * 'Actions' -- User-defined script to be run in the event
 *              of an incident occurring. 
 *
 *              Dreamt up on the balcony, Friday 26th April
 */

/* Enable Sub-System */

static int static_enabled = 0;

int l_action_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_action_enable failed to connect to postgres database"); return -1; }

  /* Check actions table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'actions' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE actions (id serial, descr varchar, enabled integer, activation integer, delay integer, rerun integer, rerundelay integer, timefilter integer, daymask integer, starthour integer, endhour integer, script varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_action_enable failed to create actions table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check action_entities table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'action_entities' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE action_entities (id serial, action integer, type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_action_enable failed to create entities table (%s)", PQresultErrorMessage (pgres)); }

    /* Create Dummy Record FIX: Not sure why this is needed.... but it is. */
    pgres = PQexec (pgconn, "INSERT INTO action_entities (action) VALUES ('0')");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_action_enable failed to insert dummy action_entities record (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check action_configvar table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'action_configvars' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE action_configvars (id serial, action integer, name varchar, value varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_action_enable failed to create configvar table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check action_entities table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'action_history' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE action_history (id serial, action integer, type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar, timestamp integer, comments varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_action_enable failed to create entities table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);
  
  /* Get next case ID */
  pgres = PQexec (pgconn, "SELECT nextval('actions_id_seq')");
  if (pgres && PQresultStatus(pgres) == PGRES_TUPLES_OK && PQntuples(pgres) > 0)
  {
    char *curid_str = PQgetvalue (pgres, 0, 0);
    if (curid_str) l_action_id_setcurrent (atol(curid_str));
  } 
  else
  { i_printf (1, "l_action_enable warning, failed to retrieve next action ID from SQL (%s)", PQresultErrorMessage (pgres)); }   
  PQclear (pgres);

  /* Close SQL */
  i_pg_close (pgconn);

  /* Set flag */
  static_enabled = 1;
  
  return 0;
}

/* Struct */

l_action* l_action_create ()
{
  l_action *action = malloc (sizeof(l_action));
  memset (action, 0, sizeof(l_action));

  action->entity_list = i_list_create ();
  action->configvar_list = i_list_create ();

  return action;
}

void l_action_free (void *actionptr)
{
  l_action *action = actionptr;

  if (action->desc_str) free (action->desc_str);
  if (action->entity_list) i_list_free (action->entity_list);
  if (action->configvar_list) i_list_free (action->configvar_list);
  if (action->script_file) free (action->script_file);

  free (action);
}

