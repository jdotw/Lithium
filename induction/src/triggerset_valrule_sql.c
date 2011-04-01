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
 * CEMent Trigggers - Value Rules - SQL Operations
 */

/* Get */

int i_triggerset_valrule_sql_get (i_resource *self, long id, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  i_callback *callback;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_valrule_sql_get failed to connect to SQL database"); return -1; }

  /* Create callback struct */
  callback = i_callback_create ();
  callback->func = cbfunc;
  callback->passdata = passdata;

  /* Execute command */
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, trg_name, trg_desc, trgtype, xval, yval, duration, adminstate FROM triggerset_valrules WHERE id='%li'", id);
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_valrule_sql_get_sqlcb, callback);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_valrule_sql_get failed to execute SELECT command");
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  return 0;
}

int i_triggerset_valrule_sql_get_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  i_callback *callback = passdata;
  i_triggerset_valrule *rule = NULL;

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
    char *trg_name;
    char *trg_desc;
    char *trgtype_str;
    char *xval_str;
    char *yval_str;
    char *duration_str;
    char *adminstate_str;

    /* Get values */
    id_str = PQgetvalue (result, 0, 0);
    site_name = PQgetvalue (result, 0, 1);
    site_desc = PQgetvalue (result, 0, 2);
    dev_name = PQgetvalue (result, 0, 3);
    dev_desc = PQgetvalue (result, 0, 4);
    obj_name = PQgetvalue (result, 0, 5);
    obj_desc = PQgetvalue (result, 0, 6);
    trg_name = PQgetvalue (result, 0, 7);
    trg_desc = PQgetvalue (result, 0, 8);
    trgtype_str = PQgetvalue (result, 0, 9);
    xval_str = PQgetvalue (result, 0, 10);
    yval_str = PQgetvalue (result, 0, 11);
    duration_str = PQgetvalue (result, 0, 12);
    adminstate_str = PQgetvalue (result, 0, 13);

    /* Rule */
    rule = i_triggerset_valrule_create ();
    if (id_str) rule->id = atol (id_str);
    if (site_name && strlen(site_name) > 0) rule->site_name = strdup (site_name);
    if (site_desc && strlen(site_desc) > 0) rule->site_desc = strdup (site_desc);
    if (dev_name && strlen(dev_name) > 0) rule->dev_name = strdup (dev_name);
    if (dev_desc && strlen(dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
    if (obj_name && strlen(obj_name) > 0) rule->obj_name = strdup (obj_name);
    if (obj_desc && strlen(obj_desc) > 0) rule->obj_desc = strdup (obj_desc);
    if (trg_name && strlen(trg_name) > 0) rule->trg_name = strdup (trg_name);
    if (trg_desc && strlen(trg_desc) > 0) rule->trg_desc = strdup (trg_desc);
    if (xval_str && strlen(xval_str) > 0) rule->xval_str = strdup (xval_str);
    if (yval_str && strlen(yval_str) > 0) rule->yval_str = strdup (yval_str);
    if (duration_str && strlen(duration_str) > 0) rule->duration_sec = atol(duration_str);
    if (trgtype_str) rule->trg_type = atoi (trgtype_str);
    if (adminstate_str) rule->adminstate = atoi (adminstate_str);
  }

  /* Call callback */
  callback->func (self, rule, callback->passdata);

  /* Cleanup */
  i_triggerset_valrule_free (rule);
  i_callback_free (callback);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Insert */

int i_triggerset_valrule_sql_insert (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_valrule *rule)
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
  char *trg_name_esc;
  char *trg_desc_esc;
  char *xval_esc;
  char *yval_esc;
  i_pg_async_conn *conn;
  
  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_valrule_sql_insert failed to connect to SQL database"); return -1; }
  
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
  
  /* Create trg strings */
  if (rule->trg_name)
  {
    temp = i_postgres_escape (rule->trg_name);
    asprintf (&trg_name_esc, "'%s'", temp);
    free (temp);
    temp = i_postgres_escape (rule->trg_desc);
    asprintf (&trg_desc_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&trg_name_esc, "NULL");
    asprintf (&trg_desc_esc, "NULL");
  }

  /* Create xval strings */
  if (rule->xval_str)
  {
    temp = i_postgres_escape (rule->xval_str);
    asprintf (&xval_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&xval_esc, "NULL");
  }
  
  /* Create yval strings */
  if (rule->yval_str)
  {
    temp = i_postgres_escape (rule->yval_str);
    asprintf (&yval_esc, "'%s'", temp);
    free (temp);
  } 
  else
  { 
    asprintf (&yval_esc, "NULL");
  } 

  asprintf (&query, "INSERT INTO triggerset_valrules (cnt_name, tset_name, trg_name, trg_desc, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, trgtype, xval, yval, duration, adminstate) VALUES ('%s', '%s', %s, %s, %s, %s, %s, %s, %s, %s, '%i', %s, %s, '%li', '%i')",
    obj->cnt->name_str, tset->name_str, trg_name_esc, trg_desc_esc, 
    site_name_esc, site_desc_esc, dev_name_esc, dev_desc_esc, obj_name_esc, obj_desc_esc, 
    rule->trg_type, xval_esc, yval_esc, rule->duration_sec, rule->adminstate);
  free (site_name_esc);
  free (site_desc_esc);
  free (dev_name_esc);
  free (dev_desc_esc);
  free (obj_name_esc);
  free (obj_desc_esc);
  free (trg_name_esc);
  free (trg_desc_esc);
  free (xval_esc);
  free (yval_esc);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_valrule_sql_sqlcb, tset);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_valrule_sql_insert failed to execute");
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}


