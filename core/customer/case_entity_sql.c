#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/container.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/callback.h>
#include <induction/postgresql.h>
#include <induction/incident.h>

#include "case.h"
#include "incident.h"

/*
 * Case System -- Entity SQL Functions
 */

/* Insert */

int l_case_entity_sql_insert (i_resource *self, long caseid, i_entity_descriptor *ent)
{
  int num;
  char *query;
  i_pg_async_conn *conn;
  struct timeval now;

  gettimeofday (&now, NULL);

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_entity_sql_insert failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "INSERT INTO case_entities (caseid, case_state, type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc, tstamp) VALUES ('%li', '%i', '%i', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%li');",
    caseid, CASE_STATE_OPEN, ent->type, ent->site_name, ent->site_desc, ent->site_suburb,
    ent->dev_name, ent->dev_desc, ent->cnt_name, ent->cnt_desc, ent->obj_name, ent->obj_desc,
    ent->met_name, ent->met_desc, ent->trg_name, ent->trg_desc, now.tv_sec);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_entity_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_entity_sql_insert failed to execute INSERT for case %li", caseid); return -1; }

  return 0;
}

/* Delete */

int l_case_entity_sql_delete (i_resource *self, long caseid, i_entity_descriptor *ent)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_entity_sql_delete failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM case_entities WHERE caseid='%li' AND type='%i' AND site_name='%s' AND dev_name='%s' AND cnt_name='%s' AND obj_name='%s' AND met_name='%s' AND trg_name='%s';",
    caseid, ent->type, ent->site_name, ent->dev_name, ent->cnt_name, ent->obj_name, ent->met_name, ent->trg_name);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_entity_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_entity_sql_delete failed to execute DELETE for case %li", caseid); return -1; }

  return 0;
}

int l_case_entity_sql_delete_device (i_resource *self, i_device *dev)
{
  /* Delete all case-bound entities for a particular device */
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);  if (!conn)
  { i_printf (1, "l_case_entity_sql_delete_device failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "DELETE FROM case_entities WHERE site_name='%s' AND dev_name='%s'", dev->site->name_str, dev->name_str);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_entity_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_entity_sql_delete_device failed to execute DELETE"); return -1; }

  return 0;
}

/* SQL Callback */

int l_case_entity_sql_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_case_entity_sql_cb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Load/Search */

i_callback* l_case_entity_sql_list (i_resource *self, char *caseid_str, char *casestate_str, char *type_str, char *site_str, char *dev_str, char *cnt_str, char *obj_str, char *met_str, char *trg_str, char *tstampupper_str, char *tstamplower_str, int (*cbfunc) (), void *passdata)
{
  int num;
  int termindex = 0;
  char *termstr;
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
  { i_printf (1, "l_case_sql_list failed to open SQL database connection"); return NULL; }

  /* Query terms */
  termlist = i_list_create ();
  i_list_set_destructor (termlist, free);
  if (caseid_str) { asprintf (&str, "caseid='%s'", caseid_str); i_list_enqueue (termlist, str); }
  if (casestate_str) { asprintf (&str, "case_state='%s'", casestate_str); i_list_enqueue (termlist, str); }
  if (type_str) { asprintf (&str, "type>='%s'", type_str); i_list_enqueue (termlist, str); }
  if (site_str) { asprintf (&str, "site_name='%s'", site_str); i_list_enqueue (termlist, str); }
  if (dev_str) { asprintf (&str, "dev_name='%s'", dev_str); i_list_enqueue (termlist, str); }
  if (cnt_str) { asprintf (&str, "cnt_name='%s'", cnt_str); i_list_enqueue (termlist, str); }
  if (obj_str) { asprintf (&str, "obj_name='%s'", obj_str); i_list_enqueue (termlist, str); }
  if (met_str) { asprintf (&str, "met_name='%s'", met_str); i_list_enqueue (termlist, str); }
  if (trg_str) { asprintf (&str, "trg_name='%s'", trg_str); i_list_enqueue (termlist, str); }
  if (termlist->size < 1)
  { i_printf (1, "l_case_entity_sql_list called with no criteria"); i_list_free (termlist); return NULL; }
  
  /* Query string */
  asprintf (&query, "SELECT caseid, case_state, type, site_name, site_desc, site_suburb, dev_name, dev_desc, cnt_name, cnt_desc, obj_name, obj_desc, met_name, met_desc, trg_name, trg_desc FROM case_entities WHERE");
  for (i_list_move_head(termlist); (termstr=i_list_restore(termlist))!=NULL; i_list_move_next(termlist))
  {
    char *sepstr;
    char *tmpstr;
      
    if (termindex == 0) sepstr = " ";
    else sepstr = " AND ";

    asprintf (&tmpstr, "%s%s%s", query, sepstr, termstr);
    free (query);
    query = tmpstr;
    termindex++;
  }
  i_list_free (termlist);   /* Frees strings by destructor */

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_entity_sql_list_cb, cb);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_entity_sql_list failed to execute SELECT query"); i_callback_free (cb); return NULL; }
 
  return cb;
}

