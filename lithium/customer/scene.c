#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "scene.h"

/*
 * 'Scenes' -- User-defined background images with monitoring data 
 *             overlayed to show live status. 
 *
 *             Uses l_document for document storage.
 */

/* Enable Sub-System */

int l_scene_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_scene_enable failed to connect to postgres database"); return -1; }

  /* Check scenes table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'scenes' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE scenes (doc integer, background bytea)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_scene_enable failed to create scenes table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Check scene_overlays table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'scene_overlays' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    if (pgres) { PQclear (pgres); pgres = NULL; }
    pgres = PQexec (pgconn, "CREATE TABLE scene_overlays (doc integer, uuid varchar, x real, y real, width real, height real, ent_type integer, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar, autosize integer)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_scene_enable failed to create overlays table (%s)", PQresultErrorMessage (pgres)); }
  }
  if (pgres) { PQclear (pgres); pgres = NULL; }

  /* Close SQL */
  i_pg_close (pgconn);

  return 0;
}

/* Struct */

l_scene* l_scene_create ()
{
  l_scene *scene = malloc (sizeof(l_scene));
  memset (scene, 0, sizeof(l_scene));

  scene->overlay_list = i_list_create ();
  i_list_set_destructor (scene->overlay_list, l_scene_overlay_free);

  return scene;
}

void l_scene_free (void *sceneptr)
{
  l_scene *scene = sceneptr;

  if (scene->overlay_list) i_list_free (scene->overlay_list);
  if (scene->background_data) free (scene->background_data);

  free (scene);
}