/* Update */

int i_triggerset_valrule_sql_update (i_resource *self, i_triggerset *tset, i_triggerset_valrule *rule)
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
  char *xval_esc;
  char *yval_esc;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_valrule_sql_update failed to connect to SQL database"); return -1; }

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

  /* Create xval strings */
  if (rule->xval_str)
  {
    temp = i_postgres_escape (rule->xval_str);
    asprintf (&xval_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&xval_esc, "NULL");
  }

  /* Create yval strings */
  if (rule->yval_str)
  {
    temp = i_postgres_escape (rule->yval_str);
    asprintf (&yval_esc, "'%s'", temp);
    free (temp);
  }
  else
  {
    asprintf (&yval_esc, "NULL");
  }

  /* Create query */
  asprintf (&query, "UPDATE triggerset_valrules SET site_name=%s, site_desc=%s, dev_name=%s, dev_desc=%s, obj_name=%s, obj_desc=%s, trgtype='%i', xval=%s, yval=%s, duration='%li', adminstate='%i' WHERE id='%li'",
    site_name_esc, site_desc_esc, dev_name_esc, dev_desc_esc, 
    obj_name_esc, obj_desc_esc, rule->trg_type,
    xval_esc, yval_esc, rule->duration_sec, rule->adminstate, rule->id);
  free (site_name_esc);
  free (site_desc_esc);
  free (dev_name_esc);
  free (dev_desc_esc);
  free (obj_name_esc);
  free (obj_desc_esc);
  free (xval_esc);
  free (yval_esc);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_valrule_sql_sqlcb, tset);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_valrule_sql_update failed to execute");
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

/* Delete */

int i_triggerset_valrule_sql_delete (i_resource *self, i_triggerset *tset, long rule_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_valrule_sql_delete failed to connect to SQL database"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM triggerset_valrules WHERE id='%li'", rule_id);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_valrule_sql_sqlcb, tset);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_valrule_sql_delete failed to execute");
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

int i_triggerset_valrule_sql_delete_specific (i_resource *self, i_object *obj, i_triggerset *tset, char *trg_name)
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
  { i_printf (1, "i_triggerset_valrule_sql_delete failed to connect to SQL database"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM triggerset_valrules WHERE cnt_name='%s' AND tset_name='%s' AND site_name='%s' AND dev_name='%s' AND obj_name='%s' AND trg_name='%s'",
    obj->cnt->name_str, tset->name_str, obj->cnt->dev->site->name_str,
    obj->cnt->dev->name_str, obj->name_str, trg_name);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_valrule_sql_sqlcb, tset);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_valrule_sql_delete_specific failed to execute");
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

