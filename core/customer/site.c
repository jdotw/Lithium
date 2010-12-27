#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

#include "device.h"
#include "navtree.h"
#include "lic.h"
#include "site.h"

/* 
 * Site Related Functions
 */

/* Add */

i_site* l_site_add (i_resource *self, char *name_str, char *desc_str, char *addr1_str, char *addr2_str, char *addr3_str, char *suburb_str, char *state_str, char *postcode_str, char *country_str, double longitude, double latitude)
{
  int num;
  i_site *site;
  i_pg_async_conn *conn;

  /* Create struct */
  site = i_site_create (name_str, desc_str, addr1_str, addr2_str, addr3_str, suburb_str, state_str, postcode_str, country_str, longitude, latitude);
  if (!site)
  { i_printf (1, "l_site_add failed to create site struct"); return NULL; }
  site->navtree_func = l_navtree_func_site;
  site->dev_list_sortfunc = l_device_sortfunc_desc; 
  
  /* Register site */
  num = i_entity_register (self, ENTITY(self->hierarchy->cust), ENTITY(site));
  if (num != 0)
  { i_printf (1, "l_site_add failed to register entity for site %s", site->name_str); i_entity_free (ENTITY(site)); return NULL; }

  /* License */
  site->licensed = l_lic_take (self, ENTITY(site));

  /* Sort site list */
  i_list_sort (self->hierarchy->cust->site_list, l_site_sortfunc_suburb);
  
  /* Add to SQL */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *query;
    char *name_esc = i_postgres_escape (site->name_str);
    char *desc_esc = i_postgres_escape (site->desc_str);
    char *addr1_esc = i_postgres_escape (site->addr1_str);
    char *addr2_esc = i_postgres_escape (site->addr2_str);
    char *addr3_esc = i_postgres_escape (site->addr3_str);
    char *suburb_esc = i_postgres_escape (site->suburb_str);
    char *state_esc = i_postgres_escape (site->state_str);
    char *postcode_esc = i_postgres_escape (site->postcode_str);
    char *country_esc = i_postgres_escape (site->country_str);
    char uuid_str[37];

    uuid_unparse_lower (site->uuid, uuid_str);

    /* Create query */
    asprintf (&query, "INSERT INTO sites (name, descr, addr1, addr2, addr3, suburb, state, postcode, country, longlat, uuid) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '(%f,%f)', '%s')", 
      name_esc, desc_esc, addr1_esc, addr2_esc, addr3_esc,
      suburb_esc, state_esc, postcode_esc, country_esc,
      site->longitude, site->latitude, uuid_str);
    free (name_esc);
    free (desc_esc);
    free (addr1_esc);
    free (addr2_esc);
    free (addr3_esc);
    free (suburb_esc);
    free (state_esc);
    free (postcode_esc);
    free (country_esc);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_site_sqlcb, "add");
    free (query);
    if (num != 0)
    { i_printf (1, "l_site_add warning, failed to execute SQL insert query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_site_add failed to open SQL conn for site %s", site->name_str); }

  /* Load devices */
  num = l_device_loadall (self, site);
  if (num != 0)
  { i_printf (1, "l_site_add warning, failed to load all devices for site %s", site->name_str); }

  return site;
}

/* Update */

int l_site_update (i_resource *self, i_site *site)
{
  int num;
  char *query;
  i_device *dev;
  i_pg_async_conn *conn;

  /* Sort site list */
  i_list_sort (self->hierarchy->cust->site_list, l_site_sortfunc_suburb);
  
  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_site_update failed to open SQL connection for site %s", site->name_str); return -1; }

  /* Create query */
  asprintf (&query, "UPDATE sites SET descr='%s', addr1='%s', addr2='%s', addr3='%s', suburb='%s', state='%s', postcode='%s', country='%s', longlat='(%f,%f)' WHERE name='%s'", 
    site->desc_str, site->addr1_str, site->addr2_str, site->addr3_str, site->suburb_str, 
    site->state_str, site->postcode_str, site->country_str, 
    site->longitude, site->latitude, site->name_str); 
  i_printf(0,"query=%s", query);

  /* Execute query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_site_sqlcb, "update");
  free (query);
  if (num != 0)
  { i_printf (1, "l_site_update failed to execute UPDATE query for site %s", site->name_str); i_pg_async_conn_close (conn); return -1; }

  /* Restart device resources */
  for (i_list_move_head(site->dev_list); (dev=i_list_restore(site->dev_list))!=NULL; i_list_move_next(site->dev_list))
  {
    num = l_device_res_restart (self, dev);
    if (num != 0)
    { i_printf (1, "l_site_update warning, failed to restart device %s at site %s", dev->name_str, site->name_str); continue; }
  }

  return 0;
}

/* Remove */

int l_site_remove (i_resource *self, i_site *site)
{
  int num;
  i_device *dev;
  i_pg_async_conn *conn;

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "DELETE FROM sites WHERE name='%s'", site->name_str);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_site_sqlcb, "remove");
    free (query);
    if (num != 0)
    { i_printf (1, "l_site_remove warning, failed to execute DELETE query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_site_remove failed to open SQL connection"); }

  /* Destroy device resources */
  for (i_list_move_head(site->dev_list); (dev=i_list_restore(site->dev_list))!=NULL; i_list_move_next(site->dev_list))
  {
    num = l_device_remove (self, dev);
    if (num != 0)
    { i_printf (1, "l_site_remove warning, failed to remove device %s at site %s", dev->name_str, site->name_str); continue; }
  }

  /* License */
  l_lic_rescind (self, ENTITY(site));

  /* Desregister entity (will de-register and free children) */
  num = i_entity_deregister (self, ENTITY(site));
  if (num != 0)
  { i_printf (1, "l_site_remove warning, failed to deregister entity for site %s", site->name_str); }
  
  /* Sort site list */
  i_list_sort (self->hierarchy->cust->site_list, l_site_sortfunc_suburb);

  /* Free entity */
  i_entity_free (ENTITY(site));

  return 0;
}

