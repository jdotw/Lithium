#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "induction.h"
#include "list.h"
#include "hashtable.h"
#include "callback.h"
#include "timer.h"
#include "name.h"
#include "entity.h"
#include "hierarchy.h"
#include "site.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "triggerset.h"
#include "postgresql.h"

/** \addtogroup triggerset Trigger Sets
 * @ingroup trigger
 * @{
 */

/* 
 * CEMent Trigggers - Application Rules - SQL Operations
 */

/* Get */

int i_triggerset_apprule_sql_get (i_resource *self, long id, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  i_callback *callback;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_apprule_sql_get failed to connect to SQL database"); return -1; }

  /* Create callback struct */
  callback = i_callback_create ();
  callback->func = cbfunc;
  callback->passdata = passdata;

  /* Execute command */
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, applyflag FROM triggerset_apprules WHERE id='%li'", id);
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_apprule_sql_get_sqlcb, callback);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_apprule_sql_get failed to execute SELECT command");
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  return 0;
}

int i_triggerset_apprule_sql_get_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  i_callback *callback = passdata;
  i_triggerset_apprule *rule = NULL;

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
    char *applyflag_str;

    /* Get values */
    id_str = PQgetvalue (result, 0, 0);
    site_name = PQgetvalue (result, 0, 1);
    site_desc = PQgetvalue (result, 0, 2);
    dev_name = PQgetvalue (result, 0, 3);
    dev_desc = PQgetvalue (result, 0, 4);
    obj_name = PQgetvalue (result, 0, 5);
    obj_desc = PQgetvalue (result, 0, 6);
    applyflag_str = PQgetvalue (result, 0, 7);

    /* Rule */
    rule = i_triggerset_apprule_create ();
    if (id_str) rule->id = atol (id_str);
    if (site_name && strlen(site_name) > 0) rule->site_name = strdup (site_name);
    if (site_desc && strlen(site_desc) > 0) rule->site_desc = strdup (site_desc);
    if (dev_name && strlen(dev_name) > 0) rule->dev_name = strdup (dev_name);
    if (dev_desc && strlen(dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
    if (obj_name && strlen(obj_name) > 0) rule->obj_name = strdup (obj_name);
    if (obj_desc && strlen(obj_name) > 0) rule->obj_desc = strdup (obj_desc);
    if (applyflag_str) rule->applyflag = atoi (applyflag_str);
  }

  /* Call callback */
  callback->func (self, rule, callback->passdata);

  /* Cleanup */
  i_triggerset_apprule_free (rule);
  i_callback_free (callback);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Insert */

int i_triggerset_apprule_sql_insert (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_apprule *rule)
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
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_apprule_sql_insert failed to connect to SQL database"); return -1; }

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

  asprintf (&query, "INSERT INTO triggerset_apprules (cnt_name, tset_name, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, applyflag) VALUES ('%s', '%s', %s, %s, %s, %s, %s, %s, '%i')",
    obj->cnt->name_str, tset->name_str,
    site_name_esc, site_desc_esc, dev_name_esc, dev_desc_esc, obj_name_esc, obj_desc_esc, rule->applyflag);
  free (site_name_esc);
  free (site_desc_esc);
  free (dev_name_esc);
  free (dev_desc_esc);
  free (obj_name_esc);
  free (obj_desc_esc);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_apprule_sql_sqlcb, tset);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_apprule_sql_insert failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }
  free (query);

  return 0;
}

/* Update */

int i_triggerset_apprule_sql_update (i_resource *self, i_triggerset_apprule *rule, i_triggerset *tset)
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
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_apprule_sql_update failed to connect to SQL database"); return -1; }

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

  /* Create query */
  asprintf (&query, "UPDATE triggerset_apprules SET site_name=%s, site_desc=%s, dev_name=%s, dev_desc=%s, obj_name=%s, obj_desc=%s, applyflag='%i' WHERE id='%li'",
    site_name_esc, site_desc_esc, dev_name_esc, dev_desc_esc, obj_name_esc, obj_desc_esc, 
    rule->applyflag, rule->id);
  free (site_name_esc);
  free (site_desc_esc);
  free (dev_name_esc);
  free (dev_desc_esc);
  free (obj_name_esc);
  free (obj_desc_esc);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_apprule_sql_sqlcb, tset);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_apprule_sql_update failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }
  free (query);

  return 0;
}