int i_triggerset_valrule_sql_delete_exclusive (i_resource *self, i_object *obj, i_triggerset *tset, i_triggerset_valrule *rule)
{
  /* Deletes all other rules to ensure that the supplied rule
   * will take affect for its given scope. 
   * 
   * The supplied rule must *not* be deleted by the delete statement; 
   * the supplied rule may or may not have an ID
   */
  
  char *tmp;

  /* Connect */
  i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_valrule_sql_delete_exclusive failed to connect to SQL database"); return -1; }

  /* Create basic query (will delete all for the given cnt/tset/trg) */
  char *query;
  asprintf(&query, "DELETE FROM triggerset_valrules WHERE cnt_name='%s' AND tset_name='%s' AND trg_name='%s'", obj->cnt->name_str, tset->name_str, rule->trg_name);

  /* Check if the supplied rule has an ID, if so, we must avoid
   * deleting it 
   */
  if (rule->id != 0)
  {
    asprintf(&tmp, "%s AND id != '%li'", query, rule->id);
    free(query);
    query = tmp;
  }

  /* Add filters for site/dev/obj specifics 
   *
   * This statement should only delete rules that are AS-SPECIFIC or MORE specific, 
   * that is it should delete rows where the column is NOT NULL if it is NULL in the rule
   * or if the rule is a direct match for the specificity of the supplied rule
   */

  /* Add the query parameters needed to delete the AS-SPECIFIC rules */
  char *site_match_str = strdup("IS NULL");
  char *dev_match_str = strdup("IS NULL");
  char *obj_match_str = strdup("IS NULL");
  if (rule->site_name) { free(site_match_str); asprintf(&site_match_str, "= '%s'", rule->site_name); }
  if (rule->dev_name) { free(dev_match_str); asprintf(&dev_match_str, "= '%s'", rule->dev_name); }
  if (rule->obj_name) { free(obj_match_str); asprintf(&obj_match_str, "= '%s'", rule->obj_name); }
  asprintf(&tmp, "%s AND ((site_name %s AND dev_name %s AND obj_name %s)", query, site_match_str, dev_match_str, obj_match_str);
  free(query);
  query = tmp;

  /* Add the query parameters needed to delete MORE-SPECIFIC rules */
  if (!rule->site_name || !rule->dev_name || !rule->obj_name)
  {
    /* There's atleast one wildcard criteria on the rule, so selective filters
     * should be used to only delete rules more specific than this rule. That
     * is, where the supplied rule has a wildcard, this query should delete
     * any rule that has a specific value set
     */

    asprintf (&tmp, "%s OR (", query);
    free (query);
    query = tmp;

    char *and_condition = "";

    /* If there's no site specific (all sites), delete any site-specific rule */
    if (!rule->site_name) 
    {
      asprintf(&tmp, "%s %s site_name IS NOT NULL", query, and_condition);
      free(query);
      query = tmp;
      and_condition = "AND";
    }

    /* If there's no device (all devices), delete any dev-specific rule */
    if (!rule->dev_name)
    {
      asprintf(&tmp, "%s %s dev_name IS NOT NULL", query, and_condition);
      free(query);
      query = tmp;
      and_condition = "AND";
    }

    /* If there's no object (all objects), delete any obj-specific rule */
    if (!rule->obj_name)
    {
      asprintf(&tmp, "%s %s obj_name IS NOT NULL", query, and_condition);
      free(query);
      query = tmp;
      and_condition = "AND";
    }
    
    asprintf (&tmp, "%s)", query);
    free (query);
    query = tmp;

  }
  else
  {
    /* There rule is the least specific type (all objects, all sites, all
     * device). Do not add any further criteria in order to delete all other
     * rules present for this cnt/tset/trg
     */
  }
  
  asprintf(&tmp, "%s)", query);
  free(query);
  query = tmp;

  /* DEBUG */
  i_printf(1, "i_triggerset_valrule_sql_delete_exclusive QUERY: %s", query);
  /* END DEBUG */

  /* Execute command */
  int num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_valrule_sql_sqlcb, tset);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_valrule_sql_delete_exclusive failed to execute");
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

