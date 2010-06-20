#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <uuid/uuid.h>
#include <ctype.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/entity_xmlsync.h>
#include <induction/timer.h>
#include <induction/customer.h>
#include <induction/hierarchy.h>

#include "snmpagent.h"
#include "customer.h"
#include "lic_admin.h"

/* 
 * Customer Related Functions
 */

static i_list *static_list = NULL;

/* List Manipulation */

i_list* l_customer_list ()
{ return static_list; }

i_list* l_customer_list_create ()
{
  static_list = i_list_create ();
  if (!static_list)
  { i_printf (1, "l_customer_list_create failed to create static_list"); return NULL; }
  i_list_set_destructor (static_list, i_entity_free);

  return static_list;
}

int l_customer_list_free ()
{
  if (static_list)
  { i_list_free (static_list); static_list = NULL; }

  return 0;
}

/* Get */

i_customer* l_customer_get (i_resource *self, char *name_str)
{
  i_customer *cust;

  for (i_list_move_head(static_list); (cust=i_list_restore(static_list))!=NULL; i_list_move_next(static_list))
  { 
    if (!strcmp(cust->name_str, name_str)) return cust; 
  }

  return NULL;
}

/* Add */

i_customer* l_customer_add (i_resource *self, char *name_str, char *desc_str, char *baseurl_str)
{
  int num;
  i_list *cust_list;
  i_customer *cust;
  i_pg_async_conn *conn;

  /* Check/Create list */
  cust_list = l_customer_list ();
  if (!cust_list)
  { 
    cust_list = l_customer_list_create ();
    if (!cust_list)
    { i_printf (1, "l_customer_add failed to create customer list"); return NULL; }
  }

  /* Create struct */
  cust = i_customer_create (name_str, desc_str, baseurl_str);
  if (!cust)
  { i_printf (1, "l_customer_add failed to create customer struct"); return NULL; }
  uuid_generate (cust->uuid);
  
  /* Enqueue */
  num = i_list_enqueue (cust_list, cust);
  if (num != 0)
  { 
    i_printf (1, "l_customer_add failed to enqueue customer %s", cust->name_str); 
    i_entity_free (ENTITY(cust));
    return NULL;
  }
  
  /* Enable XML Sync */
  if (l_snmpagent_is_enabled())
  { i_entity_xmlsync_enable (self, ENTITY(cust), 60, l_snmpagent_xmlsync_delegate); }

  /* Open SQL Conn */
  conn = i_pg_async_conn_open (self, "lithium");
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "INSERT INTO customers (name, descr, baseurl) VALUES ('%s', '%s', '%s')", cust->name_str, cust->desc_str, cust->baseurl_str);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_customer_sqlcb, "add");
    free (query);
    if (num != 0)
    { i_printf (1, "l_customer_add warning, failed to execute SQL insert query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_customer_add failed to open SQL conn for customer %s", cust->name_str); }

  /* (Re)start Resource */
  num = l_customer_res_restart (self, cust);
  if (num != 0)
  { i_printf (1, "l_customer_add warning, failed (re)start customer resource %s", cust->name_str); }

  return cust;
}

/* Update */

int l_customer_update (i_resource *self, i_customer *cust)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open (self, "lithium");
  if (!conn)
  { i_printf (1, "l_customer_update failed to open SQL connection for customer %s", cust->name_str); return -1; }

  /* Create query */
  asprintf (&query, "UPDATE customers SET descr='%s', baseurl='%s' WHERE name='%s'", cust->desc_str, cust->baseurl_str, cust->name_str); 

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_customer_sqlcb, "update");
  free (query);
  if (num != 0)
  { i_printf (1, "l_customer_update failed to execute UPDATE query for customer %s", cust->name_str); i_pg_async_conn_close (conn); return -1; }

  /* Restart Customer Resource */
  num = l_customer_res_restart (self, cust);
  if (num != 0)
  { i_printf (1, "l_customer_update warning, failed to restart customer resource %s", cust->name_str); }

  return 0;
}

/* Remove */

int l_customer_remove (i_resource *self, i_customer *cust)
{
  int num;
  i_list *cust_list;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open (self, "lithium");
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "DELETE FROM customers WHERE name='%s'", cust->name_str);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_customer_sqlcb, "remove");
    free (query);
    if (num != 0)
    { i_printf (1, "l_customer_remove warning, failed to execute DELETE query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_customer_remove failed to open SQL connection"); }

  /* Destroy customer resource */
  l_customer_res_destroy (self, cust);

  /* Remove (and free) cust from list */
  cust_list = l_customer_list ();
  num = i_list_search (cust_list, cust);
  if (num == 0)
  { i_list_delete (cust_list); }
  else
  { i_printf (1, "l_customer_remove warning, failed to find cust in cust list"); }

  return 0;
}

/* SQL Callback */

int l_customer_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_customer_sqlcb failed to execute query during %s operation", (char *) passdata); }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Load */

