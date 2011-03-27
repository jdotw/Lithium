#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/inventory.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/entity_xmlsync.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/opstate.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>
#include <induction/xml.h>

#include "inventory.h"
#include "lic.h"
#include "case.h"
#include "group.h"
#include "vrack.h"
#include "scene.h"
#include "device.h"
#include "searchcache.h"

/* 
 * Device Related Functions
 */

/* Add */

static int start_delay = 5;

i_device* l_device_add (i_resource *self, i_site *site, char *name_str, char *desc_str, char *ip_str, char *lom_ip_str, int snmpversion, char *snmpcomm_str, char *snmpauthpass_str, char *snmpprivpass_str, int snmpauthmethod, int snmpprivenc, char *username_str, char *password_str, char *lom_username_str, char *lom_password_str, char *vendor_str, char *profile_str, long refresh_interval, int protocol, int icmp, int lithiumsnmp, int swrun, int nagios, int lom, int xsan, int min_action_sev)
{
  int num;
  i_device *dev;
  i_pg_async_conn *conn;

  /* Create struct */
  dev = i_device_create (name_str, desc_str, ip_str, lom_ip_str, snmpcomm_str, username_str, password_str, lom_username_str, lom_password_str, vendor_str, profile_str, refresh_interval);
  if (!dev)
  { i_printf (1, "l_device_add failed to create device struct"); return NULL; }
  dev->snmpversion = snmpversion;
  if (snmpauthpass_str) dev->snmpauthpass_str = strdup (snmpauthpass_str);
  if (snmpprivpass_str) dev->snmpprivpass_str = strdup (snmpprivpass_str);
  dev->snmpauthmethod = snmpauthmethod;
  dev->snmpprivenc = snmpprivenc;
  dev->protocol = protocol;
  dev->icmp = icmp;
  dev->lithiumsnmp = lithiumsnmp;
  dev->swrun = swrun;
  dev->nagios = nagios;
  dev->lom = lom;
  dev->xsan = xsan;
  dev->minimum_action_severity = min_action_sev;
  
  /* Register device */
  num = i_entity_register (self, ENTITY(site), ENTITY(dev));
  if (num != 0)
  { i_printf (1, "l_device_add failed to register entity for device %s", dev->name_str); i_entity_free (ENTITY(dev)); return NULL; }

  /* License */
  dev->licensed = l_lic_take (self, ENTITY(dev));

  /* Sort device list */
  i_list_sort (site->dev_list, l_device_sortfunc_desc);
  
  /* Add to SQL */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *site_esc = i_postgres_escape (site->name_str);
    char *name_esc = i_postgres_escape (dev->name_str);
    char *desc_esc = i_postgres_escape (dev->desc_str);
    char *ip_esc = i_postgres_escape (dev->ip_str);
    char *lom_ip_esc = i_postgres_escape (dev->lom_ip_str);
    char *snmpcomm_esc = i_postgres_escape (dev->snmpcomm_str);
    char *snmpauthpass_esc = i_postgres_escape (dev->snmpauthpass_str);
    char *snmpprivpass_esc = i_postgres_escape (dev->snmpprivpass_str);
    char *username_esc = i_postgres_escape (dev->username_str);
    char *password_esc = i_postgres_escape (dev->password_str);
    char *lom_username_esc = i_postgres_escape (dev->lom_username_str);
    char *lom_password_esc = i_postgres_escape (dev->lom_password_str);
    char *vendor_esc = i_postgres_escape (dev->vendor_str);
    char *profile_esc = i_postgres_escape (dev->profile_str);
    char *query;
    char uuid_str[37];

    uuid_unparse_lower (dev->uuid, uuid_str);

    /* Create query */
    asprintf (&query, "INSERT INTO devices (site, name, descr, ip, lom_ip, snmpversion, snmpcomm, snmpauthpass, snmpprivpass, snmpauthmethod, snmpprivenc, username, password, lom_username, lom_password, vendor, profile, refresh_interval, protocol, icmp, lithiumsnmp, swrun, nagios, lom, xsan, uuid, minimum_action_severity) VALUES ('%s', '%s', '%s', '%s', '%s', '%i', '%s', '%s', '%s', '%i', '%i', '%s', '%s', '%s', '%s', '%s', '%s', '%li', '%i', '%i', '%i', '%i', '%i', '%i', '%i', '%s', '%i')", 
      site_esc, name_esc, desc_esc, ip_esc, lom_ip_esc, dev->snmpversion, snmpcomm_esc, snmpauthpass_esc, snmpprivpass_esc, dev->snmpauthmethod, dev->snmpprivenc, username_esc, password_esc, lom_username_esc, lom_password_esc, vendor_esc, profile_esc, dev->refresh_interval, dev->protocol, dev->icmp, dev->lithiumsnmp, dev->swrun, dev->nagios, dev->lom, dev->xsan, uuid_str, dev->minimum_action_severity);
    free (site_esc);
    free (name_esc);
    free (desc_esc);
    free (ip_esc);
    free (lom_ip_esc);
    free (snmpcomm_esc);
    free (snmpauthpass_esc);
    free (snmpprivpass_esc);
    free (username_esc);
    free (password_esc);
    free (lom_username_esc);
    free (lom_password_esc);
    free (vendor_esc);
    free (profile_esc);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_device_sqlcb, "add");
    free (query);
    if (num != 0)
    { i_printf (1, "l_device_add warning, failed to execute SQL insert query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_device_add failed to open SQL conn for device %s", dev->name_str); }

  /* (Re)start device resource */
  num = l_device_res_restart (self, dev);
  if (num != 0)
  { i_printf (1, "l_device_add warning, failed to (re)start device resource for %s at %s", dev->name_str, dev->site->name_str); }

  return dev;
}

