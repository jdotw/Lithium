#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/hierarchy.h>

#include "dhcp.h"

/* 
 * ISC DHCP Management Sub-System - Subnet Functions
 */

static i_list* static_list = NULL;
static long static_cur_id = 0;

/* Struct Manipulation */

l_dhcp_subnet* l_dhcp_subnet_create ()
{
  l_dhcp_subnet *sub;

  sub = (l_dhcp_subnet *) malloc (sizeof(l_dhcp_subnet));
  if (!sub)
  { i_printf (1, "l_dhcp_subnet_create failed to malloc l_dhcp_subnet struct"); return NULL; }
  memset (sub, 0, sizeof(l_dhcp_subnet));

  sub->range_list = i_list_create ();
  if (!sub->range_list)
  { i_printf (1, "l_dhcp_subnet_create failed to create sub->range_list"); l_dhcp_subnet_free (sub); return NULL; }
  i_list_set_destructor (sub->range_list, l_dhcp_range_free);

  return sub;
}

void l_dhcp_subnet_free (void *subptr)
{
  l_dhcp_subnet *sub = subptr;

  if (!sub) return;

  if (sub->network_str) free (sub->network_str);
  if (sub->mask_str) free (sub->mask_str);
  if (sub->router_str) free (sub->router_str);
  if (sub->domain_str) free (sub->domain_str);
  if (sub->options_str) free (sub->options_str);
  if (sub->range_list) i_list_free (sub->range_list);

  free (sub);
}

/* List Manipulation */

i_list* l_dhcp_subnet_list ()
{ return static_list; }

i_list* l_dhcp_subnet_list_create ()
{
  static_list = i_list_create ();
  if (!static_list)
  { i_printf (1, "l_dhcp_subnet_list_create failed to create static_list"); return NULL; }
  i_list_set_destructor (static_list, l_dhcp_subnet_free);

  return static_list;
}

int l_dhcp_subnet_list_free ()
{
  if (static_list)
  { i_list_free (static_list); static_list = NULL; }

  return 0;
}

/* ID Assignment */

long l_dhcp_subnet_id_assign ()
{ static_cur_id++; return static_cur_id; }

int l_dhcp_subnet_id_setcurrent (long cur_id)
{ static_cur_id = cur_id; return 0; }

/* Get */

l_dhcp_subnet* l_dhcp_subnet_get (i_resource *self, long id)
{
  l_dhcp_subnet *subnet;
  i_list *sub_list = l_dhcp_subnet_list();
  
  for (i_list_move_head(sub_list); (subnet=i_list_restore(sub_list))!=NULL; i_list_move_next(sub_list))
  { if (subnet->id == id) return subnet; }

  return NULL;
}

/* Add */

