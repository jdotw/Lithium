#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/callback.h>
#include <induction/message.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/incident.h>
#include <induction/postgresql.h>

#include "action.h"

/* Action SQL Operations */

typedef struct l_action_sql_load_candidate_req_s
{
  char *incident_match_str;             /* SQL matching for the incident entity */
  struct i_list_s *list;                /* Action list */
  
} l_action_sql_load_candidate_req;

/* Load list of candidate actions */

i_callback* l_action_sql_load_candidates (i_resource *self, i_incident *inc, int (*cbfunc) (), void *passdata)
{
  /* Load list of actions that are a candidates for execution
   * based on the incident provided.
   *
   * Step 1) Load a list of actions that 'could' be executed 
   *         based on whether they are enabled and the time of day.
   *
   * Step 2) Load a list of action ids that match the entity
   *         in the specified incident using NULL-is-all wildcard
   *
   * Step 3) Go through the list of actions and action id's from
   *         the first and second queries and set execution flags 
   *         accordingly. (If no entities, set execution... if entities
   *         and a hit from the entities table, set execution)
   *
   * Step 4) Return list
   *
   */

  /* Load list of all actions */
  int num;
  char *query;
  i_pg_async_conn *conn;
  i_callback *cb;
  l_action_sql_load_candidate_req *req;

  /* Create req */
  req = (l_action_sql_load_candidate_req *) malloc (sizeof(l_action_sql_load_candidate_req));
  memset (req, 0, sizeof(l_action_sql_load_candidate_req));
  req->list = i_list_create ();
  i_list_set_destructor (req->list, l_action_free);

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;
  cb->data = req;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_action_sql_load_candidates_list failed to open SQL database connection"); return NULL; }

  /* Get time */
  struct timeval now_tv;
  struct tm *now_tm;
  gettimeofday (&now_tv, NULL);
  now_tm = localtime (&now_tv.tv_sec);
  int daymask = 0;
  if (now_tm->tm_wday == 0) daymask = 64;
  else if (now_tm->tm_wday == 1) daymask = 1;
  else if (now_tm->tm_wday == 2) daymask = 2;
  else if (now_tm->tm_wday == 3) daymask = 4;
  else if (now_tm->tm_wday == 4) daymask = 8;
  else if (now_tm->tm_wday == 5) daymask = 16;
  else if (now_tm->tm_wday == 6) daymask = 32;

  /* Create query */
  asprintf (&query, "SELECT id, descr, enabled, activation, delay, rerun, rerundelay, timefilter, daymask, starthour, endhour, script, log_output, (SELECT count(*) FROM action_entities WHERE action_entities.action=actions.id) AS entities FROM actions WHERE enabled='1' AND (daymask & %i) > 0 AND (timefilter='0' OR (starthour >= %i AND endhour < %i))", daymask, now_tm->tm_hour, now_tm->tm_hour);

  /* Create incident/entity match string 
   *
   * We do this now because we can't rely on the incident hanging around
   */
  asprintf (&req->incident_match_str, "(site_name='%s' OR site_name IS NULL) AND (dev_name='%s' OR dev_name IS NULL) AND (cnt_name='%s' OR cnt_name IS NULL) AND (obj_name='%s' OR obj_name IS NULL) AND (met_name='%s' OR met_name IS NULL) AND (trg_name='%s' OR trg_name IS NULL)", inc->ent->site_name, inc->ent->dev_name, inc->ent->cnt_name, inc->ent->obj_name, inc->ent->met_name, inc->ent->trg_name);

  i_printf(0, "DEBUG: Query is %s", query);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_action_sql_load_candidates_actioncb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_action_sql_load_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_action_sql_load_candidates_actioncb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from FIRST SELECT query */
  int num;
  int row;
  int row_count;
  char *action_id_match = NULL;
  i_callback *cb = passdata;
  l_action_sql_load_candidate_req *req = cb->data;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_aciton_sql_load_candidates_actioncb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
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
    char *log_output_str;
    char *entities_str;
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
    log_output_str = PQgetvalue (res, row, 12);
    entities_str = PQgetvalue (res, row, 13);

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
    if (log_output_str) action->log_output = atoi (log_output_str);
    if (entities_str) action->entity_count = atoi (entities_str);

    /* Enqueue */
    i_list_enqueue (req->list, action);

    /* Check entity count */
    if (action->entity_count > 0)
    {
      /* This action is filtered by entity
       * Add our ID to the match string for
       * the second SQL query
       */

      if (action_id_match)
      {
        char *temp_str;
        asprintf (&temp_str, "%s OR action='%li'", action_id_match, action->id);
        free (action_id_match);
        action_id_match = temp_str;
      }
      else
      {
        asprintf (&action_id_match, "action='%li'", action->id);
      }
    }
    else
    {
      /* There are no entities to filter. Mark for execution */
      action->execute_flag = 1;
    }
  }

  /* Check to see if a second SQL query is necessary to 
   * do any entity matching
   */
  if (action_id_match)
  {
    /* Some actions are marked as being filtered by entity. 
     * We now execute a second SQL string to see which of the 
     * candidate actions are eligable according to the 
     * entity that has raised the incident
     */

    /* Open conn */
    i_pg_async_conn *second_conn = i_pg_async_conn_open_customer (self);
    if (!second_conn)
    { i_printf (1, "l_action_sql_load_candidates_actioncb failed to open SQL database connection"); return -1; }

    /* Create query */
    char *query_str;
    asprintf (&query_str, "SELECT action FROM action_entities WHERE (%s) AND (%s)", req->incident_match_str, action_id_match);
    free (action_id_match);

    /* Execute query */
    num = i_pg_async_query_exec (self, second_conn, query_str, 0, l_action_sql_load_candidates_idcb, cb);
    free (query_str);
    if (num != 0)
    { i_printf (1, "l_action_sql_load_candidates_actioncb failed to execute SELECT query"); i_callback_free (cb); return -1; }
  }
  else
  {
    /* There were no actions that require entity matching. 
     *
     * We can now run the callback
     */
    
    /* Run Callback */
    if (cb->func)
    {
      num = cb->func (self, req->list, cb->passdata);
      if (num != 0)
      {
        /* Returning !=0 causes the list to be freed */
        i_list_free (req->list);
      }
    }
    else
    { i_list_free (req->list); }
    
    /* Total cleanup */
    if (req->incident_match_str) free (req->incident_match_str);
    free (req);
    i_callback_free (cb);
  }

  /* SQL Cleanup */
  i_pg_async_conn_close (conn);

  return 0;
}

