#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "postgresql.h"
#include "files.h"
#include "list.h"
#include "configfile.h"
#include "hierarchy.h"
#include "customer.h"

static int static_enabled;
static i_list *static_conn_list;
static i_pg_async_conn *static_customer_conn = NULL;

i_pg_async_conn* i_pg_async_conn_persistent ()
{ return static_customer_conn; }

/* ###################################
 * Enable / Disable Functions
 */

int i_pg_async_conn_enable (i_resource *self)
{
  if (static_enabled == 1)
  { i_printf (1, "i_pg_async_conn_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  static_conn_list = i_list_create ();
  if (!static_conn_list)
  { i_printf (1, "i_pg_async_conn_enable failed to creates static_conn_list"); i_pg_async_conn_disable (self); return -1; }
  i_list_set_destructor (static_conn_list, i_pg_async_conn_free);

  return 0;
}

int i_pg_async_conn_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "i_pg_async_conn_disable warning, sub-system already disabled"); return 0; }
  
  static_enabled = 0;

  if (static_conn_list)
  { i_list_free (static_conn_list); static_conn_list = NULL; }

  return 0;
}

/* ###################################
 * Connection Related Functions
 */

/* Struct Manipulation */

i_pg_async_conn* i_pg_async_conn_create ()
{
  i_pg_async_conn *conn;

  conn = (i_pg_async_conn *) malloc (sizeof(i_pg_async_conn));
  if (!conn)
  { i_printf (1, "i_pg_async_conn_create failed to malloc conn struct"); return NULL; }
  memset (conn, 0, sizeof(i_pg_async_conn));

  return conn;
}

void i_pg_async_conn_free (void *connptr)
{
  i_pg_async_conn *conn = connptr;

  if (!conn) return;

  if (conn->query)
  { i_pg_async_query_free (conn); }
  if (conn->pgconn)
  { i_pg_async_conn_close (conn); }

  free (conn);
}

/* List Related */

i_list* i_pg_async_conn_list ()
{ 
  if (static_enabled == 0)
  { return NULL; }

  return static_conn_list;
}

/* Open / Close */

i_pg_async_conn* i_pg_async_conn_open (i_resource *self, char *dbname)
{
  int num;
  char *username;
  char *password;
  char *host;
  char *port;
  i_pg_async_conn *conn;
  
  if (static_enabled == 0)
  { i_printf (1, "i_pg_async_conn_open failed. i_pg_async sub-system not enabled"); return NULL; }
  if (!self)
  { i_printf (1, "i_pg_async_conn_open called with NULL self"); return NULL; }

  username = i_configfile_get (self, NODECONF_FILE, "postgresql", "username", 0);
  if (!username) 
  { i_printf (1, "i_pg_async_conn_open did not find username in postgresql of %s", NODECONF_FILE); return NULL; }

  password = i_configfile_get (self, NODECONF_FILE, "postgresql", "password", 0);
  if (!password) 
  { i_printf (1, "i_pg_async_conn_open did not find password in postgresql of %s", NODECONF_FILE); free (username); return NULL; }
  
  host = i_configfile_get (self, NODECONF_FILE, "postgresql", "host", 0);
  if (!host) 
  { i_printf (1, "i_pg_async_conn_open did not find host in postgresql of %s", NODECONF_FILE); free (username); free (password); return NULL; }

  port = i_configfile_get (self, NODECONF_FILE, "postgresql", "port", 0);
  if (!port) 
  { i_printf (1, "i_pg_async_conn_open did not find port in postgresql of %s", NODECONF_FILE); free (username); free (password); free (host); return NULL; }

  conn = i_pg_async_conn_create ();
  if (!conn)
  { i_printf (1, "i_pg_async_conn_open failed to create conn struct"); free (username); free (password); free (host); free (port); return NULL; }

  conn->pgconn = PQsetdbLogin(host, port, NULL, NULL, dbname, username, password);
  free (username);
  free (password);
  free (host);
  free (port);

  if (PQstatus(conn->pgconn) == CONNECTION_BAD)
  {
    i_printf (1, "i_pg_async_conn_open failed to connect to database %s (%s)", dbname, PQerrorMessage(conn->pgconn));
    PQfinish(conn->pgconn);
    return NULL;
  }

  num = PQsetnonblocking (conn->pgconn, 1);
  if (num != 0)
  {
    i_printf (1, "i_pg_async_conn_open failed to set non-blocking state for database %s (%s)", dbname, PQerrorMessage(conn->pgconn));
    PQfinish(conn->pgconn);
    return NULL;
  }

  num = i_list_enqueue (static_conn_list, conn);
  if (num != 0)
  { 
    i_printf (1, "i_pg_async_conn_open failed to enqueue connection into static_conn_list"); 
    PQfinish(conn->pgconn);
    return NULL;
  }

  /* Set usage count */
  conn->usage = 1;
  
  return conn;
}

void i_pg_async_conn_close (void *connptr)
{
  /* This function just closes the 
   * connection. The conn struct will
   * be freed along with any active request
   * in the next i_loop iteration
   */

  i_pg_async_conn *conn = connptr;

  if (!conn) return;  

  conn->usage--;
  
  if (conn->usage == 0)
  {
    PQfinish(conn->pgconn);
    conn->pgconn = NULL;
    if (conn == static_customer_conn)
    { static_customer_conn = NULL; }
  }

  return;
}

/* Customer Specific */

i_pg_async_conn* i_pg_async_conn_open_customer (i_resource *self) 
{
  /* Connect to a customers DB */
  char *dbname;
  i_pg_async_conn *conn;

  if (static_enabled == 0)
  { i_printf (1, "i_pg_async_conn_open_customer failed. i_pg_async sub-system not enabled"); return NULL; }
  
  if (!self || !self->customer_id)
  { i_printf (1, "i_pg_async_conn_open_customer called with either NULL self or NULL self->customer_id"); return NULL; }

  /* Check for a customer conn already open */
  if (static_customer_conn)
  { static_customer_conn->usage++; return static_customer_conn; }

  /* Check which db to ue */
  if (self->hierarchy->cust && self->hierarchy->cust->use_lithium_db == 1)
  {
    dbname = strdup("lithium");
  }
  else
  {
    asprintf (&dbname, "customer_%s", self->customer_id);
  }

  /* Open conn */
  conn = i_pg_async_conn_open (self, dbname);
  free (dbname);
  if (!conn)
  {
    i_printf (1, "i_pg_async_conn_open_customer failed to connect to customer %s's database", self->customer_id);
    return NULL;
  }

  static_customer_conn = conn;

  return conn;
}

