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

#include "service.h"

/*
 * 'Actions' -- User-defined script to be run in the event
 *              of an incident occurring. 
 *
 *              Dreamt up on the balcony, Friday 26th April
 */

/* Enable Sub-System */

static int static_enabled = 0;

int l_service_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_service_enable failed to connect to postgres database"); return -1; }

  /* Check services table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'services' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE services (id serial, descr varchar, enabled integer, activation integer, delay integer, rerun integer, rerundelay integer, timefilter integer, daymask integer, starthour integer, endhour integer, script varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_service_enable failed to create services table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check service_entities table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'service_entities' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE service_entities (id serial, service integer, type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_service_enable failed to create entities table (%s)", PQresultErrorMessage (pgres)); }

    /* Create Dummy Record FIX: Not sure why this is needed.... but it is. */
    pgres = PQexec (pgconn, "INSERT INTO service_entities (service) VALUES ('0')");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_service_enable failed to insert dummy service_entities record (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check service_configvar table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'service_configvars' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE service_configvars (id serial, service integer, name varchar, value varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_service_enable failed to create configvar table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* Check service_entities table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'service_history' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE service_history (id serial, service integer, type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar, timestamp integer, comments varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_service_enable failed to create entities table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);
  
  /* Get next case ID */
  pgres = PQexec (pgconn, "SELECT nextval('services_id_seq')");
  if (pgres && PQresultStatus(pgres) == PGRES_TUPLES_OK && PQntuples(pgres) > 0)
  {
    char *curid_str = PQgetvalue (pgres, 0, 0);
    if (curid_str) l_service_id_setcurrent (atol(curid_str));
  } 
  else
  { i_printf (1, "l_service_enable warning, failed to retrieve next service ID from SQL (%s)", PQresultErrorMessage (pgres)); }   
  PQclear (pgres);

  /* Close SQL */
  i_pg_close (pgconn);

  /* Set flag */
  static_enabled = 1;
  
  return 0;
}

/* Struct */

l_service* l_service_create ()
{
  l_service *service = malloc (sizeof(l_service));
  memset (service, 0, sizeof(l_service));

  service->entity_list = i_list_create ();
  service->configvar_list = i_list_create ();

  return service;
}

void l_service_free (void *serviceptr)
{
  l_service *service = serviceptr;

  if (service->desc_str) free (service->desc_str);
  if (service->entity_list) i_list_free (service->entity_list);
  if (service->configvar_list) i_list_free (service->configvar_list);
  if (service->script_file) free (service->script_file);

  free (service);
}