/* Delete */

int i_triggerset_apprule_sql_delete (i_resource *self, long rule_id, i_triggerset *tset)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_apprule_sql_delete failed to connect to SQL database"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM triggerset_apprules WHERE id='%li'", rule_id);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_apprule_sql_sqlcb, tset);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_apprule_sql_delete failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }
  free (query);

  return 0;
}

int i_triggerset_apprule_sql_delete_specific (i_resource *self, i_object *obj, i_triggerset *tset)
{
  /* Removes all fully-specific rules for this object
   * reltating to this tset
   */
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_apprule_sql_delete failed to connect to SQL database"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM triggerset_apprules WHERE cnt_name='%s' AND tset_name='%s' AND site_name='%s' AND dev_name='%s' AND obj_name='%s'",
    obj->cnt->name_str, tset->name_str, obj->cnt->dev->site->name_str,
    obj->cnt->dev->name_str, obj->name_str);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_apprule_sql_sqlcb, tset);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_apprule_sql_delete_specific failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }
  free (query);

  return 0;
}

int i_triggerset_apprule_sql_delete_wildcard (i_resource *self, i_entity *ent)
{
  /* Removes all rules that apply to the given entity and
   * any entity below it 
   */
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_apprule_sql_delete_wildcard failed to connect to SQL database"); return -1; }

  /* Create query */
  i_entity_descriptor *entdesc = i_entity_descriptor_struct_static (ent);
  if (ent->ent_type == ENT_CUSTOMER)
  { asprintf (&query, "DELETE FROM triggerset_apprules"); }
  else if (ent->ent_type == ENT_SITE)
  { asprintf (&query, "DELETE FROM triggerset_apprules WHERE site_name='%s'", entdesc->site_name); }
  else if (ent->ent_type == ENT_DEVICE)
  { asprintf (&query, "DELETE FROM triggerset_apprules WHERE site_name='%s' AND dev_name='%s'", 
    entdesc->site_name, entdesc->dev_name); }
  else if (ent->ent_type == ENT_CONTAINER)
  { asprintf (&query, "DELETE FROM triggerset_apprules WHERE site_name='%s' AND dev_name='%s' AND cnt_name='%s'", 
    entdesc->site_name, entdesc->dev_name, entdesc->cnt_name); }
  else if (ent->ent_type == ENT_OBJECT)
  { asprintf (&query, "DELETE FROM triggerset_apprules WHERE site_name='%s' AND dev_name='%s' AND cnt_name='%s' AND obj_name='%s'", 
    entdesc->site_name, entdesc->dev_name, entdesc->cnt_name, entdesc->obj_name); }
  else if (ent->ent_type == ENT_METRIC)
  { asprintf (&query, "DELETE FROM triggerset_apprules WHERE site_name='%s' AND dev_name='%s' AND cnt_name='%s' AND obj_name='%s' AND met_name='%s'", 
    entdesc->site_name, entdesc->dev_name, entdesc->cnt_name, entdesc->obj_name, entdesc->met_name); }

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_apprule_sql_sqlcb, NULL);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_apprule_sql_delete_wildcard failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }
  free (query);

  return 0;
}

/* Generic callback */

int i_triggerset_apprule_sql_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  i_triggerset *tset = passdata;

  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "i_triggerset_apprule_sql_sqlcb query failed"); }

  /* Close conn */  
  i_pg_async_conn_close (conn);

  /* Invalidate cache */
  i_triggerset_apprule_sql_invalidate_cache ();

  /* Re-Evaluate */
  if (tset && tset->cnt)
  {
    i_object *obj;
    for (i_list_move_head(tset->cnt->obj_list); (obj=i_list_restore(tset->cnt->obj_list))!=NULL; i_list_move_next(tset->cnt->obj_list))
    {
      int num = i_triggerset_evalapprules_allsets (self, obj);
      if (num != 0)
      { i_printf (1, "i_triggerset_apprule_sql_sqlcb failed to evaluate apprules for triggerset"); }
    }
  }
  
  return 0;
}