int i_triggerset_valrule_sql_delete_wildcard (i_resource *self, i_entity *ent)
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
  { i_printf (1, "i_triggerset_valrule_sql_delete_wildcard failed to connect to SQL database"); return -1; }
  
  /* Create query */
  i_entity_descriptor *entdesc = i_entity_descriptor_struct_static (ent);
  if (ent->ent_type == ENT_CUSTOMER)
  { asprintf (&query, "DELETE FROM triggerset_valrules"); }
  else if (ent->ent_type == ENT_SITE)
  { asprintf (&query, "DELETE FROM triggerset_valrules WHERE site_name='%s'", entdesc->site_name); }
  else if (ent->ent_type == ENT_DEVICE)
  { asprintf (&query, "DELETE FROM triggerset_valrules WHERE site_name='%s' AND dev_name='%s'",
    entdesc->site_name, entdesc->dev_name); }
  else if (ent->ent_type == ENT_CONTAINER)
  { asprintf (&query, "DELETE FROM triggerset_valrules WHERE site_name='%s' AND dev_name='%s' AND cnt_name='%s'",
    entdesc->site_name, entdesc->dev_name, entdesc->cnt_name); }
  else if (ent->ent_type == ENT_OBJECT)
  { asprintf (&query, "DELETE FROM triggerset_valrules WHERE site_name='%s' AND dev_name='%s' AND cnt_name='%s' AND obj_name='%s'",
    entdesc->site_name, entdesc->dev_name, entdesc->cnt_name, entdesc->obj_name); }
  else if (ent->ent_type == ENT_METRIC)
  { asprintf (&query, "DELETE FROM triggerset_valrules WHERE site_name='%s' AND dev_name='%s' AND cnt_name='%s' AND obj_name='%s' AND met_name='%s'",
    entdesc->site_name, entdesc->dev_name, entdesc->cnt_name, entdesc->obj_name, entdesc->met_name); }

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, query, 0, i_triggerset_valrule_sql_sqlcb, NULL);
  if (num != 0)
  {
    i_printf (1, "i_triggerset_valrule_sql_delete_wildcard failed to execute '%s'", query);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }
  free (query);

  return 0;
}

/* Generic callback */

int i_triggerset_valrule_sql_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  i_triggerset *tset = passdata;

  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "i_triggerset_valrule_sql_sqlcb query failed"); }

  /* Invalidate Cache */
  i_triggerset_valrule_sql_invalidate_cache ();

  /* Re-Evaluate */
  if (tset && tset->cnt)
  {
    i_object *obj;
    for (i_list_move_head(tset->cnt->obj_list); (obj=i_list_restore(tset->cnt->obj_list))!=NULL; i_list_move_next(tset->cnt->obj_list))
    {
      int num = i_triggerset_evalvalrules (self, obj, tset);
      if (num != 0)
      { i_printf (1, "i_triggerset_valrule_sql_sqlcb failed to evaluate valrules for triggerset"); }

      i_metric *met = (i_metric *) i_entity_child_get (ENTITY(obj), tset->metname_str);
      if (met) 
      { i_trigger_process_all (self, met); }
    }
  }
  
  /* Close conn */  
  i_pg_async_conn_close (conn);

  return 0;
}

/* Syncrhonous Loading */

sqlite3 *static_valrule_cache = NULL;

void i_triggerset_valrule_sql_invalidate_cache ()
{
  if (static_valrule_cache)
  { 
    sqlite3_close (static_valrule_cache); 
    static_valrule_cache = NULL; 
  }
}