/* Update */

int l_device_update (i_resource *self, i_device *dev)
{
  int num;
  char *query;
  char *group_query;
  i_pg_async_conn *conn;

  /* Sort device list */
  i_list_sort (dev->site->dev_list, l_device_sortfunc_desc);
  
  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "l_device_update failed to open SQL connection for device %s", dev->name_str); return -1; }

  /* Create query */
  char *site_esc = i_postgres_escape (dev->site->name_str);
  char *name_esc = i_postgres_escape (dev->name_str);
  char *desc_esc = i_postgres_escape (dev->desc_str);
  char *ip_esc = i_postgres_escape (dev->ip_str);
  char *lom_ip_esc = i_postgres_escape (dev->lom_ip_str);
  char *snmpcomm_esc = i_postgres_escape (dev->snmpcomm_str);
  char *snmpauthpass_esc = i_postgres_escape (dev->snmpauthpass_str);
  char *snmpprivpass_esc = i_postgres_escape (dev->snmpprivpass_str);
  char *username_esc = i_postgres_escape (dev->username_str);
  char *password_esc = i_postgres_escape (dev->password_str);
  char *lom_username_esc = i_postgres_escape (dev->lom_username_str);
  char *lom_password_esc = i_postgres_escape (dev->lom_password_str);
  char *vendor_esc = i_postgres_escape (dev->vendor_str);
  char *profile_esc = i_postgres_escape (dev->profile_str);
  asprintf (&query, "UPDATE devices SET descr='%s', ip='%s', lom_ip='%s', snmpversion='%i', snmpcomm='%s', snmpauthpass='%s', snmpprivpass='%s', snmpauthmethod='%i', snmpprivenc='%i', username='%s', password='%s', lom_username='%s', lom_password='%s', vendor='%s', profile='%s', refresh_interval='%li', protocol='%i', icmp='%i', lithiumsnmp='%i', swrun='%i', nagios='%i', lom='%i', xsan='%i', minimum_action_severity='%i'  WHERE site='%s' AND name='%s'", 
    desc_esc, ip_esc, lom_ip_esc, dev->snmpversion, snmpcomm_esc, snmpauthpass_esc, snmpprivpass_esc, dev->snmpauthmethod, dev->snmpprivenc, username_esc, password_esc, lom_username_esc, lom_password_esc, vendor_esc, profile_esc, dev->refresh_interval, dev->protocol, dev->icmp, dev->lithiumsnmp, dev->swrun, dev->nagios, dev->lom, dev->xsan, dev->minimum_action_severity, site_esc, name_esc); 
  asprintf (&group_query, "UPDATE group_entities SET dev_desc='%s' WHERE site_name='%s' AND dev_name='%s'", desc_esc, site_esc, name_esc);
  free (site_esc);
  free (name_esc);
  free (desc_esc);
  free (ip_esc);
  free (lom_ip_esc);
  free (snmpcomm_esc);
  free (snmpauthpass_esc);
  free (snmpprivpass_esc);
  free (username_esc);
  free (password_esc);
  free (lom_username_esc);
  free (lom_password_esc);
  free (vendor_esc);
  free (profile_esc);

  /* Execute device table query */
  num = i_pg_async_query_exec (self, conn, query, 0, l_device_sqlcb, "update");
  free (query);
  if (num != 0)
  { i_printf (1, "l_device_update failed to execute UPDATE query for device %s", dev->name_str); i_pg_async_conn_close (conn); return -1; }

  /* Execute groups table query */
  num = i_pg_async_query_exec (self, conn, group_query, 0, NULL, NULL);
  free (group_query);

  /* License */
  dev->licensed = l_lic_take (self, ENTITY(dev));

  /* Restart device resource */
  num = l_device_res_restart (self, dev);
  if (num != 0)
  { i_printf (1, "l_device_update warning, failed to restart device resource for %s at %s", dev->name_str, dev->site->name_str); }

  return 0;
}

