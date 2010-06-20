#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/callback.h>
#include <induction/message.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "group.h"

/* Group SQL Operations */

int l_group_sql_insert (i_resource *self, l_group *group)
{
  int num;
  char *desc_esc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_group_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  desc_esc = i_postgres_escape (group->desc_str);
  asprintf (&query, "INSERT INTO groups (descr, parent) VALUES ('%s', '%i')",
    group->desc_str, group->parent_id);
  free (desc_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_insert failed to execute INSERT for new group"); return -1; }

  /* Set id */
  group->id = l_group_id_assign ();

  return 0;
}

int l_group_sql_update (i_resource *self, l_group *group)
{
  int num;
  char *query;
  char *desc_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_group_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  desc_esc = i_postgres_escape (group->desc_str);
  asprintf (&query, "UPDATE groups SET descr='%s', parent='%i' WHERE id='%li'",
    desc_esc, group->parent_id, group->id);
  free (desc_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_update failed to execute UPDATE for group %li (%s)", group->id, group->desc_str); return -1; }

  return 0;
}

int l_group_sql_delete (i_resource *self, int group_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_group_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM groups WHERE id='%li'", group_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_update failed to execute DELETE for group %li", group_id); return -1; }

  return 0;
}

/* Group Entities */

int l_group_sql_entity_insert (i_resource *self, l_group_entity *g_ent)
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
  { i_printf (1, "l_group_sql_entity_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  if (g_ent->entdesc && g_ent->entdesc->site_name) 
  { asprintf (&site_name, "'%s'",  g_ent->entdesc->site_name); asprintf (&site_desc, "'%s'", g_ent->entdesc->site_desc); }
  else 
  { asprintf (&site_name, "NULL"); asprintf (&site_desc, "NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->site_suburb) 
  { asprintf (&site_suburb, "'%s'", g_ent->entdesc->site_suburb); }
  else 
  { asprintf (&site_suburb, "NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->dev_name) 
  { asprintf (&dev_name, "'%s'",  g_ent->entdesc->dev_name); asprintf (&dev_desc, "'%s'", g_ent->entdesc->dev_desc); }
  else 
  { asprintf (&dev_name, "NULL"); asprintf (&dev_desc, "NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->cnt_name) 
  { asprintf (&cnt_name, "'%s'",  g_ent->entdesc->cnt_name); asprintf (&cnt_desc, "'%s'", g_ent->entdesc->cnt_desc); }
  else 
  { asprintf (&cnt_name, "NULL"); asprintf (&cnt_desc, "NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->obj_name) 
  { asprintf (&obj_name, "'%s'",  g_ent->entdesc->obj_name); asprintf (&obj_desc, "'%s'", g_ent->entdesc->obj_desc); }
  else 
  { asprintf (&obj_name, "NULL"); asprintf (&obj_desc, "NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->met_name) 
  { asprintf (&met_name, "'%s'",  g_ent->entdesc->met_name); asprintf (&met_desc, "'%s'", g_ent->entdesc->met_desc); }
  else 
  { asprintf (&met_name, "NULL"); asprintf (&met_desc, "NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->trg_name) 
  { asprintf (&trg_name, "'%s'",  g_ent->entdesc->trg_name); asprintf (&trg_desc, "'%s'", g_ent->entdesc->trg_desc); }
  else 
  { asprintf (&trg_name, "NULL"); asprintf (&trg_desc, "NULL"); }
  
  asprintf (&query, "INSERT INTO group_entities (parent, ent_type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc) VALUES ('%i', '%i', %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);",
    g_ent->parent_id, g_ent->entdesc ? g_ent->entdesc->type : 0, 
    site_name, site_desc, site_suburb, dev_name, dev_desc, 
    cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, 
    trg_name, trg_desc);
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
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_entity_insert failed to execute INSERT"); return -1; }

  return 0;
}

int l_group_sql_entity_delete (i_resource *self, l_group_entity *g_ent)
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
  { i_printf (1, "l_group_sql_entity_delete failed to open SQL database connection"); return -1; }

  /* Create query */
  if (g_ent->entdesc && g_ent->entdesc->site_name) 
  { asprintf (&site_name, "='%s'",  g_ent->entdesc->site_name); asprintf (&site_desc, "='%s'", g_ent->entdesc->site_desc); }
  else 
  { asprintf (&site_name, " IS NULL"); asprintf (&site_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->site_suburb) 
  { asprintf (&site_suburb, "='%s'", g_ent->entdesc->site_suburb); }
  else 
  { asprintf (&site_suburb, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->dev_name) 
  { asprintf (&dev_name, "='%s'",  g_ent->entdesc->dev_name); asprintf (&dev_desc, "='%s'", g_ent->entdesc->dev_desc); }
  else 
  { asprintf (&dev_name, " IS NULL"); asprintf (&dev_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->cnt_name) 
  { asprintf (&cnt_name, "='%s'",  g_ent->entdesc->cnt_name); asprintf (&cnt_desc, "='%s'", g_ent->entdesc->cnt_desc); }
  else 
  { asprintf (&cnt_name, " IS NULL"); asprintf (&cnt_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->obj_name) 
  { asprintf (&obj_name, "='%s'",  g_ent->entdesc->obj_name); asprintf (&obj_desc, "='%s'", g_ent->entdesc->obj_desc); }
  else 
  { asprintf (&obj_name, " IS NULL"); asprintf (&obj_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->met_name) 
  { asprintf (&met_name, "='%s'",  g_ent->entdesc->met_name); asprintf (&met_desc, "='%s'", g_ent->entdesc->met_desc); }
  else 
  { asprintf (&met_name, " IS NULL"); asprintf (&met_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->trg_name) 
  { asprintf (&trg_name, "='%s'",  g_ent->entdesc->trg_name); asprintf (&trg_desc, "='%s'", g_ent->entdesc->trg_desc); }
  else 
  { asprintf (&trg_name, " IS NULL"); asprintf (&trg_desc, " IS NULL"); }
  
  asprintf (&query, "DELETE FROM group_entities WHERE parent='%i' AND ent_type='%i' AND site_name%s AND site_desc%s AND site_suburb%s AND dev_name%s AND dev_desc%s AND cnt_name%s AND cnt_desc%s AND obj_name%s AND obj_desc%s AND met_name%s AND met_desc%s AND trg_name%s AND trg_desc%s",
    g_ent->parent_id, g_ent->entdesc ? g_ent->entdesc->type : 0, 
    site_name, site_desc, site_suburb, dev_name, dev_desc, 
    cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, 
    trg_name, trg_desc);
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
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_entity_delete failed to execute DELETE"); return -1; }

  return 0;
}

int l_group_sql_entity_delete_device (i_resource *self, i_device *dev)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_group_sql_entity_delete_device failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM group_entities WHERE site_name='%s' AND dev_name='%s'", dev->site->name_str, dev->name_str);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_entity_delete_device failed to execute DELETE"); return -1; }

  return 0;
}

int l_group_sql_entity_move (i_resource *self, int prev_parent, l_group_entity *g_ent)
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
  { i_printf (1, "l_group_sql_entity_move failed to open SQL database connection"); return -1; }

  /* Create query */
  if (g_ent->entdesc && g_ent->entdesc->site_name) 
  { asprintf (&site_name, "='%s'",  g_ent->entdesc->site_name); asprintf (&site_desc, "='%s'", g_ent->entdesc->site_desc); }
  else 
  { asprintf (&site_name, " IS NULL"); asprintf (&site_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->site_suburb) 
  { asprintf (&site_suburb, "='%s'", g_ent->entdesc->site_suburb); }
  else 
  { asprintf (&site_suburb, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->dev_name) 
  { asprintf (&dev_name, "='%s'",  g_ent->entdesc->dev_name); asprintf (&dev_desc, "='%s'", g_ent->entdesc->dev_desc); }
  else 
  { asprintf (&dev_name, " IS NULL"); asprintf (&dev_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->cnt_name) 
  { asprintf (&cnt_name, "='%s'",  g_ent->entdesc->cnt_name); asprintf (&cnt_desc, "='%s'", g_ent->entdesc->cnt_desc); }
  else 
  { asprintf (&cnt_name, " IS NULL"); asprintf (&cnt_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->obj_name) 
  { asprintf (&obj_name, "='%s'",  g_ent->entdesc->obj_name); asprintf (&obj_desc, "='%s'", g_ent->entdesc->obj_desc); }
  else 
  { asprintf (&obj_name, " IS NULL"); asprintf (&obj_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->met_name) 
  { asprintf (&met_name, "='%s'",  g_ent->entdesc->met_name); asprintf (&met_desc, "='%s'", g_ent->entdesc->met_desc); }
  else 
  { asprintf (&met_name, " IS NULL"); asprintf (&met_desc, " IS NULL"); }
  if (g_ent->entdesc && g_ent->entdesc->trg_name) 
  { asprintf (&trg_name, "='%s'",  g_ent->entdesc->trg_name); asprintf (&trg_desc, "='%s'", g_ent->entdesc->trg_desc); }
  else 
  { asprintf (&trg_name, " IS NULL"); asprintf (&trg_desc, " IS NULL"); }
  
  asprintf (&query, "UPDATE group_entities SET parent='%i' WHERE parent='%i' AND ent_type='%i' AND site_name%s AND site_desc%s AND site_suburb%s AND dev_name%s AND dev_desc%s AND cnt_name%s AND cnt_desc%s AND obj_name%s AND obj_desc%s AND met_name%s AND met_desc%s AND trg_name%s AND trg_desc%s",
    g_ent->parent_id, prev_parent, g_ent->entdesc ? g_ent->entdesc->type : 0, 
    site_name, site_desc, site_suburb, dev_name, dev_desc, 
    cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, 
    trg_name, trg_desc);
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
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_entity_move failed to execute UPDATE"); return -1; }

  return 0;
}

/*
 * Loading the list of groups is a two stage process:
 *
 * 1) Load the list of groups
 * 2) Load the entities in the groups
 *
 * If a non-zero use_group_id is specified, then only the
 * children of that group are loaded 
 *
 */

i_callback* l_group_sql_load_list (i_resource *self, int group_id, int use_group_id, int (*cbfunc) (), void *passdata)
{
  /* Load list of all groups */
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;
  if (use_group_id)
  {
    cb->data = malloc (sizeof(int));
    memcpy (cb->data, &group_id, sizeof(int));
  }

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_group_sql_load_list failed to open SQL database connection"); return NULL; }

  /* Create query */
  if (use_group_id > 0)
  { 
    asprintf (&query, "SELECT id, descr, parent FROM groups WHERE parent='%i'", group_id); 
  }
  else
  { asprintf (&query, "SELECT id, descr, parent FROM groups"); }

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_group_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
  i_callback *cb = (i_callback *) passdata;
  int use_group_id = 0;
  int group_id = 0;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_group_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Retrieve group id */
  if (cb->data)
  {
    memcpy (&group_id, cb->data, sizeof(int));
    use_group_id = 1;
    free (cb->data);
    cb->data = NULL;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_group_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *descr_str;
    char *parent_str;
    l_group *group;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    descr_str = PQgetvalue (res, row, 1);
    parent_str = PQgetvalue (res, row, 2);

    /* Create group */
    group = l_group_create ();
    if (id_str) group->id = atoi (id_str);
    if (descr_str) group->desc_str = strdup (descr_str);
    if (parent_str) group->parent_id = atoi (parent_str);

    /* Enqueue */
    i_list_enqueue (list, group);
  }
  cb->data = list;

  /* Create query for entities */
  char *query;
  if (use_group_id)
  {
    asprintf (&query, "SELECT parent, ent_type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc FROM group_entities WHERE parent='%i'", group_id);
  }
  else
  {
    asprintf (&query, "SELECT parent, ent_type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc FROM group_entities");
  }

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_group_sql_load_entity_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_group_sql_load_list failed to execute SELECT query"); i_callback_free (cb); return 0; }

  return 0;
}

int l_group_sql_load_entity_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the group_entities SELECT query */
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_group_sql_load_entity_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  int num;
  int row;
  int row_count;
  i_list *list = i_list_create ();
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *parent_str;
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

    /* Fields */
    parent_str = PQgetvalue (res, row, 0);
    ent_type_str = PQgetvalue (res, row, 1);
    site_name = PQgetvalue (res, row, 2);
    site_desc = PQgetvalue (res, row, 3);
    site_suburb = PQgetvalue (res, row, 4);
    dev_name = PQgetvalue (res, row, 5);
    dev_desc = PQgetvalue (res, row, 6);
    cnt_name = PQgetvalue (res, row, 7);
    cnt_desc = PQgetvalue (res, row, 8);
    obj_name = PQgetvalue (res, row, 9);
    obj_desc = PQgetvalue (res, row, 10);
    met_name = PQgetvalue (res, row, 11);
    met_desc = PQgetvalue (res, row, 12);
    trg_name = PQgetvalue (res, row, 13);
    trg_desc = PQgetvalue (res, row, 14);

    /* Create group entity */
    l_group_entity *g_ent = l_group_entity_create ();
    g_ent->parent_id = atoi (parent_str);

    /* Create entity */
    if (atoi(ent_type_str) > 0)
    {
      g_ent->entdesc = i_entity_descriptor_create ();
      if (ent_type_str) g_ent->entdesc->type = atoi (ent_type_str);
      g_ent->entdesc->cust_name = strdup (self->hierarchy->cust_name);
      g_ent->entdesc->cust_desc = strdup (self->hierarchy->cust_desc);
      if (site_name) g_ent->entdesc->site_name = strdup (site_name);
      if (site_desc) g_ent->entdesc->site_desc = strdup (site_desc);
      if (site_suburb) g_ent->entdesc->site_suburb = strdup (site_suburb);
      if (dev_name) g_ent->entdesc->dev_name = strdup (dev_name);
      if (dev_desc) g_ent->entdesc->dev_desc = strdup (dev_desc);
      if (cnt_name) g_ent->entdesc->cnt_name = strdup (cnt_name);
      if (cnt_desc) g_ent->entdesc->cnt_desc = strdup (cnt_desc);
      if (obj_name) g_ent->entdesc->obj_name = strdup (obj_name);
      if (obj_desc) g_ent->entdesc->obj_desc = strdup (obj_desc);
      if (met_name) g_ent->entdesc->met_name = strdup (met_name);
      if (met_desc) g_ent->entdesc->met_desc = strdup (met_desc);
      if (trg_name) g_ent->entdesc->trg_name = strdup (trg_name);
      if (trg_desc) g_ent->entdesc->trg_desc = strdup (trg_desc);
    }

    /* Enqueue */
    i_list_enqueue (list, g_ent);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, (i_list *)cb->data, list, cb->passdata);
    if (num != 0) 
    {
      i_list_free ((i_list *)cb->data);
      cb->data = NULL;
      i_list_free (list);
    }
  }
  else
  {
    i_list_free ((i_list *)cb->data);
    cb->data = NULL;
    i_list_free (list);
  }

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* SQL Callback */

int l_group_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_group_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

