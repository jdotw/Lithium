#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/callback.h>
#include <induction/message.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/incident.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "scene.h"

/*
 * Scene Retrieval
 *
 * This is a two stage process:
 *
 * 1) Query the scenes table for basic info
 * 2) Query the scene_overlays table for the list of overlays
 *
 * Parse the output and fire a callback passing it the l_scene struct
 */

i_callback* l_scene_sql_get (i_resource *self, int doc_id, int (*cbfunc) (), void *passdata)
{
  int num;

  /* Callback */
  i_callback *cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_scene_sql_get failed to open SQL database connection"); return NULL; }

  /* Create query */
  char *query;
  asprintf (&query, "SELECT doc, background FROM scenes WHERE doc='%i'", doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_scene_sql_get_scenecb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_scene_sql_get failed to execute initial SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_scene_sql_get_scenecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_aciton_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Check row count */
  int row_count = PQntuples (res);
  if (row_count < 1)
  {
    i_printf (1, "l_aciton_sql_list_cb failed to find scene");
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  int num;
  int row;
  l_scene *scene = l_scene_create ();
  cb->data = scene;
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *background_data;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    background_data = PQgetvalue (res, row, 1);

    /* Setup scene */
    if (id_str) scene->doc_id = atoi (id_str);
    if (background_data) scene->background_data = strdup (background_data);
  }

  /* Query scene overlays */
  char *query;
  asprintf (&query, "SELECT uuid, x, y, width, height, ent_type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, autosize FROM scene_overlays WHERE doc='%i'", scene->doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_scene_sql_get_overlaycb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_scene_sql_get failed to execute scene_overlays SELECT query"); i_callback_free (cb); return -1; }

  return 0;
}

int l_scene_sql_get_overlaycb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the scene_overlays SELECT query */
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_scene_sql_get_overlaycb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  int num;
  int row;
  int row_count;
  l_scene *scene = cb->data;
  cb->data = scene;
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *uuid_str;
    char *x_str;
    char *y_str;
    char *width_str;
    char *height_str;
    char *ent_type_str;
    char *site_name;
    char *site_desc;
    char *site_suburb;
    char *dev_name;
    char *dev_desc;
    char *cnt_name;
    char *cnt_desc;
    char *obj_name;
    char *obj_desc;
    char *met_name;
    char *met_desc;
    char *trg_name;
    char *trg_desc;
    char *autosize_str;

    /* Fields */
    uuid_str = PQgetvalue (res, row, 0);
    x_str = PQgetvalue (res, row, 1);
    y_str = PQgetvalue (res, row, 2);
    width_str = PQgetvalue (res, row, 3);
    height_str = PQgetvalue (res, row, 4);
    ent_type_str = PQgetvalue (res, row, 5);
    site_name = PQgetvalue (res, row, 6);
    site_desc = PQgetvalue (res, row, 7);
    site_suburb = PQgetvalue (res, row, 8);
    dev_name = PQgetvalue (res, row, 9);
    dev_desc = PQgetvalue (res, row, 10);
    cnt_name = PQgetvalue (res, row, 11);
    cnt_desc = PQgetvalue (res, row, 12);
    obj_name = PQgetvalue (res, row, 13);
    obj_desc = PQgetvalue (res, row, 14);
    met_name = PQgetvalue (res, row, 15);
    met_desc = PQgetvalue (res, row, 16);
    trg_name = PQgetvalue (res, row, 17);
    trg_desc = PQgetvalue (res, row, 18);
    autosize_str = PQgetvalue (res, row, 19);

    /* Create overlay */
    l_scene_overlay *overlay = l_scene_overlay_create ();
    overlay->uuid_str = strdup (uuid_str);
    overlay->x = atof (x_str);
    overlay->y = atof (y_str);
    overlay->width = atof (width_str);
    overlay->height = atof (height_str);
    overlay->autosize = atoi (autosize_str);

    /* Create entity */
    if (atoi(ent_type_str) > 0)
    {
      overlay->entdesc = i_entity_descriptor_create ();
      if (ent_type_str) overlay->entdesc->type = atoi (ent_type_str);
      overlay->entdesc->cust_name = strdup (self->hierarchy->cust_name);
      overlay->entdesc->cust_desc = strdup (self->hierarchy->cust_desc);
      if (site_name) overlay->entdesc->site_name = strdup (site_name);
      if (site_desc) overlay->entdesc->site_desc = strdup (site_desc);
      if (site_suburb) overlay->entdesc->site_suburb = strdup (site_suburb);
      if (dev_name) overlay->entdesc->dev_name = strdup (dev_name);
      if (dev_desc) overlay->entdesc->dev_desc = strdup (dev_desc);
      if (cnt_name) overlay->entdesc->cnt_name = strdup (cnt_name);
      if (cnt_desc) overlay->entdesc->cnt_desc = strdup (cnt_desc);
      if (obj_name) overlay->entdesc->obj_name = strdup (obj_name);
      if (obj_desc) overlay->entdesc->obj_desc = strdup (obj_desc);
      if (met_name) overlay->entdesc->met_name = strdup (met_name);
      if (met_desc) overlay->entdesc->met_desc = strdup (met_desc);
      if (trg_name) overlay->entdesc->trg_name = strdup (trg_name);
      if (trg_desc) overlay->entdesc->trg_desc = strdup (trg_desc);
    }

    /* Enqueue */
    i_list_enqueue (scene->overlay_list, overlay);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, scene, cb->passdata);
    if (num != 0) l_scene_free (scene);
  }
  else l_scene_free (scene);

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Scene SQL Operations */