/* Remove */

int l_device_remove (i_resource *self, i_device *dev)
{
  int num;
  i_pg_async_conn *conn;

  /* Remove from other SQL tables */
  l_searchcache_delete_device(self, dev);
  l_group_sql_entity_delete_device (self, dev);
  l_case_entity_sql_delete_device (self, dev);
  l_vrack_sql_cable_delete_device (self, dev);
  l_vrack_sql_device_delete_device (self, dev);
  l_scene_sql_overlay_delete_device (self, dev);

  /* Open Conn */
  conn = i_pg_async_conn_open_customer (self);
  if (conn)
  {
    char *query;

    /* Create query */
    asprintf (&query, "DELETE FROM devices WHERE site='%s' AND name='%s'", dev->site->name_str, dev->name_str);

    /* Execute query */
    num = i_pg_async_query_exec (self, conn, query, 0, l_device_sqlcb, "remove");
    free (query);
    if (num != 0)
    { i_printf (1, "l_device_remove warning, failed to execute DELETE query"); i_pg_async_conn_close (conn); }
  }
  else
  { i_printf (1, "l_device_remove failed to open SQL connection"); }

  /* Destroy device resource */
  num = l_device_res_destroy (self, dev);
  if (num != 0)
  { i_printf (1, "l_device_update warning, failed to restart device %s at device %s", dev->name_str, dev->site->name_str); }

  /* License */
  l_lic_rescind (self, ENTITY(dev));
  dev->licensed = 0;

  /* Desregister entity */
  num = i_entity_deregister (self, ENTITY(dev));
  if (num != 0)
  { i_printf (1, "l_device_remove warning, failed to deregister entity for device %s", dev->name_str); }
  
  /* Sort device list */
  i_list_sort (dev->site->dev_list, l_device_sortfunc_desc);

  /* Free entity */
  i_entity_free (ENTITY(dev));

  return 0;
}

/* SQL Callback */

int l_device_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Check result */
  if (!result || (PQresultStatus(result) != PGRES_COMMAND_OK && PQresultStatus(result) != PGRES_TUPLES_OK))
  { i_printf (1, "l_device_sqlcb failed to execute query during %s operation", (char *) passdata); }

  /* Close conn */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Load */

