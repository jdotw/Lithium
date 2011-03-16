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

#include "group.h"

/*
 * Groups - Generic multi-level containers for entities
 */

int l_group_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_group_enable failed to connect to postgres database"); return -1; }

  /* Check groups table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'groups' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE groups (id serial, descr varchar, parent integer)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_group_enable failed to create groups table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Check group_entities table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'group_entities' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE group_entities (id serial, parent integer, ent_type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_group_enable failed to create group_entities table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Perform table maintenance
   *
   * - Delete all orphan group entities 
   */
  pgres = PQexec(pgconn, "DELETE FROM group_entities where id IN (select group_entities.id from group_entities left join devices on devices.name = group_entities.dev_name and devices.site = group_entities.site_name left join sites on sites.name = group_entities.site_name WHERE devices.uuid IS NULL OR sites.name is NULL)");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  { i_printf(0, "l_group_enable warning: failed to perform table maintenance (%s)", PQresultErrorMessage (pgres)); }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Get next group ID */
  pgres = PQexec (pgconn, "SELECT nextval('groups_id_seq')");
  if (pgres && PQresultStatus(pgres) == PGRES_TUPLES_OK && PQntuples(pgres) > 0)
  {
    char *curid_str = PQgetvalue (pgres, 0, 0);
    if (curid_str) l_group_id_setcurrent (atol(curid_str));
  } 
  else
  { i_printf (1, "l_group_enable warning, failed to retrieve next group ID from SQL (%s)", PQresultErrorMessage (pgres)); }   
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Close SQL */
  i_pg_close (pgconn);
  
  return 0;
}

l_group* l_group_create ()
{
  l_group *doc = (l_group *) malloc (sizeof(l_group));
  memset (doc, 0, sizeof(l_group));
  return doc;
}

void l_group_free (void *docptr)
{
  l_group *doc = docptr;

  if (doc->desc_str) free (doc->desc_str);

  free (doc);
}

l_group_entity* l_group_entity_create ()
{
  l_group_entity *g_ent = (l_group_entity *) malloc (sizeof(l_group_entity));
  memset (g_ent, 0, sizeof(l_group_entity));
  return g_ent;
}

void l_group_entity_free (void *entptr)
{
  l_group_entity *g_ent = entptr;

  if (g_ent->entdesc) i_entity_descriptor_free (g_ent->entdesc);

  free (g_ent);
}


