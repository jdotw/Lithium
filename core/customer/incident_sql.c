#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/socket.h>
#include <induction/callback.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/data.h>
#include <induction/incident.h>

#include "case.h"
#include "incident.h"

/* Incident SQL Functions */

/* Insert Incident */

int l_incident_sql_insert (i_resource *self, i_incident *inc)
{
  int num;
  char *query;
  char *raised_valstr;
  char *cleared_valstr;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_incident_sql_insert failed to open SQL database connection"); return -1; }

  /* Valstrs */
  if (inc->raised_valstr)
  { asprintf (&raised_valstr, "'%s'", inc->raised_valstr); }
  else
  { raised_valstr = strdup ("NULL"); }
  if (inc->cleared_valstr)
  { asprintf (&cleared_valstr, "'%s'", inc->cleared_valstr); }
  else
  { cleared_valstr = strdup ("NULL"); }

  /* Create query */
  asprintf (&query, "INSERT INTO incidents (state, start_sec, end_sec, type, cust_name, cust_desc, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, adminstate, opstate, caseid, raised_valstr, cleared_valstr) VALUES ('%i', '%li', '%li', '%i', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%i', '%i', '%li', %s, %s)",
    inc->state, inc->start_tv.tv_sec, inc->end_tv.tv_sec, inc->type, 
    inc->ent->cust_name, inc->ent->cust_desc, inc->ent->site_name, inc->ent->site_desc, inc->ent->site_suburb,
    inc->ent->dev_name, inc->ent->dev_desc, inc->ent->cnt_name, inc->ent->cnt_desc, 
    inc->ent->obj_name, inc->ent->obj_desc, inc->ent->met_name, inc->ent->met_desc, 
    inc->ent->trg_name, inc->ent->trg_desc, inc->ent->adminstate, inc->ent->opstate, 
    inc->caseid, raised_valstr, cleared_valstr);
  free (raised_valstr);
  free (cleared_valstr);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_incident_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_incident_sql_insert failed to execute INSERT for incident %li", inc->id); return -1; }
  
  return 0;
}

/* Update Incident */

int l_incident_sql_update (i_resource *self, i_incident *inc)
{
  int num;
  char *query;
  char *raised_valstr;
  char *cleared_valstr;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);  
  if (!conn)
  { i_printf (1, "l_incident_sql_update failed to open SQL database connection"); return -1; }

  /* Valstrs */
  if (inc->raised_valstr)
  { asprintf (&raised_valstr, "'%s'", inc->raised_valstr); }
  else
  { raised_valstr = strdup ("NULL"); }
  if (inc->cleared_valstr)
  { asprintf (&cleared_valstr, "'%s'", inc->cleared_valstr); }
  else
  { cleared_valstr = strdup ("NULL"); }

  /* Create query */
  asprintf (&query, "UPDATE incidents SET state='%i', start_sec='%li', end_sec='%li', type='%i', cust_name='%s', cust_desc='%s', site_name='%s', site_desc='%s', site_suburb='%s', dev_name='%s', dev_desc='%s', cnt_name='%s', cnt_desc='%s', obj_name='%s', obj_desc='%s', met_name='%s', met_desc='%s', trg_name='%s', trg_desc='%s', adminstate='%i', opstate='%i', caseid='%li', raised_valstr=%s, cleared_valstr=%s WHERE id='%li'",
    inc->state, inc->start_tv.tv_sec, inc->end_tv.tv_sec, inc->type,
    inc->ent->cust_name, inc->ent->cust_desc, inc->ent->site_name, inc->ent->site_desc, inc->ent->site_suburb, 
    inc->ent->dev_name, inc->ent->dev_desc, inc->ent->cnt_name, inc->ent->cnt_desc, 
    inc->ent->obj_name, inc->ent->obj_desc, inc->ent->met_name, inc->ent->met_desc, 
    inc->ent->trg_name, inc->ent->trg_desc, inc->ent->adminstate, inc->ent->opstate, 
    inc->caseid, raised_valstr, cleared_valstr, inc->id);
  free (raised_valstr);
  free (cleared_valstr);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_incident_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_incident_sql_insert failed to execute UPDATE for incident %li", inc->id); return -1; }
  
  return 0;
}