int l_device_loadall (i_resource *self, i_site *site)
{
  int num;
  int row;
  int row_count;
  char *query;
  PGresult *res;
  PGconn *pgconn;

  /* CHeck site */
  if (!site || !site->name_str)
  { i_printf (1, "l_device_loadall failed, site or site->name_str is null"); return -1; }

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_device_loadall failed to connect to SQL database"); return -1; }

  /* Load all devices */
  asprintf (&query, "SELECT DISTINCT ON (name) name, descr, ip, lom_ip, snmpversion, snmpcomm, snmpauthpass, snmpprivpass, snmpauthmethod, snmpprivenc, username, password, lom_username, lom_password, vendor, profile, refresh_interval, protocol, icmp, lithiumsnmp, swrun, nagios, lom, xsan, uuid, mark, minimum_action_severity FROM devices WHERE site='%s'", site->name_str);
  res = PQexec (pgconn, query);
  free (query);
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
  { 
    i_printf (1, "l_device_loadall failed to execute SELECT query for the devices table");
    if (res) PQclear (res);
    i_pg_close (pgconn);
    return -1;
  }

  /* Loop through each device record */
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *name_str;
    char *desc_str;
    char *ip_str;
    char *lom_ip_str;
    int snmpversion = 1;
    char *snmpcomm_str;
    char *snmpauthpass_str;
    char *snmpprivpass_str;
    int snmpauthmethod = 0;
    int snmpprivenc = 0; 
    char *username_str;
    char *password_str;
    char *lom_username_str;
    char *lom_password_str;
    char *vendor_str;
    char *profile_str;
    char *refresh_interval_str;
    time_t refresh_interval;
    int protocol = 0;
    int icmp = 0;
    int lithiumsnmp = 0;
    int swrun = 0;
    int nagios = 0;
    int lom = 0;
    int xsan = 0;
    char *uuid_str;
    int mark = 0;
    int minimum_action_severity = 1;
    i_device *dev;

    /* Fields */
    name_str = PQgetvalue (res, row, 0);
    desc_str = PQgetvalue (res, row, 1);
    ip_str = PQgetvalue (res, row, 2);
    lom_ip_str = PQgetvalue (res, row, 3);
    if (PQgetvalue (res, row, 4)) snmpversion = atoi(PQgetvalue (res, row, 4));
    if (snmpversion == 0) snmpversion = 1;
    snmpcomm_str = PQgetvalue (res, row, 5);
    snmpauthpass_str = PQgetvalue (res, row, 6);
    snmpprivpass_str = PQgetvalue (res, row, 7);
    if (PQgetvalue (res, row, 8)) snmpauthmethod = atoi(PQgetvalue (res, row, 8));
    if (PQgetvalue (res, row, 9)) snmpprivenc = atoi(PQgetvalue (res, row, 9));
    username_str = PQgetvalue (res, row, 10);
    password_str = PQgetvalue (res, row, 11);
    lom_username_str = PQgetvalue (res, row, 12);
    lom_password_str = PQgetvalue (res, row, 13);
    vendor_str = PQgetvalue (res, row, 14);
    profile_str = PQgetvalue (res, row, 15);
    refresh_interval_str = PQgetvalue (res, row, 16);
    if (refresh_interval_str)
    { refresh_interval = atol (refresh_interval_str); }
    else
    { refresh_interval = DEVICE_DEFAULT_REFRESH_INTERVAL; }
    if (PQgetvalue (res, row, 17)) protocol = atoi (PQgetvalue (res, row, 17));
    if (PQgetvalue (res, row, 18)) icmp = atoi (PQgetvalue (res, row, 18));
    if (PQgetvalue (res, row, 19)) lithiumsnmp = atoi (PQgetvalue (res, row, 19));
    if (PQgetvalue (res, row, 20)) swrun = atoi (PQgetvalue (res, row, 20));
    if (PQgetvalue (res, row, 21)) nagios = atoi (PQgetvalue (res, row, 21));
    if (PQgetvalue (res, row, 22)) lom = atoi (PQgetvalue (res, row, 22));
    if (PQgetvalue (res, row, 23)) xsan = atoi (PQgetvalue (res, row, 23));
    uuid_str = PQgetvalue (res, row, 24);
    if (PQgetvalue (res, row, 25)) mark = atoi (PQgetvalue (res, row, 25));
    if (PQgetvalue (res, row, 26)) minimum_action_severity = atoi (PQgetvalue (res, row, 26));

    /* Create device */
    dev = i_device_create (name_str, desc_str, ip_str, lom_ip_str, snmpcomm_str, username_str, password_str, lom_username_str, lom_password_str, vendor_str, profile_str, refresh_interval);
    if (!dev)
    { i_printf (1, "l_device_loadall failed to create device struct at device record %i", row); continue; }
    dev->snmpversion = snmpversion;
    if (snmpauthpass_str) dev->snmpauthpass_str = strdup (snmpauthpass_str);
    if (snmpprivpass_str) dev->snmpprivpass_str = strdup (snmpprivpass_str);
    dev->snmpauthmethod = snmpauthmethod;
    dev->snmpprivenc = snmpprivenc;
    dev->protocol = protocol;
    dev->icmp = icmp;
    dev->lithiumsnmp = lithiumsnmp;
    dev->swrun = swrun;
    dev->nagios = nagios;
    dev->lom = lom;
    dev->xsan = xsan;
    uuid_parse (uuid_str, dev->uuid);
    dev->mark = mark;
    dev->minimum_action_severity = minimum_action_severity;

    /* Register device */
    num = i_entity_register (self, ENTITY(site), ENTITY(dev));
    if (num != 0)
    { i_printf (1, "l_device_loadall failed to register device %s", dev->name_str); i_entity_free (dev); continue; }

    /* License */
    dev->licensed = l_lic_take (self, ENTITY(dev));

    /* Add timer to (re)start device resource */
    i_timer_add (self, start_delay, 0, l_device_loadall_timercb, dev);
    i_printf (0, "l_device_loadall device %s will start in %i seconds", dev->desc_str, start_delay);
    start_delay += 5 + (start_delay % 3);

    /* Set initial opstate if marked */
    if (dev->mark != 0) i_opstate_change (self, ENTITY(dev), dev->mark);
  }

  /* Close DB */
  PQclear (res);
  i_pg_close (pgconn);

  return 0;
}