int i_triggerset_valrule_sql_load_cache (i_resource *self)
{
  /* Perform a synchronous query to PostgreSQL database to
   * cache all tset value rules that would be applicable to any/all 
   * triggers on this device
   */

  /* Connect */
  PGconn *conn = i_pg_connect_customer (self);
  if (!conn)
  { i_printf (1, "i_triggerset_valrule_sql_load_cache failed to connect to SQL database"); return -1; }

  /* Create query */
  char *query;
  char *site_esc = i_postgres_escape (self->hierarchy->site->name_str);
  char *dev_esc = i_postgres_escape (self->hierarchy->dev->name_str);
  asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, trg_name, trg_desc, trgtype, xval, yval, duration, adminstate, cnt_name, cnt_desc, tset_name, tset_desc FROM triggerset_valrules WHERE (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY dev_name, site_name ASC;", dev_esc, site_esc);
  free (site_esc);
  free (dev_esc);

  /* Execute query */
  PGresult *result = PQexec (conn, query);

  /* Check for result */
  if (result && PQresultStatus(result)==PGRES_TUPLES_OK)
  {
    /* Create cache DB */
    char *sqlerr = NULL;
    int num = sqlite3_open (":memory:", &static_valrule_cache);
    if (num != SQLITE_OK)
    {
      i_printf (1, "i_triggerset_valrule_sql_load_cache failed to create static_recrule_cache sqlite database"); return -1;
      PQclear (result);
      i_pg_close (conn);
      free (query);
      return -1;
    }
    char *create = "CREATE TABLE triggerset_valrules (id serial, cnt_name varchar, cnt_desc varchar, tset_name varchar, tset_desc varchar, trg_name varchar, trg_desc varchar, site_name varchar, site_desc varchar, dev_name varchar, dev_desc varchar, obj_name varchar, obj_desc varchar, trgtype integer, xval varchar, yval varchar, duration integer, adminstate integer)";
    sqlite3_exec (static_valrule_cache, create, NULL, NULL, &sqlerr);

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
      char *obj_name;
      char *obj_desc;
      char *trg_name;
      char *trg_desc;
      char *trgtype_str;
      char *xval_str;
      char *yval_str;
      char *duration_str;
      char *adminstate_str;
      char *cnt_name;
      char *cnt_desc;
      char *tset_name;
      char *tset_desc;

      /* Get values */
      if (strlen(PQgetvalue (result, y, 0))) asprintf (&id_str, "'%s'", PQgetvalue (result, y, 0));
      else id_str = "NULL";
      if (strlen(PQgetvalue (result, y, 1))) asprintf (&site_name, "'%s'", PQgetvalue (result, y, 1));
      else site_name = "NULL";
      if (strlen(PQgetvalue (result, y, 2))) asprintf (&site_desc, "'%s'", PQgetvalue (result, y, 2));
      else site_desc = "NULL";
      if (strlen(PQgetvalue (result, y, 3))) asprintf (&dev_name, "'%s'", PQgetvalue (result, y, 3));
      else dev_name = "NULL";
      if (strlen(PQgetvalue (result, y, 4))) asprintf (&dev_desc, "'%s'", PQgetvalue (result, y, 4));
      else dev_desc = "NULL";
      if (strlen(PQgetvalue (result, y, 5))) asprintf (&obj_name, "'%s'", PQgetvalue (result, y, 5));
      else obj_name = "NULL";
      if (strlen(PQgetvalue (result, y, 6))) asprintf (&obj_desc, "'%s'", PQgetvalue (result, y, 6));
      else obj_desc = "NULL";
      if (strlen(PQgetvalue (result, y, 7))) asprintf (&trg_name, "'%s'", PQgetvalue (result, y, 7));
      else trg_name = "NULL";
      if (strlen(PQgetvalue (result, y, 8))) asprintf (&trg_desc, "'%s'", PQgetvalue (result, y, 8));
      else trg_desc = "NULL";
      if (strlen(PQgetvalue (result, y, 9))) asprintf (&trgtype_str, "'%s'", PQgetvalue (result, y, 9));
      else trgtype_str = "NULL";
      if (strlen(PQgetvalue (result, y, 10))) asprintf (&xval_str, "'%s'", PQgetvalue (result, y, 10));
      else xval_str = "NULL";
      if (strlen(PQgetvalue (result, y, 11))) asprintf (&yval_str, "'%s'", PQgetvalue (result, y, 11));
      else yval_str = "NULL";
      if (strlen(PQgetvalue (result, y, 12))) asprintf (&duration_str, "'%s'", PQgetvalue (result, y, 12));
      else duration_str = "NULL";
      if (strlen(PQgetvalue (result, y, 13))) asprintf (&adminstate_str, "'%s'", PQgetvalue (result, y, 13));
      else adminstate_str = "NULL";
      if (strlen(PQgetvalue (result, y, 14))) asprintf (&cnt_name, "'%s'", PQgetvalue (result, y, 14));
      else cnt_name = "NULL";
      if (strlen(PQgetvalue (result, y, 15))) asprintf (&cnt_desc, "'%s'", PQgetvalue (result, y, 15));
      else cnt_desc = "NULL";
      if (strlen(PQgetvalue (result, y, 16))) asprintf (&tset_name, "'%s'", PQgetvalue (result, y, 16));
      else tset_name = "NULL";
      if (strlen(PQgetvalue (result, y, 17))) asprintf (&tset_desc, "'%s'", PQgetvalue (result, y, 17));
      else tset_desc = "NULL";

      /* Create insert query */
      char *insert;
      asprintf (&insert, "INSERT INTO triggerset_valrules (id, cnt_name, cnt_desc, tset_name, tset_desc, trg_name, trg_desc, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, trgtype, xval, yval, duration, adminstate) VALUES (%s, %s, %s , %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);",
        id_str, cnt_name, cnt_desc, tset_name, tset_desc, trg_name, trg_desc, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, trgtype_str, xval_str, yval_str, duration_str, adminstate_str);

      /* Insert row into cache */
      sqlerr = NULL;
      sqlite3_exec (static_valrule_cache, insert, NULL, NULL, &sqlerr);
      free (insert);
      if (sqlerr)
      { i_printf (1, "i_triggerset_valrule_sql_load_cache error from sqlite: %s", sqlerr); sqlite3_free (sqlerr); }
    }
  }
  else
  { i_printf (1, "i_triggerset_valrule_sql_load_cache failed execute query '%s'", query); }

  PQclear (result);
  i_pg_close (conn);
  free (query);

  return 0;
}

