#include <stdlib.h>

#include "induction.h"
#include "threshold.h"
#include "postgresql.h"
#include "list.h"
#include "hierarchy.h"

static int static_enabled = 0;

/* Enable / Disable */

int i_threshold_sql_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *res;
      
  if (static_enabled == 1)
  { i_printf (1, "i_threshold_sql_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  pgconn = i_pg_connect_customer (self);      
  if (!pgconn)          
  { i_printf (1, "i_threshold_sql_enable failed to connect to SQL database"); i_threshold_sql_disable (self); return -1; }

  i_pg_begin (pgconn);
  res = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'thresholds' AND relkind = 'r'");
  i_pg_end (pgconn);
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK || (PQntuples(res)) < 1)
  {
    /* events table not present in customers SQL database */
    res = PQexec (pgconn, "CREATE TABLE thresholds (site_id varchar, device_id varchar, section_name varchar, thold_name varchar, object_desc varchar, value_desc varchar, alert_value numeric, alert_default numeric, critical_value numeric, critical_default numeric, tolerance_percent numeric, tolerance_default numeric, last_registered integer)");
    if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
    { i_printf (1, "i_threshold_sql_enable failed to create thresholds table (%s)", PQresultErrorMessage (res)); PQclear(res); i_pg_close (pgconn); i_threshold_sql_disable (self); return -1; }
  }
  PQclear(res);

  i_pg_close (pgconn);
    
  return 0;
}

int i_threshold_sql_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "i_threshold_sql_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0;
  
  return 0;
}

/* SQL Operations */

i_threshold* i_threshold_sql_get (i_resource *self, i_threshold_section *section, char *name)
{
  int x;
  int field_count;
  int row_count;
  char *command;
  char *site_id_esc;
  char *device_id_esc;
  char *sect_name_esc;
  char *name_esc;
  struct timeval now;
  PGresult *result;
  PGconn *pgconn;
  i_threshold *thold;

  if (!self || !self->hierarchy || !section || !section->name || !name) return NULL;
  
  if (static_enabled == 0)
  { i_printf (1, "i_threshold_sql_get called with sub-system disabled"); return NULL; }

  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "i_threshold_sql_get failed to connect to SQL database"); return NULL; }

  site_id_esc = i_postgres_escape (self->hierarchy->site_id);
  device_id_esc = i_postgres_escape (self->hierarchy->device_id);
  sect_name_esc = i_postgres_escape (section->name);
  name_esc = i_postgres_escape (name);
  asprintf (&command, "SELECT * FROM thresholds WHERE site_id='%s' AND device_id='%s' AND section_name='%s' AND thold_name='%s'", 
    site_id_esc, device_id_esc, sect_name_esc, name_esc);
  if (site_id_esc) free (site_id_esc);
  if (device_id_esc) free (device_id_esc);
  if (sect_name_esc) free (sect_name_esc);
  if (name_esc) free (name_esc);

  i_pg_begin (pgconn);
  result = PQexec (pgconn, command);
  free (command);
  i_pg_end (pgconn);
  if (!result || PQresultStatus (result) != PGRES_TUPLES_OK)
  { i_printf (1, "i_threshold_sql_get failed to execute SELECT from thresholds table for threshold %s in section %s", name, section->name); i_pg_close (pgconn); return NULL; }

  row_count = PQntuples (result);
  if (row_count < 1)
  { i_pg_close (pgconn); return NULL; }

  /* Create the sturct */

  thold = i_threshold_create ();
  if (!thold)
  { i_printf (1, "i_threshold_sql_get failed to create threshold struct"); PQclear (result); return NULL; }
  thold->name = strdup (name);
  thold->section = section;
    
  /* Fill in the struct from the row data */

  field_count = PQnfields (result);
  for (x=0; x < field_count; x++)
  {
    /* Loops through each field in the row */

    char *field_name;
    char *value;

    field_name = PQfname (result, x);
    if (!field_name)
    { i_printf (1, "l_sql_populate_event_incident_list failed to get field name for x=%i, continuing", x); continue; }
    
    value = PQgetvalue (result, 0, x);

    if (!strcmp(field_name, "object_desc"))
    { if (value) thold->object_desc = strdup (value); }
    if (!strcmp(field_name, "value_desc"))
    { if (value) thold->value_desc = strdup (value); }
    
    if (!strcmp(field_name, "alert_value"))
    { if (value) thold->alert_value = atof (value); }
    if (!strcmp(field_name, "alert_default"))
    { if (value) thold->alert_default = atof (value); }
    
    if (!strcmp(field_name, "critical_value"))
    { if (value) thold->critical_value = atof (value); }
    if (!strcmp(field_name, "critical_default"))
    { if (value) thold->critical_default = atof (value); }
    
    if (!strcmp(field_name, "tolerance_percent"))
    { if (value) thold->tolerance_percent = atof (value); }
    if (!strcmp(field_name, "tolerance_default"))
    { if (value) thold->tolerance_default = atof (value); }
  } 
  
  PQclear (result);  

  /* Update last_registered field */

  gettimeofday (&now, NULL);
  site_id_esc = i_postgres_escape (self->hierarchy->site_id);
  device_id_esc = i_postgres_escape (self->hierarchy->device_id);
  sect_name_esc = i_postgres_escape (section->name);
  name_esc = i_postgres_escape (name);
  asprintf (&command, "UPDATE thresholds SET last_registered='%li' WHERE site_id='%s' AND device_id='%s' AND section_name='%s' AND thold_name='%s'",
    now.tv_sec, site_id_esc, device_id_esc, sect_name_esc, name_esc);
  if (site_id_esc) free (site_id_esc);
  if (device_id_esc) free (device_id_esc);
  if (sect_name_esc) free (sect_name_esc);
  if (name_esc) free (name_esc);

  i_pg_begin (pgconn);
  result = PQexec (pgconn, command);
  free (command);
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
  { i_printf (1, "i_threshold_sql_get failed to update last_registered for threshold %s in section %s (%s)", thold->name, thold->section->name, PQresultErrorMessage (result)); }
  PQclear (result);

  /* Finished */

  i_pg_close (pgconn);
    
  return thold;
}

