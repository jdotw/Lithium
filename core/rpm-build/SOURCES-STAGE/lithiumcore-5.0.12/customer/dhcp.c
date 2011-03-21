#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>

#include "dhcp.h"

/* 
 * ISC DHCP Management Sub-System - "Basement"
 */

static int static_enabled = 0;

int l_dhcp_enabled ()
{ return static_enabled; }

/* Enable / Disable */

int l_dhcp_enable (i_resource *self)
{
  int num;
  PGconn *pgconn;
  PGresult *result;

  /* Set state */
  static_enabled = 1;

  /*
   * Initialise SQL 
   */

  /* Connect to SQL db */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_dhcp_enable failed to connect to postgres database"); l_dhcp_disable (self); return -1; }

  /* Check dhcp_subnets SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'dhcp_subnets' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* dhcp_subnets table not in database */
    result = PQexec (pgconn, "CREATE TABLE dhcp_subnets (id serial, site varchar, network varchar, mask varchar, router varchar, domain varchar, options varchar)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_dhcp_enable failed to create dhcp_items table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Check dhcp_ranges SQL table */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'dhcp_ranges' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* dhcp_pools table not in database */
    result = PQexec (pgconn, "CREATE TABLE dhcp_ranges (id serial, subnet_id integer, start_ip varchar, end_ip varchar)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_dhcp_enable failed to create dhcp_ranges table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Get subnet sequence */
  result = PQexec (pgconn, "SELECT nextval('dhcp_subnets_id_seq')");
  if (result && PQresultStatus(result) == PGRES_TUPLES_OK && (PQntuples(result)) > 0)
  {
    char *id_str = PQgetvalue (result, 0, 0);
    if (id_str) l_dhcp_subnet_id_setcurrent (atol(id_str));
  }
  PQclear(result);

  /* Get range sequence */
  result = PQexec (pgconn, "SELECT nextval('dhcp_ranges_id_seq')");
  if (result && PQresultStatus(result) == PGRES_TUPLES_OK && (PQntuples(result)) > 0)
  {
    char *id_str = PQgetvalue (result, 0, 0);
    if (id_str) l_dhcp_range_id_setcurrent (atol(id_str));
  }
  PQclear(result);

  /* Close SQL conn */
  i_pg_close (pgconn);

  /*
   * Load Subnets 
   */

  num = l_dhcp_subnet_loadall (self);
  if (num != 0)
  { i_printf (1, "l_dhcp_enable failed to load all subnets"); l_dhcp_disable (self); return -1; }

  /*
   * Update Configuration
   */

  num = l_dhcp_daemon_writeconf (self);
  if (num != 0)
  { i_printf (1, "l_dhcp_enable failed to write DHCP daemon configuration"); l_dhcp_disable (self); return -1; }

  /*
   * (Re)start DHCP Daemon
   */

  num = l_dhcp_daemon_restart (self);
  if (num != 0)
  { i_printf (1, "l_dhcp_enable failed to (re)start DHCP daemon"); l_dhcp_disable (self); return -1; }

  return 0;
}

int l_dhcp_disable (i_resource *self)
{
  /* Free subnet list */
  l_dhcp_subnet_list_free ();

  /* Stop DHCP Daemon */
  l_dhcp_daemon_stop (self);

  return 0;
}