l_dhcp_subnet* l_dhcp_subnet_add (i_resource *self, char *site_name, char *network_str, char *mask_str, char *router_str, char *domain_str, char *options_str)
{
  int num;
  i_list *sub_list;
  l_dhcp_subnet *subnet;
  i_pg_async_conn *conn;

  /* Check/Create list */
  sub_list = l_dhcp_subnet_list ();
  if (!sub_list)
  { 
    sub_list = l_dhcp_subnet_list_create ();
    if (!sub_list)
    { i_printf (1, "l_dhcp_subnet_add failed to create subnet list"); return NULL; }
  }

  /* Create struct */
  subnet = l_dhcp_subnet_create ();
  if (!subnet)
  { i_printf (1, "l_dhcp_subnet_add failed to create subnet struct"); return NULL; }
  subnet->network_str = strdup (network_str);
  subnet->mask_str = strdup (mask_str);
  subnet->router_str = strdup (router_str);
  subnet->domain_str = strdup (domain_str);
  subnet->options_str = strdup (options_str);
  inet_aton (subnet->network_str, &subnet->network_addr);
  inet_aton (subnet->mask_str, &subnet->mask_addr);
  inet_aton (subnet->router_str, &subnet->router_addr);
  
  /* Find site entity */
  if (site_name && strlen(site_name) > 0)
  {
    /* Get site */
    subnet->site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), site_name);
    if (!subnet->site)
    { i_printf (1, "l_dhcp_subnet_add failed, specified site (%s) not found", site_name); l_dhcp_subnet_free (subnet); return NULL; }

    /* Add subnet to site */
    if (!subnet->site->subnet_list) subnet->site->subnet_list = i_list_create ();
    i_list_enqueue (subnet->site->subnet_list, subnet);
  }
  else
  { i_printf (1, "l_dhcp_subnet_add failed, invalid site_name specified"); l_dhcp_subnet_free (subnet); return NULL; }

  /* Enqueue */
  num = i_list_enqueue (sub_list, subnet);
  if (num != 0)
  { 
    i_printf (1, "l_dhcp_subnet_add failed to enqueue subnet %s/%s", subnet->network_str, subnet->mask_str); 
    l_dhcp_subnet_free (subnet);
    return NULL;
  }
  
  /* Open SQL Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "INSERT INTO dhcp_subnets (site, network, mask, router, domain, options) VALUES ('%s', '%s', '%s', '%s', '%s', '%s')",
      subnet->site->name_str, subnet->network_str, subnet->mask_str, subnet->router_str, subnet->domain_str, subnet->options_str);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_dhcp_subnet_sqlcb, "add");
    free (query);
    if (num != 0)
    { i_printf (1, "l_dhcp_subnet_add warning, failed to execute SQL insert query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_dhcp_subnet_add failed to open SQL conn for subnet %s/%s", subnet->network_str, subnet->mask_str); }

  /* Assign ID */
  subnet->id = l_dhcp_subnet_id_assign ();

  /* Load Ranges */
  num = l_dhcp_range_loadall (self, subnet);
  if (num != 0)
  { i_printf (1, "l_dhcp_subnet_add warning, failed to load ranged for subnet %s/%s", subnet->network_str, subnet->mask_str); }
  
  /* Update daemon */
  l_dhcp_daemon_update (self);

  return subnet;
}

/* Update */

int l_dhcp_subnet_update (i_resource *self, l_dhcp_subnet *subnet)
{
  int num;
  char *query;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_dhcp_subnet_update failed to open SQL connection for subnet %s/%s", subnet->network_str, subnet->mask_str); return -1; }

  /* Create query */
  asprintf (&query, "UPDATE dhcp_subnets SET site='%s', network='%s', mask='%s', router='%s', domain='%s', options='%s' WHERE id='%li'",
    subnet->site->name_str, subnet->network_str, subnet->mask_str, subnet->router_str, subnet->domain_str, subnet->options_str, subnet->id); 

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_dhcp_subnet_sqlcb, "update");
  free (query);
  if (num != 0)
  { i_printf (1, "l_dhcp_subnet_update failed to execute UPDATE query for subnet %s/%s", subnet->network_str, subnet->mask_str); i_pg_async_conn_close (conn); return -1; }

  /* Update daemon */
  l_dhcp_daemon_update (self);

  return 0;
}

/* Remove */

int l_dhcp_subnet_remove (i_resource *self, l_dhcp_subnet *subnet)
{
  int num;
  i_list *sub_list;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "DELETE FROM dhcp_subnets WHERE id='%li'", subnet->id);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_dhcp_subnet_sqlcb, "remove");
    free (query);
    if (num != 0)
    { i_printf (1, "l_dhcp_subnet_remove warning, failed to execute DELETE query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_dhcp_subnet_remove failed to open SQL connection"); }

  /* Remove from site */
  if (subnet->site && subnet->site->subnet_list)
  {
    num = i_list_search (subnet->site->subnet_list, subnet);
    if (num == 0)
    { i_list_delete (subnet->site->subnet_list); }
    if (subnet->site->subnet_list->size < 1)
    { i_list_free (subnet->site->subnet_list); subnet->site->subnet_list = NULL; }
  }

  /* Remove (and free) subnet from list */
  sub_list = l_dhcp_subnet_list ();
  num = i_list_search (sub_list, subnet);
  if (num == 0)
  { i_list_delete (sub_list); }
  else
  { i_printf (1, "l_dhcp_subnet_remove warning, failed to find subnet in subnet list"); }

  /* Update daemon */
  l_dhcp_daemon_update (self);

  return 0;
}