/* Load All */

int i_triggerset_apprule_sql_load (i_resource *self, i_triggerset *tset, i_object *obj, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  char *cnt_esc;
  char *tset_esc;
  char *site_esc;
  char *dev_esc;
  char *obj_esc;
  i_callback *callback;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn) 
  { i_printf (1, "i_triggerset_apprule_sql_load failed to connect to SQL database"); return -1; }

  /* Create query */
  cnt_esc = i_postgres_escape (tset->cnt->name_str);
  tset_esc = i_postgres_escape (tset->name_str);
  site_esc = i_postgres_escape (tset->cnt->dev->site->name_str);
  dev_esc = i_postgres_escape (tset->cnt->dev->name_str);
  if (obj) 
  {
    obj_esc = i_postgres_escape (obj->name_str);
    asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, applyflag FROM triggerset_apprules WHERE cnt_name='%s' AND tset_name='%s' AND (obj_name='%s' OR obj_name IS NULL) AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY site_name, dev_name, obj_name ASC",
      cnt_esc, tset_esc, obj_esc, dev_esc, site_esc);
    free (obj_esc);
  }
  else
  {
    asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, applyflag FROM triggerset_apprules WHERE cnt_name='%s' AND tset_name='%s' AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY site_name, dev_name, obj_name ASC",
      cnt_esc, tset_esc, dev_esc, site_esc);
  }
  free (cnt_esc);
  free (tset_esc);
  free (site_esc);
  free (dev_esc);

  /* Create callback struct */ 
  callback = i_callback_create ();
  callback->func = cbfunc;
  callback->passdata = passdata;
  
  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_apprule_sql_load_sqlcb, callback);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_apprule_sql_load failed to execute SELECT command");
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  return 0;
}

int i_triggerset_apprule_sql_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
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
      char *applyflag_str;
      i_triggerset_apprule *rule;

      /* Get values */
      id_str = PQgetvalue (result, y, 0);
      site_name = PQgetvalue (result, y, 1);
      site_desc = PQgetvalue (result, y, 2);
      dev_name = PQgetvalue (result, y, 3);
      dev_desc = PQgetvalue (result, y, 4);
      obj_name = PQgetvalue (result, y, 5);
      obj_desc = PQgetvalue (result, y, 6);
      applyflag_str = PQgetvalue (result, y, 7);

      /* Rule */
      rule = i_triggerset_apprule_create ();
      if (id_str) rule->id = atol (id_str);
      if (site_name && strlen (site_name) > 0) rule->site_name = strdup (site_name);
      if (site_desc && strlen (site_desc) > 0) rule->site_desc = strdup (site_desc);
      if (dev_name && strlen (dev_name) > 0) rule->dev_name = strdup (dev_name);
      if (dev_desc && strlen (dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
      if (obj_name && strlen (obj_name) > 0) rule->obj_name = strdup (obj_name);
      if (obj_desc && strlen (obj_desc) > 0) rule->obj_desc = strdup (obj_desc);
      if (applyflag_str) rule->applyflag = atoi (applyflag_str);

      /* Enqueue */
      i_list_enqueue (list, rule);    
    }
  }

  /* Call callback */
  callback->func (self, list, callback->passdata);

  /* Cleanup */
  i_list_set_destructor (list, i_triggerset_apprule_free);
  i_list_free (list);
  i_callback_free (callback);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Synchronous loading */

sqlite3 *static_apprule_cache = NULL;

void i_triggerset_apprule_sql_invalidate_cache ()
{
  i_printf (1, "i_triggerset_apprule_sql_invalidate_cache %p", static_apprule_cache);

  if (static_apprule_cache)
  { 
    sqlite3_close (static_apprule_cache); 
    static_apprule_cache = NULL; 
  }
}

