#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "postgresql.h"
#include "files.h"
#include "configfile.h"
#include "hierarchy.h"
#include "customer.h"

/* Connection related functions */

PGconn* i_pg_connect (i_resource *self, char *dbname)
{
  char *username;
  char *password;
  char *host;
  char *port;
  PGconn *conn;
  
  if (!self)
  { i_printf (1, "i_pg_connect called with either NULL self or NULL self->customer_id"); return NULL; }

  host = i_configfile_get (self, NODECONF_FILE, "postgresql", "host", 0);
  if (!host) 
  { i_printf (2, "i_pg_connect did not find host in postgresql of %s", NODECONF_FILE); return NULL; }

  port = i_configfile_get (self, NODECONF_FILE, "postgresql", "port", 0);
  if (!port) 
  { i_printf (1, "i_pg_connect warning, did not find port in postgresql of %s", NODECONF_FILE); return NULL; }

  username = i_configfile_get (self, NODECONF_FILE, "postgresql", "username", 0);
  if (!username) 
  { i_printf (1, "i_pg_connect warning, did not find username in postgresql of %s", NODECONF_FILE); }

  password = i_configfile_get (self, NODECONF_FILE, "postgresql", "password", 0);
  if (!password) 
  { i_printf (1, "i_pg_connect warning, did not find password in postgresql of %s", NODECONF_FILE); }
  
  conn = PQsetdbLogin(host, port, NULL, NULL, dbname, username, password);
  if (username) free (username);
  if (password) free (password);
  if (host) free (host);
  if (port) free (port);

  if (PQstatus(conn) == CONNECTION_BAD)
  {
    i_printf (1, "i_pg_connect failed to connect to database %s (%s)", dbname, PQerrorMessage(conn));
    i_pg_close (conn);
    return NULL;
  }
  
  return conn;
}

PGconn* i_pg_connect_customer (i_resource *self) 
{
  /* Connect to a customers DB */
  char *dbname;
  PGconn *conn;
  
  if (!self || !self->customer_id)
  { i_printf (1, "i_pg_connect called with either NULL self or NULL self->customer_id"); return NULL; }

  if (self->hierarchy->cust && self->hierarchy->cust->use_lithium_db == 1)
  {
    /* Use the 'lithium' db */
    dbname = strdup("lithium");
  }
  else
  {
    /* Use the 'customer_name' db */
    asprintf (&dbname, "customer_%s", self->customer_id);
  }

  conn = i_pg_connect (self, dbname);
  free (dbname);
  if (!conn)
  {
    i_printf (1, "i_pg_connect_customer failed to connect to customer %s's database (%s)", self->customer_id, PQerrorMessage(conn));
    return NULL;
  }

  return conn;
}


int i_pg_close (PGconn *conn)
{
  PQfinish(conn);
  return 0;
}

/* Misc functions */

int i_pg_begin (PGconn *conn)
{
  PGresult *res;
  
  res = PQexec(conn, "BEGIN");
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    i_printf (1, "i_pg_begin BEGIN command failed (%s)", PQresultErrorMessage (res));
    PQclear(res);
    return -1;
  }
  PQclear(res);

  return 0;
}

int i_pg_end (PGconn *conn)
{
  PGresult *res;

  res = PQexec(conn, "END");
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    i_printf (1, "i_pg_end END command failed (%s)", PQresultErrorMessage (res));
    PQclear(res);
    return -1;
  }
  PQclear(res);

  return 0;
}

int i_pg_commit (PGconn *conn)
{
  PGresult *res;

  res = PQexec(conn, "COMMIT");
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    i_printf (1, "i_pg_commit COMMIT command failed (%s)", PQresultErrorMessage (res));
    PQclear(res);
    return -1;
  }
  PQclear(res);

  return 0;
}

void i_postgres_pqclear_wrapper (void *resptr)
{
  PGresult *res = resptr;

  PQclear (res);
}

char* i_postgres_escape (char *in_str)
{
  char *str;
  size_t str_len;

  if (!in_str) 
  { return strdup (""); }     /* Return an empty string.. this stops '(null)' from appearing in databases */

  str = (char *) malloc ((2*strlen(in_str))+1);
  if (!str)
  { i_printf (1, "i_postgres_escape failed to malloc str"); return NULL; }
  memset (str, 0, (2*strlen(in_str))+1);

  str_len = PQescapeString (str, in_str, strlen(in_str));
  if (str_len < strlen(in_str))
  { i_printf (1, "i_postgres_escape failed, PQescapeString returned %i which is less than strlen(in_str) (%i bytes)", str_len, strlen(in_str)); free (str); return NULL; }

  return str;
}