i_list* i_triggerset_valrule_sql_load_sync (i_resource *self, i_triggerset *tset, i_object *obj, i_trigger *trg)
{
  /* Returns the list of value rules
   * that applied to the specified triggerset
   * when bound to that objects trigger
   */
  char *query;
  char *cnt_esc;
  char *tset_esc;
  char *trg_esc;
  char *site_esc;
  char *dev_esc;
  char *obj_esc;
  i_list *list = NULL;

  if (!static_valrule_cache)
  { i_triggerset_valrule_sql_load_cache (self); }

  /* Create query */
  cnt_esc = i_postgres_escape (tset->cnt->name_str);
  tset_esc = i_postgres_escape (tset->name_str);
  trg_esc = i_postgres_escape (trg->name_str);
  site_esc = i_postgres_escape (tset->cnt->dev->site->name_str);
  dev_esc = i_postgres_escape (tset->cnt->dev->name_str);
  if (obj)
  {
    obj_esc = i_postgres_escape (obj->name_str);
    asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, trg_name, trg_desc, trgtype, xval, yval, duration, adminstate FROM triggerset_valrules WHERE cnt_name='%s' AND tset_name='%s' AND trg_name='%s' AND (obj_name='%s' OR obj_name IS NULL) AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY (obj_name is NULL), obj_name, (dev_name is NULL), dev_name, (site_name is NULL), site_name ASC",
      cnt_esc, tset_esc, trg_esc, obj_esc, dev_esc, site_esc);
    free (obj_esc);
  }
  else
  {
    asprintf (&query, "SELECT id, site_name, site_desc, dev_name, dev_desc, obj_name, obj_desc, trg_name, trg_desc, trgtype, xval, yval, duration, adminstate FROM triggerset_valrules WHERE cnt_name='%s' AND tset_name='%s' AND (dev_name='%s' OR dev_name IS NULL) AND (site_name='%s' OR site_name IS NULL) ORDER BY obj_name, dev_name, site_name ASC",
      cnt_esc, tset_esc, dev_esc, site_esc);
  }
  free (cnt_esc);
  free (tset_esc);
  free (trg_esc);
  free (site_esc);
  free (dev_esc); 

  /* Execute query */
  char **result = NULL;
  int rows = 0;
  int cols = 0;
  char *sqlerr = NULL;
  int num = sqlite3_get_table (static_valrule_cache, query, &result, &rows, &cols, &sqlerr);

  /* Check for result */
  if (num == SQLITE_OK)
  {
    /* Create list */
    list = i_list_create ();
    i_list_set_destructor (list, i_triggerset_valrule_free);

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
      char *trg_name;
      char *trg_desc;
      char *trgtype_str;
      char *xval_str;
      char *yval_str;
      char *duration_str;
      char *adminstate_str;
      i_triggerset_valrule *rule;

      /* Get values */
      id_str = result[0 + (y * cols)];
      site_name = result[1 + (y * cols)];
      site_desc = result[2 + (y * cols)];
      dev_name = result[3 + (y * cols)];
      dev_desc = result[4 + (y * cols)];
      obj_name = result[5 + (y * cols)];
      obj_desc = result[6 + (y * cols)];
      trg_name = result[7 + (y * cols)];
      trg_desc = result[8 + (y * cols)];
      trgtype_str = result[9 + (y * cols)];
      xval_str = result[10 + (y * cols)];
      yval_str = result[11 + (y * cols)];
      duration_str = result[12 + (y * cols)];
      adminstate_str = result[13 + (y * cols)];

      /* DEBUG */
      i_printf(0, "DEBUG(obj=%p/%s): row=%i id=%s site=%s dev=%s obj=%s x=%s y=%s duration=%s admin=%s", obj, obj ? obj->name_str : NULL, y, id_str, site_name, dev_name, obj_name, xval_str, yval_str, duration_str, adminstate_str);
      /* END DEBUG */

      /* Rule */
      rule = i_triggerset_valrule_create ();
      if (id_str) rule->id = atol (id_str);
      if (site_name && strlen (site_name) > 0) rule->site_name = strdup (site_name);
      if (site_desc && strlen (site_desc) > 0) rule->site_desc = strdup (site_desc);
      if (dev_name && strlen (dev_name) > 0) rule->dev_name = strdup (dev_name);
      if (dev_desc && strlen (dev_desc) > 0) rule->dev_desc = strdup (dev_desc);
      if (obj_name && strlen (obj_name) > 0) rule->obj_name = strdup (obj_name);
      if (obj_desc && strlen (obj_desc) > 0) rule->obj_desc = strdup (obj_desc);
      if (trg_name && strlen (trg_name) > 0) rule->trg_name = strdup (trg_name);
      if (trg_desc && strlen (trg_desc) > 0) rule->trg_desc = strdup (trg_desc);
      if (trgtype_str) rule->trg_type = atoi (trgtype_str);
      if (xval_str && strlen (xval_str) > 0) rule->xval_str = strdup (xval_str);
      if (yval_str && strlen (yval_str) > 0) rule->yval_str = strdup (yval_str);
      if (duration_str) rule->duration_sec = atol (duration_str);
      if (adminstate_str) rule->adminstate = atoi (adminstate_str);

      /* Enqueue */
      i_list_enqueue (list, rule);
    }
  }
  else
  { i_printf (1, "i_triggerset_valrule_sql_load_sync failed execute query '%s'", query); }

  if (result) sqlite3_free_table (result);
  if (sqlerr) sqlite3_free (sqlerr);
  free (query);

  return list;
}

/* @} */
