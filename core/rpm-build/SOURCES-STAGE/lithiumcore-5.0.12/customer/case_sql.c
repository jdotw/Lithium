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
#include <induction/callback.h>
#include <induction/postgresql.h>

#include "case.h"

/*
 * Case System -- SQL Functions
 */

/* Insert */

int l_case_sql_insert (i_resource *self, l_case *cas)
{
  int num;
  char *hline_esc;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  hline_esc = i_postgres_escape (cas->hline_str);
  asprintf (&query, "INSERT INTO cases (state, start_sec, end_sec, hline, owner, requester) VALUES ('%u', '%li', '%li', '%s', '%s', '%s');",
    cas->state, cas->start.tv_sec, cas->end.tv_sec, hline_esc, cas->owner_str, cas->requester_str);
  free (hline_esc);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_sql_insert failed to execute INSERT for case %li", cas->id); return -1; }

  /* Set id */
  cas->id = l_case_id_assign ();

  return 0;
}

/* Update */

int l_case_sql_update (i_resource *self, l_case *cas)
{
  int num;
  char *query;
  char *hline_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_sql_update failed to open SQL db connection"); return -1; }

  /* Create query */
  hline_esc = i_postgres_escape (cas->hline_str);
  asprintf (&query, "UPDATE cases SET state='%u', start_sec='%li', end_sec='%li', hline='%s', owner='%s', requester='%s' WHERE id='%li'",
    cas->state, cas->start.tv_sec, cas->end.tv_sec, hline_esc, cas->owner_str, cas->requester_str, cas->id);
  free (hline_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_sql_update failed to execute UPDATE for case %li", cas->id); return -1; }

  return 0;
}

/* SQL Callback */

int l_case_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_case_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* List/Search */

i_callback* l_case_sql_list (i_resource *self, char *id_str, char *state_str, char *startupper_str, char *startlower_str, char *endupper_str, char *endlower_str, char *hline_str, char *owner_str, char *requester_str, int (*cbfunc) (), void *passdata)
{
  int num;
  char *str;
  char *query;
  char *hline_esc;
  i_list *termlist;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_sql_list failed to open SQL database connection"); return NULL; }

  /* Query terms */
  termlist = i_list_create ();
  i_list_set_destructor (termlist, free);
  if (id_str) { asprintf (&str, "id='%s'", id_str); i_list_enqueue (termlist, str); }
  if (state_str) { asprintf (&str, "state='%s'", state_str); i_list_enqueue (termlist, str); }
  if (startupper_str) { asprintf (&str, "start_sec<='%s'", startupper_str); i_list_enqueue (termlist, str); }
  if (startlower_str) { asprintf (&str, "start_sec>='%s'", startlower_str); i_list_enqueue (termlist, str); }
  if (endupper_str) { asprintf (&str, "end_sec<='%s'", endupper_str); i_list_enqueue (termlist, str); }
  if (endlower_str) { asprintf (&str, "end_sec>='%s'", endlower_str); i_list_enqueue (termlist, str); }
  if (hline_str) 
  { 
    hline_esc = i_postgres_escape (hline_str);
    asprintf (&str, "hline~*'%s'", hline_esc); 
    free (hline_esc);
    i_list_enqueue (termlist, str); 
  }
  if (owner_str) { asprintf (&str, "owner='%s'", owner_str); i_list_enqueue (termlist, str); }
  if (requester_str) { asprintf (&str, "requester~'%s'", requester_str); i_list_enqueue (termlist, str); }
  
  /* Query string */
  if (termlist->size > 0)
  {
    int termindex = 0;
    char *termstr;
    char *sepstr;
    char *tmpstr;
    
    asprintf (&query, "SELECT id, state, start_sec, end_sec, hline, owner, requester FROM cases WHERE");
    for (i_list_move_head(termlist); (termstr=i_list_restore(termlist))!=NULL; i_list_move_next(termlist))
    {
      if (termindex == 0) sepstr = " ";
      else sepstr = " AND ";

      asprintf (&tmpstr, "%s%s%s", query, sepstr, termstr);
      free (query);
      query = tmpstr;
      termindex++;
    }

    asprintf (&tmpstr, "%s ORDER BY start_sec DESC LIMIT 200", query);
    free (query);
    query = tmpstr;
  }
  else
  { asprintf (&query, "SELECT id, state, start_sec, end_sec, hline, owner, requester FROM cases ORDER BY start_sec DESC LIMIT 200"); }
  i_list_free (termlist);   /* Frees strings by destructor */

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_sql_list_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_sql_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }
 
  return cb;
}