int i_triggerset_apprule_sql_load_cache (i_resource *self)
{
  /* Perform a synchronous PostgreSQL query to cache all
   * apprules that would be applicable to any/all triggers
   * present on this device
   */

  /* Connect */
  PGconn *conn = i_pg_connect_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_apprule_sql_load_sync failed to connect to SQL database"); return -1; }

  /* Create query */
  char *query;
  char *site_esc = i_postgres_escape (self->hierarchy->site->name_str);
  char *dev_esc = i_postgres_escape (self->hierarchy->dev->name_str);
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, tset_name, tset_desc, applyflag FROM triggerset_apprules WHERE (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY dev_name, site_name ASC", dev_esc, site_esc);
  free (site_esc);
  free (dev_esc);

  /* Execute query */
  PGresult *result = PQexec (conn, query);

  /* Check for result */
  if (result && PQresultStatus(result)==PGRES_TUPLES_OK)
  {
    /* Create cache DB */
    char *sqlerr = NULL;
    int num = sqlite3_open (":memory:", &static_apprule_cache);    
    if (num != SQLITE_OK)
    {
      i_printf (1, "i_triggerset_apprule_sql_load_cache failed to create static_apprule_cache sqlite database"); return -1;
      PQclear (result);
      i_pg_close (conn);
      free (query);
      return -1;
    }
    char *create = "CREATE TABLE triggerset_apprules (id serial, cnt_name varchar, cnt_desc varchar, tset_name varchar, tset_desc varchar, site_name varchar, site_desc varchar, dev_name varchar, dev_desc varchar, obj_name varchar, obj_desc varchar, applyflag integer)";
    sqlite3_exec (static_apprule_cache, create, NULL, NULL, &sqlerr);

    /* Parse results */
    int row_count = PQntuples (result);
    int y;
    for (y=0; y < row_count; y++)
    {
      char *id_str;
      char *site_name;
      char *site_desc;
      char *dev_name;
      char *dev_desc;
      char *cnt_name;
      char *cnt_desc;
      char *obj_name;
      char *obj_desc;
      char *tset_name;
      char *tset_desc;
      char *applyflag_str;

      /* Get values */
      if (strlen(PQgetvalue (result, y, 0))) asprintf (&id_str, "'%s'", PQgetvalue (result, y, 0));
      else id_str = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 1))) asprintf (&site_name, "'%s'", PQgetvalue (result, y, 1));
      else site_name = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 2))) asprintf (&site_desc, "'%s'", PQgetvalue (result, y, 2));
      else site_desc = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 3))) asprintf (&dev_name, "'%s'", PQgetvalue (result, y, 3));
      else dev_name = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 4))) asprintf (&dev_desc, "'%s'", PQgetvalue (result, y, 4));
      else dev_desc = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 5))) asprintf (&cnt_name, "'%s'", PQgetvalue (result, y, 5));
      else cnt_name = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 6))) asprintf (&cnt_desc, "'%s'", PQgetvalue (result, y, 6));
      else cnt_desc = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 7))) asprintf (&obj_name, "'%s'", PQgetvalue (result, y, 7));
      else obj_name = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 8))) asprintf (&obj_desc, "'%s'", PQgetvalue (result, y, 8));
      else obj_desc = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 9))) asprintf (&tset_name, "'%s'", PQgetvalue (result, y, 9));
      else tset_name = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 10))) asprintf (&tset_desc, "'%s'", PQgetvalue (result, y, 10));
      else tset_desc = strdup("NULL");
      if (strlen(PQgetvalue (result, y, 11))) asprintf (&applyflag_str, "'%s'", PQgetvalue (result, y, 11));
      else applyflag_str = strdup("NULL");

      /* Create insert query */
      char *insert;
      asprintf (&insert, "INSERT INTO triggerset_apprules (id, cnt_name, cnt_desc, tset_name, tset_desc, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, applyflag) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);",
        id_str, cnt_name, cnt_desc, tset_name, tset_desc, site_name, site_desc, 
        dev_name, dev_desc, obj_name, obj_desc, applyflag_str);
      free (id_str);
      free (site_name);
      free (site_desc);
      free (dev_name);
      free (dev_desc);
      free (cnt_name);
      free (cnt_desc);
      free (obj_name);
      free (obj_desc);
      free (tset_name);
      free (tset_desc);
      free (applyflag_str);

      /* Insert row into cache */
      sqlerr = NULL;
      sqlite3_exec (static_apprule_cache, insert, NULL, NULL, &sqlerr);
      free (insert);
      if (sqlerr)
      { i_printf (1, "i_triggerset_apprule_sql_load_cache error from sqlite: %s", sqlerr); sqlite3_free (sqlerr); }
    }
  }
  else
  { i_printf (1, "i_triggerset_apprule_sql_load_cache failed execute query '%s'", query); }

  PQclear (result);
  i_pg_close (conn);
  free (query);

  return 0;
}

