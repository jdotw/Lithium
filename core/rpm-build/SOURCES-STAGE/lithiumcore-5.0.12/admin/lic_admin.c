#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#include <induction/path.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>

#include "lic_admin.h"
#include "../customer/lic.h"

/* 
 * License Related Functions
 */

/* Customer Count */

static int static_max_customers = 0;

int l_lic_max_customers ()
{ return static_max_customers; }

int l_lic_initadmin (i_resource *self)
{
  i_list *lic_list = l_lic_loadkeys (self);
  l_lic_key *lic;
  for (i_list_move_head(lic_list); (lic=i_list_restore(lic_list))!=NULL; i_list_move_next(lic_list))
  {
    if (strcmp(lic->type_str, "RACN") == 0)
    {
      /* ACN License present, limit the number of customers */
      static_max_customers = lic->volume / 5;
    }
  }

  return 0;
}

/* Initialise SQL */

int l_lic_initsql (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  /* Connect to SQL db */
  pgconn = i_pg_connect (self, "lithium");
  if (!pgconn)
  { i_printf (1, "l_lic_initsql failed to connect to postgres database"); return -1; }

  /* Check licenses SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'license_keys' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* licenses table not in database */
    result = PQexec (pgconn, "CREATE TABLE license_keys (id serial, key varchar, cust_name varchar)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_lic_initsql failed to create sites table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* 5.0.0 - Added 'cust_name' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='license_keys' AND column_name='cust_name' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_lic_initsql version-specific check: 'cust_name' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE license_keys ADD COLUMN cust_name varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_lic_initsql failed to add uuid column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

