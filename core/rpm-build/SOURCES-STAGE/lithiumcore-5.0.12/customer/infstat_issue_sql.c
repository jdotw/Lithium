#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/callback.h>

#include "infstat.h"

/* SQL Init */

int l_infstat_issue_sql_init (i_resource *self)
{
  PGresult *res;
  PGconn *pgconn;
    
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_infstat_issue_sql_init failed to connect to postgresql database"); return -1; }

  /* infstat_issues table */
  i_pg_begin (pgconn);
  res = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'infstat_issues' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || (PQntuples(res)) < 1)
  {   
    /* infstat_issues table doesnt exist */
    res = PQexec (pgconn, "CREATE TABLE infstat_issues (id serial, category varchar, headline varchar, descr varchar, severity integer, affects integer, state integer, closenote varchar, start_sec integer, etr_sec integer, end_sec integer, adminlead_username varchar, techlead_username varchar)");
    if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
    { i_printf (1, "l_infstat_issue_sql_init failed to create infstat_issues table (%s)", PQresultErrorMessage (res)); PQclear(res); return -1; }
  }   PQclear(res);

  /* Get next ID */
  i_pg_begin (pgconn);
  res = PQexec (pgconn, "SELECT nextval('infstat_issues_id_seq')");
  i_pg_end (pgconn);
  if (res && PQresultStatus(res) == PGRES_TUPLES_OK && (PQntuples(res)) > 0)
  {
    char *next_id_str;
    next_id_str = PQgetvalue (res, 0, 0);
    if (next_id_str)
    { l_infstat_issue_nextid_set ((atol(next_id_str))+1); }
    else
    { i_printf (1, "l_infstat_issue_sql_init failed to get next_id_str afrom PQgetvalue for infstat_issues id"); }
  }
  else
  { i_printf (1, "l_infstat_issue_sql_init failed to get next issue ID number from SQL database %s", PQresultErrorMessage(res)); }
  if (res) PQclear(res);
    
  /* Finished */
  i_pg_close (pgconn);
      
  return 0;
} 

/* SQL Load */