/* SQL Callback */

int l_site_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_site_sqlcb failed to execute query during %s operation", (char *) passdata); }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Load */

int l_site_loadall (i_resource *self)
{
  int num;
  int row;
  int row_count;
  PGresult *res;
  PGconn *pgconn;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_site_loadall failed to connect to SQL database"); return -1; }

  /* Load all sites */
  res = PQexec (pgconn, "SELECT DISTINCT ON (name) name, descr, addr1, addr2, addr3, suburb, state, postcode, country, longlat, uuid FROM sites");
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
  { 
    i_printf (1, "l_site_loadall failed to execute SELECT query for the sites table");
    if (res) PQclear (res);
    i_pg_close (pgconn);
    return -1;
  }

  /* Loop through each site record */
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *name_str;
    char *desc_str;
    char *addr1_str;
    char *addr2_str;
    char *addr3_str;
    char *suburb_str;
    char *state_str;
    char *postcode_str;
    char *country_str;
    char *longlat_str;
    double longitude;
    double latitude;
    char *uuid_str;
    i_site *site;

    /* Fields */
    name_str = PQgetvalue (res, row, 0);
    desc_str = PQgetvalue (res, row, 1);
    addr1_str = PQgetvalue (res, row, 2);
    addr2_str = PQgetvalue (res, row, 3);
    addr3_str = PQgetvalue (res, row, 4);
    suburb_str = PQgetvalue (res, row, 5);
    state_str = PQgetvalue (res, row, 6);
    postcode_str = PQgetvalue (res, row, 7);
    country_str = PQgetvalue (res, row, 8);
    longlat_str = PQgetvalue (res, row, 9);
    uuid_str = PQgetvalue (res, row, 10);
    sscanf (longlat_str, "(%lf,%lf)", &longitude, &latitude);
    
    /* Create site */
    site = i_site_create (name_str, desc_str, addr1_str, addr2_str, addr3_str, suburb_str, state_str, postcode_str, country_str, longitude, latitude);
    if (!site)
    { i_printf (1, "l_site_loadall failed to create site struct at site record %i", row); continue; }
    site->navtree_func = l_navtree_func_site;
    site->dev_list_sortfunc = l_device_sortfunc_desc; 
    uuid_parse (uuid_str, site->uuid);

    /* Register site */
    num = i_entity_register (self, ENTITY(self->hierarchy->cust), ENTITY(site));
    if (num != 0)
    { i_printf (1, "l_site_loadall failed to register site %s", site->name_str); i_entity_free (site); continue; }

    /* License */
    site->licensed = l_lic_take (self, ENTITY(site));

    /* Load devs */
    num = l_device_loadall (self, site);
    if (num != 0)
    { i_printf (1, "l_site_loadall warning, failed to load all devices for site %s", site->name_str); return -1; }
  }

  /* Close DB */
  if (res) PQclear (res);
  i_pg_close (pgconn);

  return 0;
}

/* Initialise SQL */

int l_site_initsql (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  /* Connect to SQL db */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_site_initsql failed to connect to postgres database"); return -1; }

  /* Check sites SQL table */
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'sites' AND relkind = 'r'");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* Clear first result */
    if (result) PQclear (result);

    /* sites table not in database */
    result = PQexec (pgconn, "CREATE TABLE sites (name varchar, descr varchar, addr1 varchar, addr2 varchar, addr3 varchar, suburb varchar, state varchar, postcode varchar, country varchar, longlat point, uuid varchar)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_site_initsql failed to create sites table (%s)", PQresultErrorMessage (result)); }
    PQclear(result);

    /* Create default site */
    result = PQexec (pgconn, "INSERT INTO sites (name, descr) VALUES ('default', 'Default Location')");
    if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
    { i_printf (1, "l_site_initsql warning. failed to insert default site record into sites table"); }
    PQclear(result);
  }
  else if (result)
  { PQclear (result); }

  /* 5.0.0 - Added 'uuid' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='sites' AND column_name='uuid' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_site_initsql version-specific check: 'uuid' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE sites ADD COLUMN uuid varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_site_initsql failed to add uuid column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* Ensure all rows have a UUID */
  result = PQexec (pgconn, "SELECT name, uuid FROM sites");
  if (result && PQresultStatus(result) == PGRES_TUPLES_OK)
  {
    int row_cont = PQntuples(result);
    int row;
    for (row=0; row < row_cont; row++)
    {
      char *name_str = PQgetvalue (result, row, 0);
      char *uuid_str = PQgetvalue (result, row, 1);
      if (!uuid_str || strlen(uuid_str) < 1)
      {
        /* Invalid UUID, reset it */
        uuid_t uuid;
        uuid_generate (uuid);
        char uuid_buf[37];
        uuid_unparse_lower (uuid, uuid_buf);
        char *query;
        asprintf (&query, "UPDATE sites SET uuid='%s' WHERE name='%s'", uuid_buf, name_str);
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