/* Generic SQL Callback */

int l_incident_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_incident_sql_cb failed to execute query"); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);
    
  return 0;
}

/* SQL Load */

i_callback* l_incident_sql_list (i_resource *self, char *id_str, char *state_str, char *startupper_str, char *startlower_str, char *endupper_str, char *endlower_str, char *type_str, char *cust_name, char *site_name, char *dev_name, char *cnt_name, char *obj_name, char *met_name, char *trg_name, char *adminstate_str, char *opstate_str, char *caseid_str, int max_count, int (*cbfunc) (), void *passdata)
{
  int num;
  char *str;
  char *query;
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
  { i_printf (1, "l_incident_sql_list failed to open SQL database connection"); return NULL; }

  /* Query terms */
  termlist = i_list_create ();
  i_list_set_destructor (termlist, free);
  if (id_str) { asprintf (&str, "id='%s'", id_str); i_list_enqueue (termlist, str); }
  if (state_str) { asprintf (&str, "state='%s'", state_str); i_list_enqueue (termlist, str); }
  if (startupper_str) { asprintf (&str, "start_sec < '%s'", startupper_str); i_list_enqueue (termlist, str); }
  if (startlower_str) { asprintf (&str, "start_sec > '%s'", startlower_str); i_list_enqueue (termlist, str); }
  if (endupper_str) { asprintf (&str, "end_sec < '%s'", endupper_str); i_list_enqueue (termlist, str); }
  if (endlower_str) { asprintf (&str, "end_sec > '%s'", endlower_str); i_list_enqueue (termlist, str); }
  if (type_str) { asprintf (&str, "type='%s'", type_str); i_list_enqueue (termlist, str); }   /* Incident type, NOT entity type */
  if (cust_name) { asprintf (&str, "cust_name='%s'", cust_name); i_list_enqueue (termlist, str); }
  if (site_name) { asprintf (&str, "site_name='%s'", site_name); i_list_enqueue (termlist, str); }
  if (dev_name) { asprintf (&str, "dev_name='%s'", dev_name); i_list_enqueue (termlist, str); }
  if (cnt_name) { asprintf (&str, "cnt_name='%s'", cnt_name); i_list_enqueue (termlist, str); }
  if (obj_name) { asprintf (&str, "obj_name='%s'", obj_name); i_list_enqueue (termlist, str); }
  if (met_name) { asprintf (&str, "met_name='%s'", met_name); i_list_enqueue (termlist, str); }
  if (trg_name) { asprintf (&str, "trg_name='%s'", trg_name); i_list_enqueue (termlist, str); }
  if (adminstate_str) { asprintf (&str, "adminstate='%s'", adminstate_str); i_list_enqueue (termlist, str); }
  if (opstate_str) { asprintf (&str, "opstate='%s'", opstate_str); i_list_enqueue (termlist, str); }
  if (caseid_str) { asprintf (&str, "caseid='%s'", caseid_str); i_list_enqueue (termlist, str); }

  /* Query string */
  if (termlist->size > 0)
  {
    int termindex = 0;
    char *termstr;
    char *sepstr;
    char *tmpstr;

    asprintf (&query, "SELECT id, state, start_sec, end_sec, type, cust_name, cust_desc, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, adminstate, opstate, caseid, raised_valstr, cleared_valstr FROM incidents WHERE");
    for (i_list_move_head(termlist); (termstr=i_list_restore(termlist))!=NULL; i_list_move_next(termlist))
    {
      if (termindex == 0) sepstr = " ";
      else sepstr = " AND ";

      asprintf (&tmpstr, "%s%s%s", query, sepstr, termstr);
      free (query);
      query = tmpstr;
      termindex++;
    }

    asprintf (&tmpstr, "%s ORDER BY start_sec DESC LIMIT %i", query, max_count);
    free (query);
    query = tmpstr;
  }
  else
  { asprintf (&query, "SELECT id, state, start_sec, end_sec, type, customer, site, device, container, object, metric, trigger, adminstate, opstate, caseid, raised_valstr, cleared_valstr FROM incidents ORDER BY start_sec LIMIT 200"); }
  i_list_free (termlist);   /* Frees strings by destructor */

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_incident_sql_list_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_incident_sql_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }

  return cb;
}