i_callback* l_case_sql_list_idlist (i_resource *self, i_list *idlist, int (*cbfunc) (), void *passdata)
{
  /* Retrieve cases by a list of case ID numbers */
  int num;
  int idindex;
  char *query;
  char *idstr;
  i_callback *cb;
  i_pg_async_conn *conn;

  /* Callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_sql_list_idlist failed to open SQL database connection"); return NULL; }

  /* Create query */
  asprintf (&query, "SELECT id, state, start_sec, end_sec, hline, owner, requester FROM cases WHERE");
  idindex=0;
  for (i_list_move_head(idlist); (idstr=i_list_restore(idlist))!=NULL; i_list_move_next(idlist))
  {
    char *sepstr;
    char *tmpstr;

    if (idindex == 0) sepstr = " ";
    else sepstr = " OR ";

    asprintf (&tmpstr, "%s%s id='%s'", query, sepstr, idstr);
    free (query);
    query = tmpstr;
    idindex++;
  }

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_sql_list_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_sql_list_idlist failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;

}

int l_case_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
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
    i_printf (1, "l_case_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, l_case_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *state_str;
    char *startsec_str;
    char *endsec_str;
    char *hline_str;
    char *owner_str;
    char *requester_str;
    l_case *cas;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    state_str = PQgetvalue (res, row, 1);
    startsec_str = PQgetvalue (res, row, 2);
    endsec_str = PQgetvalue (res, row, 3);
    hline_str = PQgetvalue (res, row, 4);
    owner_str = PQgetvalue (res, row, 5);
    requester_str = PQgetvalue (res, row, 6);

    /* Create case */
    cas = l_case_create ();
    if (id_str) cas->id = atol (id_str);
    if (state_str) cas->state = atoi (state_str);
    if (startsec_str) cas->start.tv_sec = atol (startsec_str);
    if (endsec_str) cas->end.tv_sec = atol (endsec_str);
    if (hline_str) cas->hline_str = strdup (hline_str);
    if (owner_str) cas->owner_str = strdup (owner_str);
    if (requester_str) cas->requester_str = strdup (requester_str);

    /* Enqueue */
    i_list_enqueue (list, cas);
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

/* Get - Full case retrieval */

i_callback *l_case_sql_get (i_resource *self, char *caseid_str, int (*cbfunc) (), void *passdata)
{
  /* Obtains the full case, including details from
   * the cases, case_logentries and case_entities tables
   */
  i_callback *cb;
  i_callback *sqlcb; 

  /* Create callback struct */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;

  /* Obtain case info from cases table */
  sqlcb = l_case_sql_list (self, caseid_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_sql_get_casecb, cb);
  if (!sqlcb)
  { i_printf (1, "l_case_sql_get failed to call l_case_sql_list"); return NULL; }

  return cb;
}

int l_case_sql_get_casecb (i_resource *self, i_list *list, void *passdata)
{
  /* Case info received from cases table */
  char *caseid_str;
  l_case *cas;
  i_callback *sqlcb; 
  i_callback *cb = passdata;

  /* Check a case was found */
  if (list->size < 1)
  {
    /* No case found */
    if (cb->func)
    { cb->func (self, NULL, cb->passdata); }
    i_callback_free (cb);
  }

  /* Restore case from list, then get rid of list (safely) */
  i_list_move_head (list);
  cas = i_list_restore (list);
  i_list_set_destructor (list, NULL);
  i_list_free (list);
  cb->data = cas;

  /* Obtain list of entities from entities table */
  asprintf (&caseid_str, "%li", cas->id);
  sqlcb = l_case_entity_sql_list (self, caseid_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_sql_get_entitycb, cb);
  free (caseid_str);
  if (!sqlcb)
  { 
    i_printf (1, "l_case_sql_get warning, failed to query entity list"); 
    if (cb->func) cb->func (self, NULL, cb->passdata);
    if (cas) l_case_free (cas);
    i_callback_free (cb);
  }

  return 0;   /* We've freed the list safely */
}

int l_case_sql_get_entitycb (i_resource *self, i_list *list, void *passdata)
{
  i_callback *sqlcb; 
  i_callback *cb = passdata;
  l_case *cas = cb->data;
  
  /* Add entity list to case */
  if (cas->ent_list) i_list_free (cas->ent_list);
  cas->ent_list = list;
  
  /* Obtain list of log entries from logentries table */
  sqlcb = l_case_logentry_sql_list (self, cas->id, l_case_sql_get_logcb, cb);
  if (!sqlcb)
  { 
    i_printf (1, "l_case_sql_get warning, failed to query logentry list"); 
    if (cb->func) cb->func (self, NULL, cb->passdata);
    if (cas) l_case_free (cas);
    i_callback_free (cb);
  }

  return 0;   /* We're now using the list */
}

int l_case_sql_get_logcb (i_resource *self, i_list *list, void *passdata)
{
  int num;
  i_callback *cb = passdata;
  l_case *cas = cb->data;
  
  /* Add logentry list to case */
  if (cas->log_list) i_list_free (cas->log_list);
  cas->log_list = list;

  /* All data gathered, call the cbfunc */
  if (cb->func)
  {
    num = cb->func (self, cas, cb->passdata); 
    if (num != 0)
    {
      /* Returning !=0 frees the case */
      l_case_free (cas);
    }
  }
  else
  { l_case_free (cas); }

  i_callback_free (cb);

  return 0;   /* We're now using the list */
}