int i_threshold_sql_put (i_resource *self, i_threshold *thold)
{
  PGconn *pgconn;
  PGresult *res;
  char *command;
  char *site_id_esc;
  char *device_id_esc;
  char *sect_name_esc;
  char *name_esc;
  char *object_esc;
  char *value_esc;

  if (!self || !self->hierarchy || !thold || !thold->section || !thold->name) return -1;

  if (static_enabled == 0)
  { i_printf (1, "i_threshold_sql_put called with sub-system disabled"); return -1; }

  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "i_threshold_sql_put failed to connect to SQL database"); return -1; }      

  site_id_esc = i_postgres_escape (self->hierarchy->site_id);
  device_id_esc = i_postgres_escape (self->hierarchy->device_id);
  sect_name_esc = i_postgres_escape (thold->section->name);
  name_esc = i_postgres_escape (thold->name);
  object_esc = i_postgres_escape (thold->object_desc);
  value_esc = i_postgres_escape (thold->value_desc);
  asprintf (&command, "INSERT INTO thresholds (site_id, device_id, section_name, thold_name, object_desc, value_desc, alert_value, alert_default, critical_value, critical_default, tolerance_percent, tolerance_default, last_registered) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%.2f', '%.2f', '%.2f', '%.2f', '%.2f', '%.2f', '0')",
    site_id_esc, device_id_esc, sect_name_esc, name_esc, object_esc, value_esc, thold->alert_value, thold->alert_default, thold->critical_value, thold->critical_default, thold->tolerance_percent, thold->tolerance_default);
  if (site_id_esc) free (site_id_esc);
  if (device_id_esc) free (device_id_esc);
  if (sect_name_esc) free (sect_name_esc);
  if (name_esc) free (name_esc);
  if (object_esc) free (object_esc);
  if (value_esc) free (value_esc);

  i_pg_begin (pgconn);
  res = PQexec (pgconn, command);
  free (command);
  i_pg_end (pgconn);
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    i_printf (1, "i_threshold_sql_put failed to execute INSERT command (%s)", PQresultErrorMessage (res));
    PQclear(res);
    i_pg_close (pgconn);
    return -1;
  }
  PQclear(res);
  i_pg_close (pgconn);

  return 0;
}

