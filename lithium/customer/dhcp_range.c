#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>

#include "dhcp.h"

/* 
 * ISC DHCP Management Sub-System - Range Functions
 */

static long static_cur_id = 0;

/* Struct Manipulation */

l_dhcp_range* l_dhcp_range_create ()
{
  l_dhcp_range *range;

  range = (l_dhcp_range *) malloc (sizeof(l_dhcp_range));
  if (!range)
  { i_printf (1, "l_dhcp_range_create failed to malloc l_dhcp_range struct"); return NULL; }
  memset (range, 0, sizeof(l_dhcp_range));

  return range;
}

void l_dhcp_range_free (void *rangeptr)
{
  l_dhcp_range *range = rangeptr;

  if (!range) return;

  if (range->start_str) free (range->start_str);
  if (range->end_str) free (range->end_str);

  free (range);
}

/* ID Assignment */

long l_dhcp_range_id_assign ()
{ static_cur_id++; return static_cur_id; }

int l_dhcp_range_id_setcurrent (long cur_id)
{ static_cur_id = cur_id; return 0; }

/* Get */

l_dhcp_range* l_dhcp_range_get (i_resource *self, l_dhcp_subnet *subnet, long id)
{
  l_dhcp_range *range;
  
  for (i_list_move_head(subnet->range_list); (range=i_list_restore(subnet->range_list))!=NULL; i_list_move_next(subnet->range_list))
  { if (range->id == id) return range; }

  return NULL;
}

/* Add */

l_dhcp_range* l_dhcp_range_add (i_resource *self, l_dhcp_subnet *subnet, char *start_str, char *end_str)
{
  int num;
  l_dhcp_range *range;
  i_pg_async_conn *conn;

  /* Create struct */
  range = l_dhcp_range_create ();
  if (!range)
  { i_printf (1, "l_dhcp_range_add failed to create range struct"); return NULL; }
  range->start_str = strdup (start_str);
  range->end_str = strdup (end_str);
  range->subnet = subnet;
  
  /* Enqueue */
  num = i_list_enqueue (subnet->range_list, range);
  if (num != 0)
  { 
    i_printf (1, "l_dhcp_range_add failed to enqueue range %s - %s", range->start_str, range->end_str); 
    l_dhcp_range_free (range);
    return NULL;
  }
  
  /* Open SQL Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "INSERT INTO dhcp_ranges (subnet_id, start_ip, end_ip) VALUES ('%li', '%s', '%s')",
      subnet->id, range->start_str, range->end_str);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_dhcp_range_sqlcb, "add");
    free (query);
    if (num != 0)
    { i_printf (1, "l_dhcp_range_add warning, failed to execute SQL insert query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_dhcp_range_add failed to open SQL conn for range %s - %s", range->start_str, range->end_str); }

  /* Assign ID */
  range->id = l_dhcp_range_id_assign (); 

  /* Update daemon */
  l_dhcp_daemon_update (self);

  return range;
}

/* Update */

int l_dhcp_range_update (i_resource *self, l_dhcp_range *range)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_dhcp_range_update failed to open SQL connection for range %s - %s", range->start_str, range->end_str); return -1; }

  /* Create query */
  asprintf (&query, "UPDATE dhcp_ranges SET subnet_id='%li', start_ip='%s', end_ip='%s' WHERE id='%li'",
    range->subnet->id, range->start_str, range->end_str, range->id); 

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_dhcp_range_sqlcb, "update");
  free (query);
  if (num != 0)
  { i_printf (1, "l_dhcp_range_update failed to execute UPDATE query for range %s - %s", range->start_str, range->end_str); i_pg_async_conn_close (conn); return -1; }

  /* Update daemon */
  l_dhcp_daemon_update (self);

  return 0;
}

/* Remove */

int l_dhcp_range_remove (i_resource *self, l_dhcp_range *range)
{
  int num;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "DELETE FROM dhcp_ranges WHERE id='%li'", range->id);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_dhcp_range_sqlcb, "remove");
    free (query);
    if (num != 0)
    { i_printf (1, "l_dhcp_range_remove warning, failed to execute DELETE query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_dhcp_range_remove failed to open SQL connection"); }

  /* Remove (and free) range from list */
  if (range->subnet)
  {
    num = i_list_search (range->subnet->range_list, range);
    if (num == 0)
    { i_list_delete (range->subnet->range_list); }
    else
    { i_printf (1, "l_dhcp_range_remove warning, failed to find range in range list"); }
  }

  /* Update daemon */
  l_dhcp_daemon_update (self);

  return 0;
}

/* SQL Callback */

int l_dhcp_range_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_dhcp_range_sqlcb failed to execute query during %s operation", (char *) passdata); }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Load */

int l_dhcp_range_loadall (i_resource *self, l_dhcp_subnet *subnet)
{
  int num;
  int row;
  int row_count;
  char *query;
  PGresult *res;
  PGconn *pgconn;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_dhcp_range_loadall failed to connect to postgresql database"); return -1; }

  /* Create query */
  asprintf (&query, "SELECT id, start_ip, end_ip FROM dhcp_ranges WHERE subnet_id='%li'", subnet->id);

  /* Load all DHCP ranges */
  res = PQexec (pgconn, query);
  free (query);
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
  { 
    i_printf (1, "l_dhcp_range_loadall failed to execute SELECT query for the dhcp_ranges table");
    if (res) PQclear (res);
    i_pg_close (pgconn);
    return -1;
  }

  /* Loop through each range record */
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *str;
    l_dhcp_range *range;

    /* Create range */
    range = l_dhcp_range_create ();
    if (!range)
    { i_printf (1, "l_dhcp_range_loadall failed to create range struct at range record %i", row); continue; }
    range->subnet = subnet;

    /* ID */
    str = PQgetvalue (res, row, 0);
    if (str) range->id = atol (str);

    /* Start */
    str = PQgetvalue (res, row, 1);
    if (str) range->start_str = strdup (str);

    /* End */
    str = PQgetvalue (res, row, 2);
    if (str) range->end_str = strdup (str);

    /* Enqueue range */
    num = i_list_enqueue (subnet->range_list, range);
    if (num != 0)
    { 
      i_printf (1, "l_dhcp_range_loadall failed to enqueue range %s - %s", range->start_str, range->end_str);
      l_dhcp_range_free (range);
      continue;
    }
  }

  i_pg_close (pgconn);

  return 0;
}

