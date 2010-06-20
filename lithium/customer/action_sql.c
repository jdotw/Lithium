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
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/incident.h>
#include <induction/postgresql.h>

#include "action.h"

/* Action SQL Operations */

int l_action_sql_insert (i_resource *self, l_action *action)
{
  int num;
  char *desc_esc;
  char *script_esc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  desc_esc = i_postgres_escape (action->desc_str);
  script_esc = i_postgres_escape (action->script_file);
  asprintf (&query, "INSERT INTO actions (descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script) VALUES ('%s', '%i', '%i', '%li', '%i', '%li', '%i', '%i', '%i', '%i', '%s');",
    action->desc_str, action->enabled, action->activation, action->delay, action->rerun, action->rerun_delay, action->time_filter, action->day_mask, action->start_hour, action->end_hour, script_esc);
  free (desc_esc);
  free (script_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_insert failed to execute INSERT for new action"); return -1; }

  /* Set id */
  action->id = l_action_id_assign ();

  return 0;
}

int l_action_sql_update (i_resource *self, l_action *action)
{
  int num;
  char *query;
  char *script_esc;
  char *desc_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  desc_esc = i_postgres_escape (action->desc_str);
  script_esc = i_postgres_escape (action->script_file);
  asprintf (&query, "UPDATE actions SET descr='%s', enabled='%i', activation='%i', delay='%li', rerun='%i', rerundelay='%li', timefilter='%i', daymask='%i', starthour='%i', endhour='%i', script='%s' WHERE id='%li'",
    desc_esc, action->enabled, action->activation, action->delay, action->rerun, action->rerun_delay, action->time_filter, action->day_mask, action->start_hour, action->end_hour, script_esc, action->id);
  free (desc_esc);
  free (script_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_update failed to execute UPDATE for action %li", action->id); return -1; }

  return 0;
}

int l_action_sql_delete (i_resource *self, int action_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM actions WHERE id='%li'", action_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_update failed to execute DELETE for action %li", action_id); return -1; }

  return 0;
}

i_callback* l_action_sql_load_list (i_resource *self, char *id_str, int (*cbfunc) (), void *passdata)
{
  /* Load list of all actions */
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_load_list failed to open SQL database connection"); return NULL; }

  /* Create query */
  if (id_str)
  {
    asprintf (&query, "SELECT id, descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script FROM actions WHERE id='%s'", id_str);
  }
  else
  {
    asprintf (&query, "SELECT id, descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script FROM actions");
  }

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_action_sql_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
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

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_action_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *descr_str;
    char *enabled_str;
    char *activation_str;
    char *delay_str;
    char *rerun_str;
    char *rerun_delay_str;
    char *time_filter_str;
    char *day_mask_str;
    char *start_hour_str;
    char *end_hour_str;
    char *script_str;
    l_action *action;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    descr_str = PQgetvalue (res, row, 1);
    enabled_str = PQgetvalue (res, row, 2);
    activation_str = PQgetvalue (res, row, 3);
    delay_str = PQgetvalue (res, row, 4);
    rerun_str = PQgetvalue (res, row, 5);
    rerun_delay_str = PQgetvalue (res, row, 6);
    time_filter_str = PQgetvalue (res, row, 7);
    day_mask_str = PQgetvalue (res, row, 8);
    start_hour_str = PQgetvalue (res, row, 9);
    end_hour_str = PQgetvalue (res, row, 10);
    script_str = PQgetvalue (res, row, 11);

    /* Create action */
    action = l_action_create ();
    if (id_str) action->id = atol (id_str);
    if (descr_str) action->desc_str = strdup (descr_str);
    if (enabled_str) action->enabled = atoi (enabled_str);
    if (activation_str) action->activation = atoi (activation_str);
    if (delay_str) action->delay = atol (delay_str);
    if (rerun_str) action->rerun = atoi (rerun_str);
    if (rerun_delay_str) action->rerun_delay = atol (rerun_delay_str);
    if (time_filter_str) action->time_filter = atoi (time_filter_str);
    if (day_mask_str) action->day_mask = atoi (day_mask_str);
    if (start_hour_str) action->start_hour= atoi (start_hour_str);
    if (end_hour_str) action->end_hour = atoi (end_hour_str);
    if (script_str) action->script_file = strdup (script_str);

    /* Enqueue */
    i_list_enqueue (list, action);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0)
    {
      /* Returning !=0 causes the list to be freed */
      i_list_free (list);
    }
  }
  else
  { i_list_free (list); }

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Entity table */