i_list* i_triggerset_apprule_sql_load_sync (i_resource *self, i_triggerset *tset, i_object *obj)
{
  int y;
  char *query;
  char *cnt_esc;
  char *tset_esc;
  char *site_esc;
  char *dev_esc;
  char *obj_esc;
  i_list *list = NULL;

  if (!static_apprule_cache)
  { i_triggerset_apprule_sql_load_cache (self); }

  /* Create query */
  cnt_esc = i_postgres_escape (tset->cnt->name_str);
  tset_esc = i_postgres_escape (tset->name_str);
  site_esc = i_postgres_escape (tset->cnt->dev->site->name_str);
  dev_esc = i_postgres_escape (tset->cnt->dev->name_str);
  if (obj)
  {
    obj_esc = i_postgres_escape (obj->name_str);
    asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, applyflag FROM triggerset_apprules WHERE cnt_name='%s' AND tset_name='%s' AND (obj_name='%s' OR obj_name IS NULL) AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY obj_name, dev_name, site_name ASC",
      cnt_esc, tset_esc, obj_esc, dev_esc, site_esc);
    free (obj_esc);
  }
  else
  {
    asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, applyflag FROM triggerset_apprules WHERE cnt_name='%s' AND tset_name='%s' AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY obj_name, dev_name, site_name ASC",
      cnt_esc, tset_esc, dev_esc, site_esc);
  }
  free (cnt_esc);
  free (tset_esc);
  free (site_esc);
  free (dev_esc); 

  /* Execute query */
  char **result = NULL;
  int rows = 0;
  int cols = 0;
  char *sqlerr = NULL;
  int num = sqlite3_get_table (static_apprule_cache, query, &result, &rows, &cols, &sqlerr);

  /* Check for result */
  if (num == SQLITE_OK)
  {
    /* Create list */
    list = i_list_create ();
    i_list_set_destructor (list, i_triggerset_apprule_free);

    for (y=1; y < (rows+1); y++)
    {
      char *id_str;
      char *site_name;
      char *site_desc;
      char *dev_name;
      char *dev_desc;
      char *obj_name;
      char *obj_desc;
      char *applyflag_str;
      i_triggerset_apprule *rule;

      /* Get values */
      id_str = result[0 + (y * cols)];
      site_name = result[1 + (y * cols)];
      site_desc = result[2 + (y * cols)];
      dev_name = result[3 + (y * cols)];
      dev_desc = result[4 + (y * cols)];
      obj_name = result[5 + (y * cols)];
      obj_desc = result[6 + (y * cols)];
      applyflag_str = result[7 + (y * cols)];

      /* Rule */
      rule = i_triggerset_apprule_create ();
      if (id_str) rule->id = atol (id_str);
      if (site_name && strlen (site_name) > 0) rule->site_name = strdup (site_name);
      if (site_desc && strlen (site_desc) > 0) rule->site_desc = strdup (site_desc);
      if (dev_name && strlen (dev_name) > 0) rule->dev_name = strdup (dev_name);
      if (dev_desc && strlen (dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
      if (obj_name && strlen (obj_name) > 0) rule->obj_name = strdup (obj_name);
      if (obj_desc && strlen (obj_desc) > 0) rule->obj_desc = strdup (obj_desc);
      if (applyflag_str) rule->applyflag = atoi (applyflag_str);

      /* Enqueue */
      i_list_enqueue (list, rule);
    }
  }
  else
  { i_printf (1, "i_triggerset_apprule_sql_load_sync failed execute query '%s'", query); }

  if (result) sqlite3_free_table (result);
  if (sqlerr) sqlite3_free (sqlerr);
  free (query);

  return list;
}

/* @} */
