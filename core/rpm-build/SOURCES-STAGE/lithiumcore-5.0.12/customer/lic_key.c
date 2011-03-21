#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>

#include "navtree.h"
#include "lic.h"

/* 
 * License Related Functions
 */

/* Add/Remove */

int l_lic_key_add (i_resource *self, char *key_str)
{
  int num;
  char *query;
  char *key_esc;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open (self, "lithium");
  if (!conn)
  { i_printf (1, "l_lic_key_add failed to open SQL db connection"); return -1; }

  /* Create query */
  key_esc = i_postgres_escape (key_str);
  asprintf (&query, "INSERT INTO license_keys (key) VALUES ('%s')", key_esc);
  free (key_esc);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_lic_key_sqlcb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_lic_key_add failed to execute INSERT"); return -1; }

  /* Reset entitlements */
  l_lic_reset_entitlement (self);
  
  return 0;
}

int l_lic_key_remove (i_resource *self, long id)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open conn */
  conn = i_pg_async_conn_open (self, "lithium");
  if (!conn)
  { i_printf (1, "l_lic_key_remove failed to open SQL db connection"); return -1; }
  
  /* Create query */
  asprintf (&query, "DELETE FROM license_keys WHERE id='%li'", id);
    
  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_lic_key_sqlcb, NULL);
  free (query);
  if (num != 0)
  { i_printf (1, "l_lic_key_remove failed to execute INSERT"); return -1; }

  /* Reset entitlements */
  l_lic_reset_entitlement (self);
  
  return 0;

}

/* SQL Callback */

int l_lic_key_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_lic_key_sqlcb failed to execute query (%s)", PQresultErrorMessage (result)); return -1; }

  /* Close conn */
  i_pg_async_conn_close (conn);

  /* Reset entitlement */
  l_lic_reset_entitlement (self);

  return 0;
}