int l_infstat_issue_sql_load (i_resource *self, l_infstat_cat *cat, int (*callback_func) (i_resource *self, int result, void *passdata), void *passdata)
{
  int num;
  char *command;
  char *name_esc;
  i_pg_async_conn *conn;
  i_callback *callback;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_infstat_issue_sql_load failed to connect to SQL database"); return -1; }

  /* Create callback struct */
  callback = i_callback_create ();
  if (!callback)
  { i_printf (1, "l_infstat_issue_sql_load failed to create callback struct"); i_pg_async_conn_close (conn); return -1; }
  callback->func = callback_func;
  callback->data = cat;
  callback->passdata = passdata;

  /* Command string */
  name_esc = i_postgres_escape (cat->name_str);
  asprintf (&command, "SELECT * FROM infstat_issues WHERE category='%s' AND state='%i'", cat->name_str, INFSTAT_STATE_OPEN);
  if (name_esc) free (name_esc);

  /* Execute command */
  num = i_pg_async_query_exec (self, conn, command, 0, l_infstat_issue_sql_load_sqlcb, callback);
  free (command);
  if (num != 0)
  {
    i_printf (1, "l_infstat_issue_sql_load failed to execute SELECT command for category %s", cat->name_str);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  /* Finished */

  return 0;
}

int l_infstat_issue_sql_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Callback for the result of SELECTing issue records
   *
   * This callback is called once per calling of l_infstat_issue_sql_load 
   * (i.e once per category)
   */

  int num;
  int y;
  int row_count;
  i_callback *callback = passdata;
  l_infstat_cat *cat;

  cat = (l_infstat_cat *) callback->data;
  
  if (!result)
  {
    i_printf (1, "l_infstat_issue_sql_load_sqlcb query failed, recvd NULL result during operation code %i", operation);
    callback->func (self, 0, callback->passdata);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  if (PQresultStatus(result) != PGRES_TUPLES_OK)
  {
    i_printf (1, "l_infstat_issue_sql_load_sqlcb query failed, at operation %i (%s)", operation, PQresultErrorMessage (result));
    callback->func (self, 0, callback->passdata);
    i_pg_async_conn_close (conn);
    i_callback_free (callback);
    return -1;
  }

  /* Interpret result */
  row_count = PQntuples (result);
  for (y=0; y < row_count; y++)
  {
    int x;
    int field_count;
    l_infstat_issue *issue;

    issue = l_infstat_issue_create ();
    if (!issue)
    { i_printf (1, "l_infstat_issue_sql_load_sqlcb failed to create issue struct at row %i for category %s, continuing", cat->name_str, y); continue; }
    issue->cat = cat;

    field_count = PQnfields (result);
    for (x=0; x < field_count; x++)
    {
      char *field_name;
      char *value;

      field_name = PQfname (result, x);
      if (!field_name)
      { i_printf (1, "l_infstat_issue_sql_load_sqlcb failed to get field name for i=%i, continuing", x); continue; }

      value = PQgetvalue (result, y, x);

      if (!strcmp(field_name, "id"))
      { if (value) issue->id = atol (value); }
      if (!strcmp(field_name, "headline"))
      { if (value) issue->headline_str = strdup (value); }
      if (!strcmp(field_name, "descr"))
      { if (value) issue->desc_str = strdup (value); }
      if (!strcmp(field_name, "severity"))
      { if (value) issue->severity = atoi (value); }
      if (!strcmp(field_name, "affects"))
      { if (value) issue->affects = atoi (value); }
      if (!strcmp(field_name, "state"))
      { if (value) issue->state = atoi (value); }
      if (!strcmp(field_name, "closenote"))
      { if (value) issue->closenote_str = strdup (value); }
      if (!strcmp(field_name, "start_sec"))
      { if (value) issue->start_sec = atol (value); }
      if (!strcmp(field_name, "etr_sec"))
      { if (value) issue->etr_sec = atol (value); }
      if (!strcmp(field_name, "end_sec"))
      { if (value) issue->end_sec = atol (value); }
      if (!strcmp(field_name, "adminlead_username"))
      { if (value) i_list_enqueue (issue->adminusername_list, strdup (value)); }
      if (!strcmp(field_name, "techlead_username"))
      { if (value) i_list_enqueue (issue->techusername_list, strdup (value)); }
    }

    /* Enqueue the issue */
    num = i_list_enqueue (cat->issue_list, issue);
    if (num != 0)
    { i_printf (1, "l_infstat_cat_sql_load_sqlcb failed to enqueue issue for cat %s at row %i, continuing", cat->name_str, x); continue; }

    /* Update Counters */
    switch (issue->severity)
    {
      case INFSTAT_SEV_NORMAL: l_infstat_issue_count_normal_inc ();
                               break;
      case INFSTAT_SEV_TRIVIAL: l_infstat_issue_count_trivial_inc ();
                                break; 
      case INFSTAT_SEV_IMPAIRED: l_infstat_issue_count_impaired_inc ();
                                 break;
      case INFSTAT_SEV_OFFLINE: l_infstat_issue_count_offline_inc ();
                                break;
    } 
  }

  /* Call the callback */
  callback->func (self, 1, callback->passdata);
  
  /* Cleanup */
  i_callback_free (callback);
  i_pg_async_conn_close (conn);

  return 0;
}

/* SQL Add */

int l_infstat_issue_sql_add (i_resource *self, l_infstat_issue *issue)
{
  int num;
  char *cat_esc;
  char *headline_esc;
  char *desc_esc;
  char *closenote_esc;
  char *adminlead_esc;
  char *techlead_esc;
  char *command;
  char *action_str;
  char *username;
  i_pg_async_conn *conn;
 
  /* Connect */

  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_infstat_cat_sql_add failed to connect to SQL database to add new issue for category %s", issue->cat->name_str); return -1; }

  /* Form command string */
  cat_esc = i_postgres_escape (issue->cat->name_str);
  headline_esc = i_postgres_escape (issue->headline_str);
  desc_esc = i_postgres_escape (issue->desc_str);
  if (issue->closenote_str)
  { closenote_esc = i_postgres_escape (issue->closenote_str); }
  else
  { closenote_esc = strdup (""); }
  i_list_move_head (issue->adminusername_list);
  username = i_list_restore (issue->adminusername_list);
  if (username) { adminlead_esc = i_postgres_escape (username); }
  else { adminlead_esc = strdup (""); }
  i_list_move_head (issue->techusername_list);
  username = i_list_restore (issue->techusername_list);
  if (username) { techlead_esc = i_postgres_escape (username); }
  else { techlead_esc = strdup (""); }
  
  asprintf (&command, "INSERT INTO infstat_issues (category, headline, descr, severity, affects, state, closenote, start_sec, etr_sec, end_sec, adminlead_username, techlead_username) VALUES ('%s', '%s', '%s', '%i', '%i', '%i', '%s', '%lu', '%li', '%li', '%s', '%s')",
    cat_esc, headline_esc, desc_esc, issue->severity, issue->affects, issue->state, closenote_esc, issue->start_sec, issue->etr_sec, issue->end_sec, adminlead_esc, techlead_esc);
  if (cat_esc) free (cat_esc);
  if (headline_esc) free (headline_esc);
  if (desc_esc) free (desc_esc);
  if (closenote_esc) free (closenote_esc);
  if (adminlead_esc) free (adminlead_esc);
  if (techlead_esc) free (techlead_esc);

  /* Execute command */
  asprintf (&action_str, "Add new issue to category %s", issue->cat->name_str);
  num = i_pg_async_query_exec (self, conn, command, 0, l_infstat_issue_sql_callback, action_str);
  free (command);  
  if (num != 0)
  {
    i_printf (1, "l_infstat_issue_sql_add failed to execute INSERT command to add category %s", issue->cat->name_str);
    i_pg_async_conn_close (conn);
    free (action_str);
    return -1;
  } 

  return 0;
}

/* Update */

int l_infstat_issue_sql_update (i_resource *self, l_infstat_issue *issue)
{
  int num;
  char *cat_esc;
  char *headline_esc;
  char *desc_esc;
  char *closenote_esc;
  char *adminlead_esc;
  char *techlead_esc;
  char *command;
  char *action_str;
  char *username;
  i_pg_async_conn *conn;

  /* Connect */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_infstat_issue_sql_edit failed to connect to SQL database to update issue %li in category %s", issue->id, issue->cat->name_str); return -1; }

  /* Form command string */
  cat_esc = i_postgres_escape (issue->cat->name_str);
  headline_esc = i_postgres_escape (issue->headline_str);
  desc_esc = i_postgres_escape (issue->desc_str);
  if (issue->closenote_str)
  { closenote_esc = i_postgres_escape (issue->closenote_str); }
  else
  { closenote_esc = strdup (""); }
  i_list_move_head (issue->adminusername_list);
  username = i_list_restore (issue->adminusername_list);
  if (username) { adminlead_esc = i_postgres_escape (username); }
  else { adminlead_esc = strdup (""); }
  i_list_move_head (issue->techusername_list);
  username = i_list_restore (issue->techusername_list);
  if (username) { techlead_esc = i_postgres_escape (username); }
  else { techlead_esc = strdup (""); }

  asprintf (&command, "UPDATE infstat_issues SET category='%s', headline='%s', descr='%s', severity='%i', affects='%i', state='%i', closenote='%s', start_sec='%li', etr_sec='%li', end_sec='%li', adminlead_username='%s', techlead_username='%s' WHERE id='%li'",
    cat_esc, headline_esc, desc_esc, issue->severity, issue->affects, issue->state, closenote_esc, issue->start_sec, issue->etr_sec, issue->end_sec, adminlead_esc, techlead_esc, issue->id);
  if (cat_esc) free (cat_esc);
  if (headline_esc) free (headline_esc);
  if (desc_esc) free (desc_esc);
  if (closenote_esc) free (closenote_esc);
  if (adminlead_esc) free (adminlead_esc);
  if (techlead_esc) free (techlead_esc);

  /* Execute command */
  asprintf (&action_str, "Update issue %li category %s", issue->id, issue->cat->name_str);
  num = i_pg_async_query_exec (self, conn, command, 0, l_infstat_issue_sql_callback, action_str);
  free (command);
  if (num != 0)
  {
    i_printf (1, "l_infstat_cat_sql_edit failed to execute UPDATE command for issue %li in category %s", issue->id, issue->cat->name_str);
    i_pg_async_conn_close (conn);
    free (action_str);
    return -1;
  }

  return 0;
}

/* SQL Callback */

int l_infstat_issue_sql_callback (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  char *action_str = passdata;

  if (!result)
  {
    i_printf (1, "l_infstat_issue_sql_callback query failed, recvd NULL result during operation code %i (%s)", operation, action_str);
    i_pg_async_conn_close (conn);
    free (action_str);
    return -1;
  }

  if (PQresultStatus(result) != PGRES_COMMAND_OK)
  { i_printf (1, "l_infstat_issue_sql_callback query failed, at operation %i - %s (%s)", operation, action_str, PQresultErrorMessage (result)); }

  i_pg_async_conn_close (conn);
  free (action_str);

  return 0;
}