int l_scene_sql_insert (i_resource *self, l_scene *scene)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_scene_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "INSERT INTO scenes (doc, background) VALUES ('%i', '%s');",
    scene->doc_id, scene->background_data);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_scene_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_scene_sql_insert failed to execute INSERT for new scene"); return -1; }

  return 0;
}

int l_scene_sql_delete (i_resource *self, int doc_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_scene_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM scenes WHERE doc='%i'", doc_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_scene_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_scene_sql_update failed to execute DELETE for scene %li", doc_id); return -1; }

  return 0;
}

/* Scene Overlay SQL Operations */

int l_scene_sql_overlay_insert (i_resource *self, l_scene *scene, l_scene_overlay *overlay)
{
  int num;
  char *site_name;
  char *site_desc;
  char *site_suburb;
  char *dev_name;
  char *dev_desc;
  char *cnt_name;
  char *cnt_desc;
  char *obj_name;
  char *obj_desc;
  char *met_name;
  char *met_desc;
  char *trg_name;
  char *trg_desc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_scene_overlay_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  if (overlay->entdesc && overlay->entdesc->site_name) 
  { asprintf (&site_name, "'%s'",  overlay->entdesc->site_name); asprintf (&site_desc, "'%s'", overlay->entdesc->site_desc); }
  else 
  { asprintf (&site_name, "NULL"); asprintf (&site_desc, "NULL"); }
  if (overlay->entdesc && overlay->entdesc->site_suburb) 
  { asprintf (&site_suburb, "'%s'", overlay->entdesc->site_suburb); }
  else 
  { asprintf (&site_suburb, "NULL"); }
  if (overlay->entdesc && overlay->entdesc->dev_name) 
  { asprintf (&dev_name, "'%s'",  overlay->entdesc->dev_name); asprintf (&dev_desc, "'%s'", overlay->entdesc->dev_desc); }
  else 
  { asprintf (&dev_name, "NULL"); asprintf (&dev_desc, "NULL"); }
  if (overlay->entdesc && overlay->entdesc->cnt_name) 
  { asprintf (&cnt_name, "'%s'",  overlay->entdesc->cnt_name); asprintf (&cnt_desc, "'%s'", overlay->entdesc->cnt_desc); }
  else 
  { asprintf (&cnt_name, "NULL"); asprintf (&cnt_desc, "NULL"); }
  if (overlay->entdesc && overlay->entdesc->obj_name) 
  { asprintf (&obj_name, "'%s'",  overlay->entdesc->obj_name); asprintf (&obj_desc, "'%s'", overlay->entdesc->obj_desc); }
  else 
  { asprintf (&obj_name, "NULL"); asprintf (&obj_desc, "NULL"); }
  if (overlay->entdesc && overlay->entdesc->met_name) 
  { asprintf (&met_name, "'%s'",  overlay->entdesc->met_name); asprintf (&met_desc, "'%s'", overlay->entdesc->met_desc); }
  else 
  { asprintf (&met_name, "NULL"); asprintf (&met_desc, "NULL"); }
  if (overlay->entdesc && overlay->entdesc->trg_name) 
  { asprintf (&trg_name, "'%s'",  overlay->entdesc->trg_name); asprintf (&trg_desc, "'%s'", overlay->entdesc->trg_desc); }
  else 
  { asprintf (&trg_name, "NULL"); asprintf (&trg_desc, "NULL"); }
  
  asprintf (&query, "INSERT INTO scene_overlays (doc, uuid, x, y, width, height, ent_type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, autosize) VALUES ('%li', '%s', '%.2f', '%.2f', '%.2f', '%.2f', '%i', %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, '%i');",
    scene->doc_id, overlay->uuid_str, overlay->x, overlay->y, overlay->width, overlay->height, 
    overlay->entdesc ? overlay->entdesc->type : 0, site_name, site_desc, site_suburb, dev_name, dev_desc, 
    cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, overlay->autosize);
  if (site_name) free (site_name);
  if (site_desc) free (site_desc);
  if (site_suburb) free (site_suburb);
  if (dev_name) free (dev_name);
  if (dev_desc) free (dev_desc);
  if (cnt_name) free (cnt_name);
  if (cnt_desc) free (cnt_desc);
  if (obj_name) free (obj_name);
  if (obj_desc) free (obj_desc);
  if (met_name) free (met_name);
  if (met_desc) free (met_desc);
  if (trg_name) free (trg_name);
  if (trg_desc) free (trg_desc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_scene_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_scene_overlay_sql_insert failed to execute INSERT for scene overlay for document %li", scene->doc_id); return -1; }

  return 0;
}

int l_scene_sql_overlay_delete (i_resource *self, long doc_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_scene_overlay_sql_delete failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM scene_overlays WHERE doc='%i'", doc_id);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_scene_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_scene_overlay_sql_delete failed to execute delete for doc_id %li", doc_id); return -1; }

  return 0;
}

int l_scene_sql_overlay_delete_device (i_resource *self, i_device *dev)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_scene_sql_overlay_delete_device failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM scene_overlays WHERE site_name='%s' AND dev_name='%s'", dev->site->name_str, dev->name_str);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_scene_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_scene_overlay_sql_delete_device failed to execute delete"); return -1; }

  return 0;
}

/* SQL Callback */

int l_scene_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_scene_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