int i_threshold_sql_update (i_resource *self, i_threshold *thold)
{
  char *command;
  PGresult *res;
  PGconn *pgconn;
  char *site_id_esc;
  char *device_id_esc;
  char *sect_name_esc;
  char *name_esc;

  if (!self || !self->hierarchy || !thold || !thold->section || !thold->name) return -1;

  if (static_enabled == 0)
  { i_printf (1, "i_threshold_sql_update called with sub-system disabled"); return -1; }

  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "i_threshold_sql_update failed to connect to SQL database"); return -1; }  

  site_id_esc = i_postgres_escape (self->hierarchy->site_id);
  device_id_esc = i_postgres_escape (self->hierarchy->device_id);
  sect_name_esc = i_postgres_escape (thold->section->name);
  name_esc = i_postgres_escape (thold->name);
  asprintf (&command, "UPDATE thresholds SET alert_value='%.2f', alert_default='%.2f', critical_value='%.2f', critical_default='%.2f', tolerance_percent='%.2f', tolerance_default='%.2f' WHERE site_id='%s' AND device_id='%s' AND section_name='%s' AND thold_name='%s'",
    thold->alert_value, thold->alert_default, 
    thold->critical_value, thold->critical_default, 
    thold->tolerance_percent, thold->tolerance_default,
    site_id_esc, device_id_esc, sect_name_esc, name_esc);
  if (site_id_esc) free (site_id_esc);
  if (device_id_esc) free (device_id_esc);
  if (sect_name_esc) free (sect_name_esc);
  if (name_esc) free (name_esc);

  i_pg_begin (pgconn);
  res = PQexec (pgconn, command);
  free (command);
  i_pg_end (pgconn);
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    i_printf (1, "i_threshold_sql_update failed to update threshold %s in section %s (%s)", thold->name, thold->section->name, PQresultErrorMessage (res));
    PQclear (res);
    i_pg_close (pgconn);
    return -1;
  }
  PQclear (res);
  i_pg_close (pgconn);

  return 0;
}

int i_threshold_sql_del (i_resource *self, i_threshold *thold)
{
  char *command;
  PGresult *res;
  PGconn *pgconn;
  char *site_id_esc;
  char *device_id_esc;
  char *sect_name_esc;
  char *name_esc;

  if (!self || !self->hierarchy || !thold || !thold->section || !thold->name) return -1;

  if (static_enabled == 0)
  { i_printf (1, "i_threshold_sql_del called with sub-system disabled"); return -1; }

  pgconn = i_pg_connect_customer (self);            
  if (!pgconn)          
  { i_printf (1, "i_threshold_sql_del failed to connect to SQL database"); return -1; }

  site_id_esc = i_postgres_escape (self->hierarchy->site_id);
  device_id_esc = i_postgres_escape (self->hierarchy->device_id);
  sect_name_esc = i_postgres_escape (thold->section->name);
  name_esc = i_postgres_escape (thold->name);
  asprintf (&command, "DELETE FROM thresholds WHERE site_id='%s' AND device_id='%s' AND section_name='%s' AND thold_name='%s'", 
    site_id_esc, device_id_esc, sect_name_esc, name_esc);
  if (site_id_esc) free (site_id_esc);
  if (device_id_esc) free (device_id_esc);
  if (sect_name_esc) free (sect_name_esc);
  if (name_esc) free (name_esc);

  i_pg_begin (pgconn);
  res = PQexec(pgconn, command);
  free (command);
  i_pg_end (pgconn);
  if (!res || PQresultStatus(res) != PGRES_COMMAND_OK)
  {
    i_printf (1, "i_threshold_sql_del DELETE command failed for threshold %s in section %s (%s)", thold->name, thold->section->name, PQresultErrorMessage (res));
    PQclear(res);
    i_pg_close (pgconn);
    return -1;
  }
  PQclear(res);
  i_pg_close (pgconn);

  return 0;
}
