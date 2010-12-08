#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/callback.h>
#include <induction/timer.h>
#include <induction/name.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "record.h"

/* 
 * Recording Rules - SQL
 */

/* Get */

int l_record_recrule_sql_get (i_resource *self, long id, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  i_callback *callback;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_record_recrule_sql_get failed to connect to SQL database"); return -1; }

  /* Create callback struct */
  callback = i_callback_create ();
  callback->func = cbfunc;
  callback->passdata = passdata;

  /* Execute command */
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, met_name, met_desc, recordflag FROM record_recrules WHERE id='%li'", id);
  num = i_pg_async_query_exec (self, conn, query, 0, l_record_recrule_sql_get_sqlcb, callback);
  free (query);
  if (num != 0)
  {
    i_printf (1, "l_record_recrule_sql_get failed to execute SELECT command");
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  return 0;
}

int l_record_recrule_sql_get_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  i_callback *callback = passdata;
  l_record_recrule *rule = NULL;

  /* Check for result */
  if (result && PQresultStatus(result)==PGRES_TUPLES_OK && PQntuples(result) > 0)
  {
    /* Create list */
    char *id_str;
    char *site_name;
    char *site_desc;
    char *dev_name;
    char *dev_desc;
    char *obj_name;
    char *obj_desc;
    char *met_name;
    char *met_desc;
    char *recordflag_str;

    /* Get values */
    id_str = PQgetvalue (result, 0, 0);
    site_name = PQgetvalue (result, 0, 1);
    site_desc = PQgetvalue (result, 0, 2);
    dev_name = PQgetvalue (result, 0, 3);
    dev_desc = PQgetvalue (result, 0, 4);
    obj_name = PQgetvalue (result, 0, 5);
    obj_desc = PQgetvalue (result, 0, 6);
    met_name = PQgetvalue (result, 0, 7);
    met_desc = PQgetvalue (result, 0, 8);
    recordflag_str = PQgetvalue (result, 0, 9);

    /* Rule */
    rule = l_record_recrule_create ();
    if (id_str) rule->id = atol (id_str);
    if (site_name && strlen(site_name) > 0) rule->site_name = strdup (site_name);
    if (site_desc && strlen(site_desc) > 0) rule->site_desc = strdup (site_desc);
    if (dev_name && strlen(dev_name) > 0) rule->dev_name = strdup (dev_name);
    if (dev_desc && strlen(dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
    if (obj_name && strlen(obj_name) > 0) rule->obj_name = strdup (obj_name);
    if (obj_desc && strlen(obj_name) > 0) rule->obj_desc = strdup (obj_desc);
    if (met_name && strlen(met_name) > 0) rule->met_name = strdup (met_name);
    if (met_desc && strlen(met_name) > 0) rule->met_desc = strdup (met_desc);
    if (recordflag_str) rule->recordflag = atoi (recordflag_str);
  }

  /* Call callback */
  callback->func (self, rule, callback->passdata);

  /* Cleanup */
  l_record_recrule_free (rule);
  i_callback_free (callback);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Insert */

int l_record_recrule_sql_insert (i_resource *self, i_container *cnt, l_record_recrule *rule)
{
  int num;
  char *temp;
  char *query;
  char *site_name_esc;
  char *site_desc_esc;
  char *dev_name_esc;
  char *dev_desc_esc;
  char *obj_name_esc;
  char *obj_desc_esc;
  char *met_name_esc;
  char *met_desc_esc;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_record_recrule_sql_insert failed to connect to SQL database"); return -1; }

  /* Create site strings */
  if (rule->site_name)
  {
    temp = i_postgres_escape (rule->site_name);
    asprintf (&site_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->site_desc);
    asprintf (&site_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  { 
    asprintf (&site_name_esc, "NULL"); 
    asprintf (&site_desc_esc, "NULL"); 
  }

  /* Create dev strings */
  if (rule->dev_name)
  {
    temp = i_postgres_escape (rule->dev_name);
    asprintf (&dev_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->dev_desc);
    asprintf (&dev_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  { 
    asprintf (&dev_name_esc, "NULL"); 
    asprintf (&dev_desc_esc, "NULL"); 
  }

  /* Create obj strings */
  if (rule->obj_name)
  {
    temp = i_postgres_escape (rule->obj_name);
    asprintf (&obj_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->obj_desc);
    asprintf (&obj_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  { 
    asprintf (&obj_name_esc, "NULL"); 
    asprintf (&obj_desc_esc, "NULL"); 
  }

  /* Create met strings */
  if (rule->met_name)
  {
    temp = i_postgres_escape (rule->met_name);
    asprintf (&met_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->met_desc);
    asprintf (&met_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  { 
    asprintf (&met_name_esc, "NULL"); 
    asprintf (&met_desc_esc, "NULL"); 
  }

  asprintf (&query, "INSERT INTO record_recrules (cnt_name, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, met_name, met_desc, recordflag) VALUES ('%s', %s, %s, %s, %s, %s, %s, %s, %s,'%i')",
    cnt->name_str, site_name_esc, site_desc_esc, dev_name_esc, dev_desc_esc, obj_name_esc, obj_desc_esc, met_name_esc, met_desc_esc, rule->recordflag);
  free (site_name_esc);
  free (site_desc_esc);
  free (dev_name_esc);
  free (dev_desc_esc);
  free (obj_name_esc);
  free (obj_desc_esc);
  free (met_name_esc);
  free (met_desc_esc);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, l_record_recrule_sql_sqlcb, query);
  if (num != 0)
  {
    i_printf (1, "l_record_recrule_sql_insert failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

/* Update */

int l_record_recrule_sql_update (i_resource *self, l_record_recrule *rule)
{
  int num;
  char *temp;
  char *query;
  char *site_name_esc;
  char *site_desc_esc;
  char *dev_name_esc;
  char *dev_desc_esc;
  char *obj_name_esc;
  char *obj_desc_esc;
  char *met_name_esc;
  char *met_desc_esc;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_record_recrule_sql_update failed to connect to SQL database"); return -1; }

  /* Create site strings */
  if (rule->site_name)
  {
    temp = i_postgres_escape (rule->site_name);
    asprintf (&site_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->site_desc);
    asprintf (&site_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&site_name_esc, "NULL");
    asprintf (&site_desc_esc, "NULL");
  }

  /* Create dev strings */
  if (rule->dev_name)
  {
    temp = i_postgres_escape (rule->dev_name);
    asprintf (&dev_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->dev_desc);
    asprintf (&dev_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&dev_name_esc, "NULL");
    asprintf (&dev_desc_esc, "NULL");
  }

  /* Create obj strings */
  if (rule->obj_name)
  {
    temp = i_postgres_escape (rule->obj_name);
    asprintf (&obj_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->obj_desc);
    asprintf (&obj_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&obj_name_esc, "NULL");
    asprintf (&obj_desc_esc, "NULL");
  }

  /* Create met strings */
  if (rule->met_name)
  {
    temp = i_postgres_escape (rule->met_name);
    asprintf (&met_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->met_desc);
    asprintf (&met_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&met_name_esc, "NULL");
    asprintf (&met_desc_esc, "NULL");
  }

  /* Create query */
  asprintf (&query, "UPDATE record_recrules SET site_name=%s, site_desc=%s, dev_name=%s, dev_desc=%s, obj_name=%s, obj_desc=%s, met_name=%s, met_desc=%s, recordflag='%i' WHERE id='%li'",
    site_name_esc, site_desc_esc, dev_name_esc, dev_desc_esc, obj_name_esc, obj_desc_esc, met_name_esc, met_desc_esc,
    rule->recordflag, rule->id);
  free (site_name_esc);
  free (site_desc_esc);
  free (dev_name_esc);
  free (dev_desc_esc);
  free (obj_name_esc);
  free (obj_desc_esc);
  free (met_name_esc);
  free (met_desc_esc);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, l_record_recrule_sql_sqlcb, query);
  if (num != 0)
  {
    i_printf (1, "l_record_recrule_sql_update failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

/* Delete */

int l_record_recrule_sql_delete (i_resource *self, long rule_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_record_recrule_sql_delete failed to connect to SQL database"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM record_recrules WHERE id='%li'", rule_id);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, l_record_recrule_sql_sqlcb, query);
  if (num != 0)
  {
    i_printf (1, "l_record_recrule_sql_delete failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

/* Generic callback */

int l_record_recrule_sql_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  char *query = passdata;

  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_record_recrule_sql_sqlcb query '%s' failed", query); }

  /* Invalidate Cache */
  l_record_recrule_sql_invalidate_cache ();

  /* Clean up */
  if (query) free (query);
  
  /* Close conn */  
  i_pg_async_conn_close (conn);

  return 0;
}


/* Load All */

int l_record_recrule_sql_load (i_resource *self, i_metric *met, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  char *cnt_esc;
  char *met_esc;
  char *site_esc;
  char *dev_esc;
  char *obj_esc;
  i_callback *callback;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn) 
  { i_printf (1, "l_record_recrule_sql_load failed to connect to SQL database"); return -1; }

  /* Create query */
  cnt_esc = i_postgres_escape (met->obj->cnt->name_str);
  met_esc = i_postgres_escape (met->name_str);
  site_esc = i_postgres_escape (met->obj->cnt->dev->site->name_str);
  dev_esc = i_postgres_escape (met->obj->cnt->dev->name_str);
  obj_esc = i_postgres_escape (met->obj->name_str);
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, met_name, met_desc, recordflag FROM record_recrules WHERE cnt_name='%s' AND (met_name='%s' OR met_name IS NULL) AND (obj_name='%s' OR obj_name IS NULL) AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY met_name, obj_name, dev_name, site_name ASC",
      cnt_esc, met_esc, obj_esc, dev_esc, site_esc);
    free (obj_esc);
  free (cnt_esc);
  free (met_esc);
  free (site_esc);
  free (dev_esc);

  /* Create callback struct */ 
  callback = i_callback_create ();
  callback->func = cbfunc;
  callback->passdata = passdata;
  
  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, l_record_recrule_sql_load_sqlcb, callback);
  free (query);
  if (num != 0)
  {
    i_printf (1, "l_record_recrule_sql_load failed to execute SELECT command");
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  return 0;
}

int l_record_recrule_sql_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  int y;
  int row_count;
  i_list *list = NULL;
  i_callback *callback = passdata;
  
  /* Check for result */
  if (result && PQresultStatus(result)==PGRES_TUPLES_OK)
  {
    /* Create list */
    list = i_list_create ();
    row_count = PQntuples (result);
    for (y=0; y < row_count; y++)
    {
      char *id_str;
      char *site_name;
      char *site_desc;
      char *dev_name;
      char *dev_desc;
      char *obj_name;
      char *obj_desc;
      char *met_name;
      char *met_desc;
      char *recordflag_str;
      l_record_recrule *rule;

      /* Get values */
      id_str = PQgetvalue (result, y, 0);
      site_name = PQgetvalue (result, y, 1);
      site_desc = PQgetvalue (result, y, 2);
      dev_name = PQgetvalue (result, y, 3);
      dev_desc = PQgetvalue (result, y, 4);
      obj_name = PQgetvalue (result, y, 5);
      obj_desc = PQgetvalue (result, y, 6);
      met_name = PQgetvalue (result, y, 7);
      met_desc = PQgetvalue (result, y, 8);
      recordflag_str = PQgetvalue (result, y, 9);

      /* Rule */
      rule = l_record_recrule_create ();
      if (id_str) rule->id = atol (id_str);
      if (site_name && strlen (site_name) > 0) rule->site_name = strdup (site_name);
      if (site_desc && strlen (site_desc) > 0) rule->site_desc = strdup (site_desc);
      if (dev_name && strlen (dev_name) > 0) rule->dev_name = strdup (dev_name);
      if (dev_desc && strlen (dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
      if (obj_name && strlen (obj_name) > 0) rule->obj_name = strdup (obj_name);
      if (obj_desc && strlen (obj_desc) > 0) rule->obj_desc = strdup (obj_desc);
      if (met_name && strlen (met_name) > 0) rule->met_name = strdup (met_name);
      if (met_desc && strlen (met_desc) > 0) rule->met_desc = strdup (met_desc);
      if (recordflag_str) rule->recordflag = atoi (recordflag_str);

      /* Enqueue */
      i_list_enqueue (list, rule);    
    }
  }

  /* Call callback */
  callback->func (self, list, callback->passdata);

  /* Cleanup */
  i_list_set_destructor (list, l_record_recrule_free);
  i_list_free (list);
  i_callback_free (callback);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Synchronous loading */

sqlite3 *static_recrule_cache = NULL;

void l_record_recrule_sql_invalidate_cache ()
{
  if (static_recrule_cache)
  { 
    sqlite3_close (static_recrule_cache); 
    static_recrule_cache = NULL; 
  }
}

int l_record_recrule_sql_load_cache (i_resource *self)
{
  /* Perform synchronous query to PostgreSQL database to cache
   * all recording rules that would be applicable to any/all 
   * metrics on this device 
   */

  /* Connect */
  PGconn *conn = i_pg_connect_customer (self);
  if (!conn)
  { i_printf (1, "l_record_recrule_sql_load_cache failed to connect to SQL database"); return -1; }

  /* Query */
  char *query;
  char *site_esc = i_postgres_escape (self->hierarchy->site->name_str);
  char *dev_esc = i_postgres_escape (self->hierarchy->dev->name_str);
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, met_name, met_desc, recordflag, cnt_name FROM record_recrules WHERE (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY dev_name, site_name ASC", dev_esc, site_esc);
  free (site_esc);
  free (dev_esc); 

  /* Perform Query and Parse Result */
  PGresult *result = PQexec (conn, query);
  if (result && PQresultStatus(result)==PGRES_TUPLES_OK)
  {
    /* Create cache DB */
    char *sqlerr = NULL;
    int num = sqlite3_open (":memory:", &static_recrule_cache);
    if (num != SQLITE_OK)
    { 
      i_printf (1, "l_record_recrule_sql_load_cache failed to create static_recrule_cache sqlite database"); return -1; 
      PQclear (result);
      i_pg_close (conn);
      free (query);
      return -1;
    }
    char *create = "CREATE TABLE record_recrules (id serial, cnt_name varchar, site_name varchar, site_desc varchar, dev_name varchar, dev_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, recordflag integer)";
    sqlite3_exec (static_recrule_cache, create, NULL, NULL, &sqlerr);

    int row_count = PQntuples (result);
    int y;
    for (y=0; y < row_count; y++)
    {
      char *id_str;
      char *site_name;
      char *site_desc;
      char *dev_name;
      char *dev_desc;
      char *obj_name;
      char *obj_desc;
      char *met_name;
      char *met_desc;
      char *recordflag_str;
      char *cnt_name;

      /* Get values */
      if (PQgetvalue (result, y, 0) && strlen(PQgetvalue (result, y, 0))) asprintf (&id_str, "'%s'", PQgetvalue (result, y, 0));
      else id_str = "NULL";
      if (PQgetvalue (result, y, 1) && strlen(PQgetvalue (result, y, 1))) asprintf (&site_name, "'%s'", PQgetvalue (result, y, 1));
      else site_name = "NULL"; 
      if (PQgetvalue (result, y, 2) && strlen(PQgetvalue (result, y, 2))) asprintf (&site_desc, "'%s'", PQgetvalue (result, y, 2));
      else site_desc = "NULL";
      if (PQgetvalue (result, y, 3) && strlen(PQgetvalue (result, y, 3))) asprintf (&dev_name, "'%s'", PQgetvalue (result, y, 3));
      else dev_name = "NULL";
      if (PQgetvalue (result, y, 4) && strlen(PQgetvalue (result, y, 4))) asprintf (&dev_desc, "'%s'", PQgetvalue (result, y, 4));
      else dev_desc = "NULL";
      if (PQgetvalue (result, y, 5) && strlen(PQgetvalue (result, y, 5))) asprintf (&obj_name, "'%s'", PQgetvalue (result, y, 5));
      else obj_name = "NULL";
      if (PQgetvalue (result, y, 6) && strlen(PQgetvalue (result, y, 6))) asprintf (&obj_desc, "'%s'", PQgetvalue (result, y, 6));
      else obj_desc = "NULL";
      if (PQgetvalue (result, y, 7) && strlen(PQgetvalue (result, y, 7))) asprintf (&met_name, "'%s'", PQgetvalue (result, y, 7));
      else met_name = "NULL";
      if (PQgetvalue (result, y, 8) && strlen(PQgetvalue (result, y, 8))) asprintf (&met_desc, "'%s'", PQgetvalue (result, y, 8));
      else met_desc = "NULL";
      if (PQgetvalue (result, y, 9) && strlen(PQgetvalue (result, y, 9))) asprintf (&recordflag_str, "'%s'", PQgetvalue (result, y, 9));
      else recordflag_str = "NULL";
      if (PQgetvalue (result, y, 10) && strlen(PQgetvalue (result, y, 10))) asprintf (&cnt_name, "'%s'", PQgetvalue (result, y, 10));
      else cnt_name = "NULL";

      /* Create insert query */
      char *insert;
      asprintf (&insert, "INSERT INTO record_recrules (id, cnt_name, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, met_name, met_desc, recordflag) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);",
        id_str, cnt_name, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, met_name, met_desc, recordflag_str);

      /* Insert row into cache */
      sqlerr = NULL;
      sqlite3_exec (static_recrule_cache, insert, NULL, NULL, &sqlerr);
      free (insert);
      if (sqlerr)
      { i_printf (1, "l_record_recrule_sql_load_cache error from sqlite: %s", sqlerr); sqlite3_free (sqlerr); }

    }
  }
  else
  { i_printf (1, "l_record_recrule_sql_load_cache failed execute query '%s'", query); }

  PQclear (result);
  i_pg_close (conn);
  free (query);

  return 0;
}

i_list* l_record_recrule_sql_load_sync (i_resource *self, i_metric *met)
{
  char *query;
  char *cnt_esc;
  char *met_esc;
  char *site_esc;
  char *dev_esc;
  char *obj_esc;
  i_list *list = NULL;

  if (!static_recrule_cache)
  { l_record_recrule_sql_load_cache (self); }

  /* Create query */
  cnt_esc = i_postgres_escape (met->obj->cnt->name_str);
  met_esc = i_postgres_escape (met->name_str);
  site_esc = i_postgres_escape (met->obj->cnt->dev->site->name_str);
  dev_esc = i_postgres_escape (met->obj->cnt->dev->name_str);
  obj_esc = i_postgres_escape (met->obj->name_str);
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, met_name, met_desc, recordflag FROM record_recrules WHERE cnt_name='%s' AND (met_name='%s' OR met_name IS NULL) AND (obj_name='%s' OR obj_name IS NULL) AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY obj_name, dev_name, site_name ASC",
      cnt_esc, met_esc, obj_esc, dev_esc, site_esc);
  free (obj_esc);
  free (cnt_esc);
  free (met_esc);
  free (site_esc);
  free (dev_esc); 

  /* Execute query */
  char **result = NULL;
  int rows = 0;
  int cols = 0;
  char *sqlerr = NULL;
  int num = sqlite3_get_table (static_recrule_cache, query, &result, &rows, &cols, &sqlerr);

  /* Check for result */
  if (num == SQLITE_OK)
  {
    /* Create list */
    list = i_list_create ();
    i_list_set_destructor (list, l_record_recrule_free);

    int y;
    for (y=1; y < (rows+1); y++)
    {
      char *id_str;
      char *site_name;
      char *site_desc;
      char *dev_name;
      char *dev_desc;
      char *obj_name;
      char *obj_desc;
      char *met_name;
      char *met_desc;
      char *recordflag_str;
      l_record_recrule *rule;

      /* Get values */
      id_str = result[0 + (y * cols)];
      site_name = result[1 + (y * cols)];
      site_desc = result[2 + (y * cols)];
      dev_name = result[3 + (y * cols)];
      dev_desc = result[4 + (y * cols)];
      obj_name = result[5 + (y * cols)];
      obj_desc = result[6 + (y * cols)];
      met_name = result[7 + (y * cols)];
      met_desc = result[8 + (y * cols)];
      recordflag_str = result[9 + (y * cols)];

      /* Rule */
      rule = l_record_recrule_create ();
      if (id_str) rule->id = atol (id_str);
      if (site_name && strlen (site_name) > 0) rule->site_name = strdup (site_name);
      if (site_desc && strlen (site_desc) > 0) rule->site_desc = strdup (site_desc);
      if (dev_name && strlen (dev_name) > 0) rule->dev_name = strdup (dev_name);
      if (dev_desc && strlen (dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
      if (obj_name && strlen (obj_name) > 0) rule->obj_name = strdup (obj_name);
      if (obj_name && strlen (obj_name) > 0) rule->obj_name = strdup (obj_name);
      if (met_desc && strlen (met_desc) > 0) rule->met_desc = strdup (met_desc);
      if (met_desc && strlen (met_desc) > 0) rule->met_desc = strdup (met_desc);
      if (recordflag_str) rule->recordflag = atoi (recordflag_str);

      /* Enqueue */
      i_list_enqueue (list, rule);
    }
  }
  else
  { i_printf (1, "l_record_recrule_sql_load_sync failed execute query '%s' against cache (%s)", query, sqlerr); }

  if (result) sqlite3_free_table (result);
  if (sqlerr) sqlite3_free (sqlerr);
  free (query);

  return list;
}