int l_action_sql_entity_insert (i_resource *self, long action_id, i_entity_descriptor *ent)
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
  { i_printf (1, "l_action_entity_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  if (ent->site_name) { asprintf (&site_name, "'%s'",  ent->site_name); asprintf (&site_desc, "'%s'", ent->site_desc); }
  else { asprintf (&site_name, "NULL"); asprintf (&site_desc, "NULL"); }
  if (ent->site_suburb) { asprintf (&site_suburb, "'%s'", ent->site_suburb); }
  else { asprintf (&site_suburb, "NULL"); }
  if (ent->dev_name) { asprintf (&dev_name, "'%s'",  ent->dev_name); asprintf (&dev_desc, "'%s'", ent->dev_desc); }
  else { asprintf (&dev_name, "NULL"); asprintf (&dev_desc, "NULL"); }
  if (ent->cnt_name) { asprintf (&cnt_name, "'%s'",  ent->cnt_name); asprintf (&cnt_desc, "'%s'", ent->cnt_desc); }
  else { asprintf (&cnt_name, "NULL"); asprintf (&cnt_desc, "NULL"); }
  if (ent->obj_name) { asprintf (&obj_name, "'%s'",  ent->obj_name); asprintf (&obj_desc, "'%s'", ent->obj_desc); }
  else { asprintf (&obj_name, "NULL"); asprintf (&obj_desc, "NULL"); }
  if (ent->met_name) { asprintf (&met_name, "'%s'",  ent->met_name); asprintf (&met_desc, "'%s'", ent->met_desc); }
  else { asprintf (&met_name, "NULL"); asprintf (&met_desc, "NULL"); }
  if (ent->trg_name) { asprintf (&trg_name, "'%s'",  ent->trg_name); asprintf (&trg_desc, "'%s'", ent->trg_desc); }
  else { asprintf (&trg_name, "NULL"); asprintf (&trg_desc, "NULL"); }
  
  asprintf (&query, "INSERT INTO action_entities (action, type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc) VALUES ('%li', '%i', %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s);",
    action_id, ent->type, site_name, site_desc, site_suburb, dev_name, dev_desc, 
    cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc);
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
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_entity_sql_insert failed to execute INSERT for action %li", action_id); return -1; }

  return 0;
}

int l_action_sql_entity_delete (i_resource *self, long action_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_entity_sql_delete failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM action_entities WHERE action='%li'", action_id);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_entity_sql_delete failed to execute delete for action_id %li", action_id); return -1; }

  return 0;

}

i_callback* l_action_sql_entity_load_list (i_resource *self, int action_id, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_entity_load_list failed to open SQL database connection"); return NULL; }

  /* Query string */
  asprintf (&query, "SELECT type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc FROM action_entities WHERE action=%li", action_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_entity_list_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_entity_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }
 
  return cb;
}