int l_case_entity_sql_list_cb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  /* Called with the result from the SELECT query above */
  int num;
  int row;
  int row_count;
  l_case *cur_case = NULL;
  i_list *case_list;
  i_callback *cb = (i_callback *) passdata;

  /* Check result */
  if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK))
  {
    i_printf (1, "l_case_entity_sql_list_cb failed to execute query (%s)", PQresultErrorMessage (res));
    if (cb->func) cb->func (self, NULL, cb->passdata);
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  /* Loops through each record */
  case_list = i_list_create ();
  i_list_set_destructor (case_list, l_case_free);
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *caseid_str;
    char *casestate_str;
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
    caseid_str = PQgetvalue (res, row, 0);
    casestate_str = PQgetvalue (res, row, 1);
    type_str = PQgetvalue (res, row, 2);
    site_name = PQgetvalue (res, row, 3);
    site_desc = PQgetvalue (res, row, 4);
    site_suburb = PQgetvalue (res, row, 5);
    dev_name = PQgetvalue (res, row, 6);
    dev_desc = PQgetvalue (res, row, 7);
    cnt_name = PQgetvalue (res, row, 8);
    cnt_desc = PQgetvalue (res, row, 9);
    obj_name = PQgetvalue (res, row, 10);
    obj_desc = PQgetvalue (res, row, 11);
    met_name = PQgetvalue (res, row, 12);
    met_desc = PQgetvalue (res, row, 13);
    trg_name = PQgetvalue (res, row, 14);
    trg_desc = PQgetvalue (res, row, 15);

    if (!caseid_str) continue;

    /* Check for start/continuation of case */
    if (!cur_case || atol(caseid_str) != cur_case->id)
    {
      /* New case */
      cur_case = l_case_create ();
      cur_case->id = atol (caseid_str);
      cur_case->state = atoi (casestate_str);
      i_list_enqueue (case_list, cur_case);
    }

    /* Create entity */
    if (cur_case)
    {
      i_list *inc_list;
      i_incident *inc;
      i_entity_descriptor *ent;

      /* Create entity */
      ent = i_entity_descriptor_create ();
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

      /* Set entity state 
       *
       * The state of the entity is based on the incidents
       * in the incident list. Here we loop through each incident,
       * match it as far as possible, and set the states for each match 
       */
      inc_list = l_incident_list ();
      for (i_list_move_head(inc_list); (inc=i_list_restore(inc_list))!=NULL; i_list_move_next(inc_list))
      {
        /* Match and set state */
        int level;
        short inc_opstate = 0;
        short inc_adminstate = 0;
        short *ent_opstate = NULL;
        short *ent_adminstate = NULL;
        char *inc_name = NULL;
        char *ent_name = NULL;
        i_site *site;
        
        for (level=1; level < 8; level++)
        {
          /* Set variables */
          switch (level)
          {
            case 1: /* Customer */
              inc_name = inc->ent->cust_name;
              ent_name = ent->cust_name;
              inc_opstate = self->hierarchy->cust->opstate;
              ent_opstate = &ent->cust_opstate;
              inc_adminstate = self->hierarchy->cust->adminstate;
              ent_adminstate = &ent->cust_adminstate;
              break;
            case 2: /* Site */
              site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), inc->ent->site_name);
              inc_name = inc->ent->site_name;
              ent_name = ent->site_name;
              inc_opstate = site->opstate;
              ent_opstate = &ent->site_opstate;
              inc_adminstate = site->adminstate;
              ent_adminstate = &ent->site_adminstate;
              break;
            case 3: /* Device */
              inc_name = inc->ent->dev_name;
              ent_name = ent->dev_name;
              inc_opstate = inc->ent->dev_opstate;
              ent_opstate = &ent->dev_opstate;
              inc_adminstate = inc->ent->dev_adminstate;
              ent_adminstate = &ent->dev_adminstate;
              break;
            case 4: /* Container */
              inc_name = inc->ent->cnt_name;
              ent_name = ent->cnt_name;
              inc_opstate = inc->ent->cnt_opstate;
              ent_opstate = &ent->cnt_opstate;
              inc_adminstate = inc->ent->cnt_adminstate;
              ent_adminstate = &ent->cnt_adminstate;
              break;
            case 5: /* Object */
              inc_name = inc->ent->obj_name;
              ent_name = ent->obj_name;
              inc_opstate = inc->ent->obj_opstate;
              ent_opstate = &ent->obj_opstate;
              inc_adminstate = inc->ent->obj_adminstate;
              ent_adminstate = &ent->obj_adminstate;
              break;
            case 6: /* Metric */
              inc_name = inc->ent->met_name;
              ent_name = ent->met_name;
              inc_opstate = inc->ent->met_opstate;
              ent_opstate = &ent->met_opstate;
              inc_adminstate = inc->ent->met_adminstate;
              ent_adminstate = &ent->met_adminstate;
              break;
            case 7: /* Trigger */
              inc_name = inc->ent->trg_name;
              ent_name = ent->trg_name;
              inc_opstate = inc->ent->trg_opstate;
              ent_opstate = &ent->trg_opstate;
              inc_adminstate = inc->ent->trg_adminstate;
              ent_adminstate = &ent->trg_adminstate;
              break;
          }

          /* Check for name match */
          if (!strcmp(inc_name, ent_name))
          {
            /* Matched at this level */
            *ent_opstate = inc_opstate;
            *ent_adminstate = inc_adminstate;

            /* Set locals if applicable */
            if (level == ent->type)
            {
              ent->opstate = inc_opstate;
              ent->adminstate = inc_adminstate;

              /* Local has been set, no further matching needed */
              break;
            }
          }
          else
          {
            /* No match, break and ensure the loop wont continue */
            level = 8;
            break;
          }
        }
      }
        
      /* Enqueue */
      i_list_enqueue (cur_case->ent_list, ent);
    }
  }

  /* Run Callback */
  if (cb->func)
  {
    num = cb->func (self, case_list, cb->passdata);
    if (num != 0) i_list_free (case_list);
  }
  else i_list_free (case_list);

  /* Cleanup */
  i_callback_free (cb);
  i_pg_async_conn_close (conn);

  return 0;
}

/* Case State */

int l_case_entity_sql_update_casestate (i_resource *self, long caseid, unsigned short state)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_case_entity_sql_update_casestate failed to open SQL database connection"); return -1; }

  /* Create query */
  asprintf (&query, "UPDATE case_entities SET case_state='%i' WHERE caseid='%li'", state, caseid);

  /* Exec query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_case_entity_sql_cb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_case_entity_sql_update_casestate failed to execute INSERT for case %li", caseid); return -1; }

  return 0;
}