int l_device_loadall_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  int num;
  i_device *dev = passdata;

  /* Check system load */
  int restart = 0;
  i_pg_async_conn *pconn = i_pg_async_conn_persistent ();
  if (pconn && pconn->query_q && pconn->query_q->size > 100)
  { 
    i_printf (1, "l_device_loadall_timercb PostgreSQL command queue is >100 (%i) deep. Delaying start of device %s at %s.", 
      pconn->query_q->size, dev->name_str, dev->site->name_str);
    restart = 1;
  }
  if (l_device_res_queue_depth () > 10)
  {
    i_printf (1, "l_device_loadall_timercb device (re)start queue is >10 (%i) deep. Delaying start of device %s at %s.", 
      l_device_res_queue_depth (), dev->name_str, dev->site->name_str);
    restart = 1;
  }
  if (restart == 1)
  {
    if (timer->timeout.tv_sec > 300)
    { i_timer_set_timeout (timer, timer->timeout.tv_sec / 2, 0); }
    return 0;
  }
  

  /* (Re)start device resource */
  num = l_device_res_restart (self, dev);
  if (num != 0)
  { i_printf (1, "l_device_loadall warning, failed to (re)start device %s at %s", dev->name_str, dev->site->name_str); }

  /* Load Inventory */
  l_inventory_static_registerall (self, dev);

  return -1;
}

/* Initialise SQL */