int l_customer_loadall (i_resource *self)
{
  int num;
  int row;
  int row_count;
  PGresult *res;
  PGconn *pgconn;
  i_list *cust_list;

  /* Check/Create List */
  cust_list = l_customer_list ();
  if (!cust_list)
  {
    cust_list = l_customer_list_create ();
    if (!cust_list)
    { i_printf (1, "l_customer_loadall failed to create cust_list"); return -1; }
  }

  /* Connect to SQL */
  pgconn = i_pg_connect (self, "lithium");
  if (!pgconn)
  { i_printf (2, "l_customer_loadall failed to connect to postgresql database"); return -1; }

  /* Load all customers */
  if (l_lic_max_customers() > 0)
  { 
    char *query_str;
    asprintf (&query_str, "SELECT name, descr, baseurl, uuid FROM customers LIMIT %i", l_lic_max_customers());
    res = PQexec (pgconn, query_str);
    i_printf (1, "l_customer_loadall encforcing limit of %i customers", l_lic_max_customers());
  }
  else
  { res = PQexec (pgconn, "SELECT name, descr, baseurl, uuid FROM customers"); }
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
  { 
    i_printf (1, "l_customer_loadall failed to execute SELECT query for the customers table");
    if (res) { PQclear (res); res = NULL; }
    i_pg_close (pgconn);
    return -1;
  }

  /* Loop through each customer record */
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *name_str;
    char *desc_str;
    char *baseurl_str;
    char *uuid_str;
    i_customer *cust;

    /* Name */
    char *str = PQgetvalue (res, row, 0);
    name_str = strdup (str);
    unsigned int i;
    for (i=0; i < strlen(name_str); i++)
    {
      name_str[i] = tolower (name_str[i]);
    }

    /* Descr */
    desc_str = PQgetvalue (res, row, 1);

    /* BaseURL */
    baseurl_str = PQgetvalue (res, row, 2);

    /* UUID */
    uuid_str = PQgetvalue (res, row, 3);

    /* Create cust */
    cust = i_customer_create (name_str, desc_str, baseurl_str);
    free (name_str);
    name_str = NULL;
    if (!cust)
    { i_printf (1, "l_customer_loadall failed to create customer struct at customer record %i", row); continue; }

    /* Set UUID */
    uuid_parse (uuid_str, cust->uuid);

    /* Enqueue cust */
    num = i_list_enqueue (cust_list, cust);
    if (num != 0)
    { 
      i_printf (1, "l_customer_loadall failed to enqueue customer %s", cust->name_str);
      i_entity_free (ENTITY(cust));
      continue;
    }

    /* Enable XML Sync */
    if (l_snmpagent_is_enabled())
    { i_entity_xmlsync_enable (self, ENTITY(cust), 60, l_snmpagent_xmlsync_delegate); }

    /* (Re)start Customer Resource */
    num = l_customer_res_restart (self, cust);
    if (num != 0)
    { i_printf (1, "l_customer_loadall warning, failed to (re)start customer resource %s", cust->name_str); }
  }

  /* Close DB */
  if (res) { PQclear (res); res = NULL; }
  i_pg_close (pgconn);

  return 0;
}

int l_customer_loadall_retry (i_resource *self, i_timer *timer, void *passdata)
{
  int num;
  num = l_customer_loadall (self);
  if (num == 0)
  { return -1; /* Kill Timer */ }
  else
  {
    i_printf (2, "l_customer_loadall_retry failed to retry loading customers, will keep trying"); 
    return 0; 
  }
}

/* Initialise SQL */

int l_customer_initsql (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  /* Connect to SQL db */
  pgconn = i_pg_connect (self, "lithium");
  if (!pgconn)
  { i_printf (1, "l_customer_initsql failed to connect to postgres database"); return -1; }

  /* Check customers SQL table */
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'customers' AND relkind = 'r'");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* customers table not in database */
    result = PQexec (pgconn, "CREATE TABLE customers (name varchar, descr varchar, baseurl varchar, uuid varchar)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_customer_initsql failed to create customers table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Update table */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='customers' AND column_name='uuid' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* uuid column not in customers table */
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_customer_initsql version-specific check: 'uuid' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE customers ADD COLUMN uuid varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_customer_initsql failed to add username column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* Ensure each customer has a UUID */
  result = PQexec (pgconn, "SELECT name, uuid FROM customers");
  if (result && PQresultStatus(result) == PGRES_TUPLES_OK)
  {
    int row_cont = PQntuples(result);
    int row;
    for (row=0; row < row_cont; row++)
    {
      char *cust_name = PQgetvalue (result, row, 0);
      char *uuid_str = PQgetvalue (result, row, 1);
      if (!uuid_str || strlen(uuid_str) < 1)
      {
        /* Invalid UUID, reset it */
        uuid_t uuid;
        uuid_generate (uuid);
        char uuid_buf[37];
        uuid_unparse_lower (uuid, uuid_buf);
        char *query;
        asprintf (&query, "UPDATE customers SET uuid='%s' WHERE name='%s'", uuid_buf, cust_name);
        PQexec (pgconn, query);
        free (query);
      }
    }
  }
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

int l_customer_initsql_retry (i_resource *self, i_timer *timer, void *passdata)
{
  int num;
  num = l_customer_initsql (self);
  if (num == 0)
  { return -1; /* Kill Timer */ }
  else
  {
    i_printf (1, "l_customer_initsql_retry failed to retry initialising sql, will keep trying");
    return 0;
  } 
} 