int l_action_sql_entity_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_action_sql_entity_load_list failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, i_entity_descriptor_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *type_str;
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
    type_str = PQgetvalue (res, row, 0);
    site_name = PQgetvalue (res, row, 1);
    site_desc = PQgetvalue (res, row, 2);
    site_suburb = PQgetvalue (res, row, 3);
    dev_name = PQgetvalue (res, row, 4);
    dev_desc = PQgetvalue (res, row, 5);
    cnt_name = PQgetvalue (res, row, 6);
    cnt_desc = PQgetvalue (res, row, 7);
    obj_name = PQgetvalue (res, row, 8);
    obj_desc = PQgetvalue (res, row, 9);
    met_name = PQgetvalue (res, row, 10);
    met_desc = PQgetvalue (res, row, 11);
    trg_name = PQgetvalue (res, row, 12);
    trg_desc = PQgetvalue (res, row, 13);

    /* Create entity */
    i_entity_descriptor *ent = i_entity_descriptor_create ();
    if (type_str) ent->type = atoi (type_str);
    ent->cust_name = strdup (self->hierarchy->cust_name);
    ent->cust_desc = strdup (self->hierarchy->cust_desc);
    if (site_name) ent->site_name = strdup (site_name);
    if (site_desc) ent->site_desc = strdup (site_desc);
    if (site_suburb) ent->site_suburb = strdup (site_suburb);
    if (dev_name) ent->dev_name = strdup (dev_name);
    if (dev_desc) ent->dev_desc = strdup (dev_desc);
    if (cnt_name) ent->cnt_name = strdup (cnt_name);
    if (cnt_desc) ent->cnt_desc = strdup (cnt_desc);
    if (obj_name) ent->obj_name = strdup (obj_name);
    if (obj_desc) ent->obj_desc = strdup (obj_desc);
    if (met_name) ent->met_name = strdup (met_name);
    if (met_desc) ent->met_desc = strdup (met_desc);
    if (trg_name) ent->trg_name = strdup (trg_name);
    if (trg_desc) ent->trg_desc = strdup (trg_desc);

    /* Enqueue */
    i_list_enqueue (list, ent);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0) i_list_free (list);
  }
  else i_list_free (list);

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Config variables */

int l_action_sql_configvar_insert (i_resource *self, long action_id, l_action_configvar *var)
{
  /* Attempts to UPDATE and/or INSERT a config variable */
  
  int num;
  char *insert_query = NULL;
  char *name_esc;
  char *value_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_configvar_sql_insert failed to open SQL database connection"); return -1; }

  /* Create queries */
  name_esc = i_postgres_escape (var->name_str);
  if (var->value_str && strlen(var->value_str) > 0)
  {
    char *str = i_postgres_escape (var->value_str);
    asprintf (&value_esc, "'%s'", str); 
  }
  else
  { asprintf (&value_esc, "NULL"); }
  
  asprintf (&insert_query, "INSERT INTO action_configvars (action, name, value) VALUES ('%li', '%s', %s)",
    action_id, name_esc, value_esc);
  free (name_esc);
  free (value_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, insert_query, 0, l_action_sql_cb, NULL);
  free (insert_query);
  if (num != 0)
  { i_printf (1, "l_action_configvar_sql_insert failed to execute query for action %li", action_id); return -1; }

  return 0;
}

int l_action_sql_configvar_delete (i_resource *self, long action_id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;
  
  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_configvar_sql_delete failed to open SQL database connection"); return -1; }
  
  /* Create query */
  asprintf (&query, "DELETE FROM action_configvars WHERE action='%li'", action_id);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_configvar_sql_delete failed to execute delete for action %li", action_id); return -1; }

  return 0;
}

i_callback* l_action_sql_configvar_load_list (i_resource *self, long action_id, int (*cbfunc) (), void *passdata)
{
  /* Load list of all actions */
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_configvar_load_list failed to open SQL database connection"); return NULL; }

  /* Create query */
  asprintf (&query, "SELECT id, name, value FROM action_configvars WHERE action='%li'", action_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_configvar_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_configvar_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_action_sql_configvar_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_aciton_sql_configvar_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_action_configvar_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *name_str;
    char *value_str;
    l_action_configvar *var;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    name_str = PQgetvalue (res, row, 1);
    value_str = PQgetvalue (res, row, 2);

    /* Create action */
    var = l_action_configvar_create ();
    if (id_str) var->id = atol (id_str);
    if (name_str) var->name_str = strdup (name_str);
    if (value_str) var->value_str = strdup (value_str);

    /* Enqueue */
    i_list_enqueue (list, var);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0)
    {
      /* Returning !=0 causes the list to be freed */
      i_list_free (list);
    }
  }
  else
  { i_list_free (list); }

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Log Entry */