int l_device_initsql (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  /* Connect to SQL db */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_device_initsql failed to connect to postgres database"); return -1; }

  /* Check devices SQL table */
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'devices' AND relkind = 'r'");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "CREATE TABLE devices (site varchar, name varchar, descr varchar, ip varchar, lom_ip varchar, snmpversion integer, snmpcomm varchar, snmpauthpass varchar, snmpprivpass varchar, snmpauthmethod integer, snmpprivenc varchar, username varchar, password varchar, lom_username varchar, lom_password varchar, vendor varchar, profile varchar, refresh_interval integer, longlat point, protocol integer, icmp integer, lithiumsnmp integer, swrun integer, nagios integer, lom integer, xsan integer, uuid varchar, minimum_action_severity integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to create devices table (%s)", PQresultErrorMessage (result)); }

    /* create install dir */
    char *path = i_path_glue (self->root, "install");
    mkdir (path, 0700);
  }
  if (result) { PQclear(result); result = NULL; }

  /* 
   * Version-specific checking
   */

  /* 4.8.3 - Added 'username' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='username' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'username' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN username varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add username column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* 4.8.3 - Added 'password' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='password' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'password' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN password varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add password column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* 4.8.7 - Added 'group' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='device_group' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'device_group' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN device_group varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add group column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* 5.0.0 - Added 'lom_username' and 'lom_password' columns */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='lom_username' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'lom_username' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN lom_username varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add lom_username column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='lom_password' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'lom_password' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN lom_password varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    { i_printf (1, "l_device_initsql failed to add lom_password column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='lom_ip' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'lom_ip' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN lom_ip varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add lom_ip column (%s)", PQresultErrorMessage (result)); }
  } 
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='protocol' ORDER BY ordinal_position");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'protocol' column missing, attempting to add it (and others)");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN protocol integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add protocol column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN icmp integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add icmp column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN lithiumsnmp integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add lithiumsnmp column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN nagios integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add nagios column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN lom integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add lom column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN xsan integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add xsan column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
  } 
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='snmpversion' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'snmpversion' column missing, attempting to add it (and others)");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN snmpversion integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add snmpversion column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN snmpauthpass varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add snmpauthpass column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN snmpprivpass varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add snmpprivpass column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN snmpauthmethod integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add snmpauthmethod column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN snmpprivenc integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)    
    { i_printf (1, "l_device_initsql failed to add snmpprivenc column (%s)", PQresultErrorMessage (result)); }
    if (result) { PQclear(result); result = NULL; }
  }
  if (result) { PQclear(result); result = NULL; }

  /* 5.0.0 - Added 'uuid' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='uuid' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'uuid' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN uuid varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add uuid column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* 5.0.0 - Added 'mark' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='mark' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'mark' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN mark integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add mark column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* 5.0.0 - Added 'swrun' column */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='swrun' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'swrun' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN swrun integer");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add swrun column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* 5.0.9 - Added 'minimum_action_severity' */
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='devices' AND column_name='minimum_action_severity' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_device_initsql version-specific check: 'minimum_action_severity' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE devices ADD COLUMN minimum_action_severity integer DEFAULT 1");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_device_initsql failed to add minimum_action_severity column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* Ensure all rows have a UUID */
  result = PQexec (pgconn, "SELECT name, uuid FROM devices");
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
        asprintf (&query, "UPDATE devices SET uuid='%s' WHERE name='%s'", uuid_buf, name_str);
        PQexec (pgconn, query);
        free (query);
      }
    }
  }
  if (result) { PQclear(result); result = NULL; }

  /*
   * Table scrubbing to remove obsolete entries 
   */

  result = PQexec (pgconn, "delete from devices where site not in (select name from sites)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from action_entities where dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from case_entities where case_entities.dev_name not in (select name from devices);");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from group_entities where dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from procpro where device not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from record_recrules where dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from scene_overlays where dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from services where device not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from triggerset_valrules where dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from triggerset_apprules where dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from vrack_devices where dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from vrack_cables where a_dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }
  result = PQexec (pgconn, "delete from vrack_cables where b_dev_name not in (select name from devices)");
  if (result) { PQclear(result); result = NULL; }

  /* Close DB */
  i_pg_close (pgconn);

  return 0;
}