int l_action_sql_load_candidates_idcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from SECOND SELECT query */
  int num;
  int row;
  int row_count;
  i_callback *cb = passdata;
  l_action_sql_load_candidate_req *req = cb->data;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_aciton_sql_load_candidates_idcb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    unsigned long id;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);

    /* Create action */
    id = atol (id_str);

    /* Go looking for a matching action */
    l_action *action;
    for (i_list_move_head(req->list); (action=i_list_restore(req->list))!=NULL; i_list_move_next(req->list))
    {
      if (action->id == id)
      {
        /* Match! */
        action->execute_flag = 1;
        break;
      }
    }
  }

  /* At this point, every action that is valid
   * by time, config and entity is in the list
   * with its execute_flag set to 1. 
   *
   * Remove any that are not 
   */

  l_action *action;
  for (i_list_move_head(req->list); (action=i_list_restore(req->list))!=NULL; i_list_move_next(req->list))
  {
    if (action->execute_flag == 0)
    { 
      i_list_delete (req->list); 
    }
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, req->list, cb->passdata);
    if (num != 0)
    {
      /* Returning !=0 causes the list to be freed */
      i_list_free (req->list);
    }
  }
  else
  { i_list_free (req->list); }
    
  /* Total cleanup */
  if (req->incident_match_str) free (req->incident_match_str);
  free (req);
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