int l_action_sql_log_insert (i_resource *self, long action_id, i_entity_descriptor *ent, char *comment_str)
{
  int num;
  char *query;
  char *comment_esc;
  i_pg_async_conn *conn;
  struct timeval now;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_log_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  gettimeofday (&now, NULL);
  if (comment_str) comment_esc = i_postgres_escape (comment_str);
  else comment_esc = strdup ("");
  if (ent)
  {
    asprintf (&query, "INSERT INTO action_history (action, type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, timestamp, comments) VALUES ('%li', '%i', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%li', '%s');",
      action_id, ent->type, ent->site_name, ent->site_desc, ent->site_suburb,
      ent->dev_name, ent->dev_desc, ent->cnt_name, ent->cnt_desc, ent->obj_name, ent->obj_desc,
      ent->met_name, ent->met_desc, ent->trg_name, ent->trg_desc, now.tv_sec, comment_esc);
    free (comment_esc);
  }
  else
  {
    asprintf (&query, "INSERT INTO action_history (action, type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, timestamp, comments) VALUES ('%li', '0', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '%li', '%s');",
      action_id, now.tv_sec, comment_esc);
    free (comment_esc);
  }

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_log_insert failed to execute INSERT for action %li", action_id); return -1; }

  return 0;
}

i_callback* l_action_sql_log_load (i_resource *self, int action_id, int (*cbfunc) (), void *passdata)
{
  int num;
  char *query;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_entity_load_list failed to open SQL database connection"); return NULL; }

  /* Query string */
  asprintf (&query, "SELECT id, type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, timestamp, comments FROM action_history WHERE action=%li LIMIT 250", action_id);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_log_load_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_entity_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }
 
  return cb;
}

int l_action_sql_log_load_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  i_list *list;
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_action_sql_log_load_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_action_log_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *type_str;
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
    char *time_str;
    char *comment_str;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    type_str = PQgetvalue (res, row, 1);
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
    time_str = PQgetvalue (res, row, 15);
    comment_str = PQgetvalue (res, row, 16);

    /* Create log entry */
    l_action_log *log = l_action_log_create ();
    if (time_str) log->timestamp = atol (time_str);
    if (comment_str) log->comment_str = strdup (comment_str);

    /* Create entity */
    log->ent = i_entity_descriptor_create ();
    if (id_str) log->id = atol (id_str);
    if (type_str) log->ent->type = atoi (type_str);
    log->ent->cust_name = strdup (self->hierarchy->cust_name);
    log->ent->cust_desc = strdup (self->hierarchy->cust_desc);
    if (site_name) log->ent->site_name = strdup (site_name);
    if (site_desc) log->ent->site_desc = strdup (site_desc);
    if (site_suburb) log->ent->site_suburb = strdup (site_suburb);
    if (dev_name) log->ent->dev_name = strdup (dev_name);
    if (dev_desc) log->ent->dev_desc = strdup (dev_desc);
    if (cnt_name) log->ent->cnt_name = strdup (cnt_name);
    if (cnt_desc) log->ent->cnt_desc = strdup (cnt_desc);
    if (obj_name) log->ent->obj_name = strdup (obj_name);
    if (obj_desc) log->ent->obj_desc = strdup (obj_desc);
    if (met_name) log->ent->met_name = strdup (met_name);
    if (met_desc) log->ent->met_desc = strdup (met_desc);
    if (trg_name) log->ent->trg_name = strdup (trg_name);
    if (trg_desc) log->ent->trg_desc = strdup (trg_desc);

    /* Enqueue */
    i_list_enqueue (list, log);
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, list, cb->passdata);
    if (num != 0) i_list_free (list);
  }
  else i_list_free (list);

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}
/* SQL Callback */

int l_action_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_action_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}