/* SQL Callback */

int l_dhcp_subnet_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_dhcp_subnet_sqlcb failed to execute query during %s operation", (char *) passdata); }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Load */

int l_dhcp_subnet_loadall (i_resource *self)
{
  int num;
  int row;
  int row_count;
  PGresult *res;
  PGconn *pgconn;
  i_list *sub_list;

  /* Check/Create List */
  sub_list = l_dhcp_subnet_list ();
  if (!sub_list)
  {
    sub_list = l_dhcp_subnet_list_create ();
    if (!sub_list)
    { i_printf (1, "l_dhcp_subnet_loadall failed to create sub_list"); return -1; }
  }

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_dhcp_subnet_loadall failed to connect to postgresql database"); return -1; }

  /* Load all DHCP subnets */
  res = PQexec (pgconn, "SELECT id, site, network, mask, router, domain, options FROM dhcp_subnets");
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
  { 
    i_printf (1, "l_dhcp_subnet_loadall failed to execute SELECT query for the dhcp_subnets table");
    if (res) PQclear (res);
    i_pg_close (pgconn);
    return -1;
  }

  /* Loop through each subnet record */
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *str;
    l_dhcp_subnet *subnet;

    /* Create subnet */
    subnet = l_dhcp_subnet_create ();
    if (!subnet)
    { i_printf (1, "l_dhcp_subnet_loadall failed to create subnet struct at subnet record %i", row); continue; }

    /* ID */
    str = PQgetvalue (res, row, 0);
    if (str) subnet->id = atol (str);

    /* Site */
    str = PQgetvalue (res, row, 1);
    if (str && strlen(str) > 0)
    { 
      /* Get site */
      subnet->site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), str);
      if (!subnet->site)
      { i_printf (1, "l_dhcp_subnet_loadall site %s not found, ignoring DHCP subnet %li", str, subnet->id); l_dhcp_subnet_free (subnet); continue; }

      /* Add subnet to site's subnet_list */
      if (!subnet->site->subnet_list) subnet->site->subnet_list = i_list_create ();
      i_list_enqueue (subnet->site->subnet_list, subnet);
    }
    else
    { i_printf (1, "l_dhcp_subnet_loadall invalid site_name for DHCP subnet %li", subnet->id); l_dhcp_subnet_free (subnet); continue; }

    /* Network */
    str = PQgetvalue (res, row, 2);
    if (str) subnet->network_str = strdup (str);
    inet_aton (subnet->network_str, &subnet->network_addr);

    /* Mask */
    str = PQgetvalue (res, row, 3);
    if (str) subnet->mask_str = strdup (str);
    inet_aton (subnet->mask_str, &subnet->mask_addr);

    /* Router */
    str = PQgetvalue (res, row, 4);
    if (str) subnet->router_str = strdup (str);
    inet_aton (subnet->router_str, &subnet->router_addr);

    /* Router */
    str = PQgetvalue (res, row, 5);
    if (str) subnet->domain_str = strdup (str);

    /* Options */
    str = PQgetvalue (res, row, 6);
    if (str) subnet->options_str = strdup (str);
    
    /* Enqueue subnet */
    num = i_list_enqueue (sub_list, subnet);
    if (num != 0)
    { 
      i_printf (1, "l_dhcp_subnet_loadall failed to enqueue subnet %s/%s", subnet->network_str, subnet->mask_str);
      l_dhcp_subnet_free (subnet);
      continue;
    }

    /* Load ranges */
    num = l_dhcp_range_loadall (self, subnet);
    if (num != 0)
    { 
      i_printf (1, "l_dhcp_subnet_loadall warning, failed to load ranges for subnet %s/%s", 
        subnet->network_str, subnet->mask_str);
      continue;
    }
  }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}

