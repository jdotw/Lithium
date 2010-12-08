#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "service.h"

/*
 * 'Services' -- User-defined scripts for monitoring protocols and services
 *
 */

/* Enable Sub-System */

static int static_enabled = 0;
static i_container *static_cnt = NULL;
static i_list *static_list = NULL;

i_container* l_service_cnt ()
{ return static_cnt; }

i_list* l_service_list ()
{ return static_list; }

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
    pgres = PQexec (pgconn, "CREATE TABLE services (id serial, descr varchar, enabled integer, script varchar, site varchar, device varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_service_enable failed to create services table (%s)", PQresultErrorMessage (pgres)); }
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

  /* Check service_history table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'service_history' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE service_history (id serial, service integer, timestamp integer, comments varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_service_enable failed to create entities table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);
  
  /* Close SQL */
  i_pg_close (pgconn);

  /* Create/Config Container */
  static_cnt = i_container_create ("service", "Services");
  if (!static_cnt)
  { i_printf (1, "l_service_enable failed to create container"); return -1; }
  
  /* Register entity */
  int num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_service_enable failed to register container"); return -1; }
  
  /* Load/Apply refresh config */
  static i_entity_refresh_config defrefconfig;
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_PARENT;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)  { i_printf (1, "l_service_enable failed to load and apply container refresh config"); return -1; }
  
  /* Create item list */
  static_cnt->item_list = i_list_create ();
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Load services */
  l_service_sql_load_list (self, NULL, self->hierarchy->site->name_str, self->hierarchy->dev->name_str, l_service_enable_loadcb, NULL);

  /* Set flag */
  static_enabled = 1;
  
  return 0;
}

int l_service_enable_loadcb (i_resource *self, i_list *list, void *passdata)
{
  /* Create objects for services */
  l_service *service;
  for (i_list_move_head(list); (service=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    l_service_object_add (self, service);
  }

  static_list = list;

  return 0;     /* Return 0 to prevent sqlcb freeing the list */
}

/* Struct */

l_service* l_service_create ()
{
  l_service *service = malloc (sizeof(l_service));
  memset (service, 0, sizeof(l_service));

  service->configvar_list = i_list_create ();

  return service;
}

void l_service_free (void *serviceptr)
{
  l_service *service = serviceptr;

  if (service->desc_str) free (service->desc_str);
  if (service->configvar_list) i_list_free (service->configvar_list);
  if (service->script_file) free (service->script_file);

  free (service);
}