int l_incident_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
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
    i_printf (1, "l_incident_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  list = i_list_create ();
  i_list_set_destructor (list, i_incident_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *state_str;
    char *startsec_str;
    char *endsec_str;
    char *type_str;   /* Incident type, NOT entity type */
    char *cust_name;
    char *cust_desc;
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
    char *adminstate_str;
    char *opstate_str;
    char *caseid_str;
    char *raised_valstr;
    char *cleared_valstr;
    i_incident *inc;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    state_str = PQgetvalue (res, row, 1);
    startsec_str = PQgetvalue (res, row, 2);
    endsec_str = PQgetvalue (res, row, 3);
    type_str = PQgetvalue (res, row, 4);  /* Incident type, NOT entity type */
    cust_name = PQgetvalue (res, row, 5);
    cust_desc = PQgetvalue (res, row, 6);
    site_name = PQgetvalue (res, row, 7);
    site_desc = PQgetvalue (res, row, 8);
    site_suburb = PQgetvalue (res, row, 9);
    dev_name = PQgetvalue (res, row, 10);
    dev_desc = PQgetvalue (res, row, 11);
    cnt_name = PQgetvalue (res, row, 12);
    cnt_desc = PQgetvalue (res, row, 13);
    obj_name = PQgetvalue (res, row, 14);
    obj_desc = PQgetvalue (res, row, 15);
    met_name = PQgetvalue (res, row, 16);
    met_desc = PQgetvalue (res, row, 17);
    trg_name = PQgetvalue (res, row, 18);
    trg_desc = PQgetvalue (res, row, 19);
    adminstate_str = PQgetvalue (res, row, 20);
    opstate_str = PQgetvalue (res, row, 21);
    caseid_str = PQgetvalue (res, row, 22);
    raised_valstr = PQgetvalue (res, row, 23);
    cleared_valstr = PQgetvalue (res, row, 24);

    /* Create incident */
    inc = i_incident_create ();
    if (id_str) inc->id = atol (id_str);
    if (state_str) inc->state = atoi (state_str);
    if (startsec_str) inc->start_tv.tv_sec = atol (startsec_str);
    if (endsec_str) inc->end_tv.tv_sec = atol (endsec_str);
    if (type_str) inc->type = atoi (type_str);      /* Incident type, NOT entity type */
    inc->ent = i_entity_descriptor_create ();
    inc->ent->type = ENT_TRIGGER;
    if (cust_name) inc->ent->cust_name = strdup (cust_name);
    if (cust_desc) inc->ent->cust_desc = strdup (cust_desc);
    if (site_name) inc->ent->site_name = strdup (site_name);
    if (site_desc) inc->ent->site_desc = strdup (site_desc);
    if (site_suburb) inc->ent->site_suburb = strdup (site_suburb);
    if (dev_name) inc->ent->dev_name = strdup (dev_name);
    if (dev_desc) inc->ent->dev_desc = strdup (dev_desc);
    if (cnt_name) inc->ent->cnt_name = strdup (cnt_name);
    if (cnt_desc) inc->ent->cnt_desc = strdup (cnt_desc);
    if (obj_name) inc->ent->obj_name = strdup (obj_name);
    if (obj_desc) inc->ent->obj_desc = strdup (obj_desc);
    if (met_name) inc->ent->met_name = strdup (met_name);
    if (met_desc) inc->ent->met_desc = strdup (met_desc);
    if (trg_name) inc->ent->trg_name = strdup (trg_name);
    if (trg_desc) inc->ent->trg_desc = strdup (trg_desc);
    if (adminstate_str) inc->ent->adminstate = atoi (adminstate_str);
    if (opstate_str) inc->ent->opstate = atoi (opstate_str);
    if (caseid_str) inc->caseid = atol (caseid_str);
    if (raised_valstr) inc->raised_valstr = strdup (raised_valstr);
    if (cleared_valstr) inc->cleared_valstr = strdup (cleared_valstr);

    /* Enqueue */
    i_list_enqueue (list, inc);
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

