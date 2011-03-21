#include <stdlib.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/data.h>
#include <induction/incident.h>

#include "case.h"
#include "incident.h"

/* Incident Sub-Systems */

static int static_enabled = 0;
static i_msgproc_handler *static_report_handler = NULL;
static i_msgproc_handler *static_transition_handler = NULL;
static i_msgproc_handler *static_clear_handler = NULL;
static i_msgproc_handler *static_clearall_handler = NULL;
static i_hashtable *static_inc_table = NULL;
static i_hashtable *static_cleared_table = NULL;
static i_list *static_inc_list = NULL;
static i_list *static_inc_uhlist = NULL;
static i_list *static_cleared_list = NULL;
static unsigned long static_count_failed = 0;
static unsigned long static_count_impaired = 0;
static unsigned long static_count_atrisk = 0;
static unsigned long static_inclist_version = 0;    // Incremented each time the incidnt list changes

/* Variable Retrieval */

i_list* l_incident_list ()
{ return static_inc_list; }

i_list* l_incident_uhlist ()
{ return static_inc_uhlist; }

i_hashtable* l_incident_table ()
{ return static_inc_table; }

i_hashtable* l_incident_cleared_table ()
{ return static_cleared_table; }

i_list* l_incident_cleared_list ()
{ return static_cleared_list; }

unsigned long l_incident_count_failed ()
{ return static_count_failed; }
int l_incident_count_failed_inc ()
{ static_count_failed++; return 0; }
int l_incident_count_failed_dec ()
{ static_count_failed--; return 0; }

unsigned long l_incident_count_impaired ()
{ return static_count_impaired; }
int l_incident_count_impaired_inc ()
{ static_count_impaired++; return 0; }
int l_incident_count_impaired_dec ()
{ static_count_impaired--; return 0; }

unsigned long l_incident_count_atrisk ()
{ return static_count_atrisk; }
int l_incident_count_atrisk_inc ()
{ static_count_atrisk++; return 0; }
int l_incident_count_atrisk_dec ()
{ static_count_atrisk--; return 0; }

unsigned long l_incident_list_version()
{ return static_inclist_version; }
void l_incident_list_version_inc()
{ static_inclist_version++; }

/* Enable/Disable */

int l_incident_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *result;

  /* Set State */
  static_enabled = 1;

  /* Connect to SQL db */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_incident_enable failed to connect to postgres database"); l_incident_disable (self); return -1; }

  /* Check incidents SQL table */
  result = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'incidents' AND relkind = 'r'");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* Incidents table not in database */
    result = PQexec (pgconn, "CREATE TABLE incidents (id serial, state integer, start_sec integer, end_sec integer, type integer, cust_name varchar, cust_desc varchar, site_name varchar, site_desc varchar, site_suburb varchar, dev_name varchar, dev_desc varchar, cnt_name varchar, cnt_desc varchar, obj_name varchar, obj_desc varchar, met_name varchar, met_desc varchar, trg_name varchar, trg_desc varchar, adminstate integer, opstate integer, caseid integer)");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_incident_enable failed to create incidents table (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* 4.9.0 - Added 'raised_valstr' column */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='incidents' AND column_name='raised_valstr' ORDER BY ordinal_position");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    i_printf (0, "l_incident_enable version-specific check: 'raised_valstr' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE incidents ADD COLUMN raised_valstr varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_incident_enable failed to add raised_valstr column (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* 4.9.0 - Added 'cleared_valstr' column */
  i_pg_begin (pgconn);
  result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='incidents' AND column_name='cleared_valstr' ORDER BY ordinal_position");
  i_pg_end (pgconn);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* devices table not in database */
    i_printf (0, "l_incident_enable version-specific check: 'cleared_valstr' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE incidents ADD COLUMN cleared_valstr varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_incident_enable failed to add cleared_valstr column (%s)", PQresultErrorMessage (result)); }
  }
  PQclear(result);

  /* Get next incident ID */
  result = PQexec (pgconn, "SELECT nextval('incidents_id_seq')");
  if (result && PQresultStatus(result) == PGRES_TUPLES_OK && (PQntuples(result)) > 0)
  {
    char *currentid_str = PQgetvalue (result, 0, 0);
    if (currentid_str)
    { l_incident_id_setcurrent (atol(currentid_str)); }
  }
  else
  { i_printf (1, "l_incident_enable warning, failed to retrieve next incident ID number (%s)", PQresultErrorMessage (result)); }
  PQclear (result);

  /* Close SQL conn */
  i_pg_close (pgconn);

  /* 
   * Create hashtables 
   */

  static_inc_table = i_hashtable_create (DEFAULT_INCIDENT_TABLE_SIZE);
  if (!static_inc_table)
  { i_printf (1, "l_incident_enable failed to create static_inc_table"); l_incident_disable (self); return -1; }

  static_cleared_table = i_hashtable_create (DEFAULT_INCIDENT_TABLE_SIZE);
  if (!static_cleared_table)
  { i_printf (1, "l_incident_enable failed to create static_cleared_table"); l_incident_disable (self); return -1; }

  /*
   * Create lists
   */

  static_inc_list = i_list_create ();
  if (!static_inc_list)
  { i_printf (1, "l_incident_enable failed to create static_inc_list"); l_incident_disable (self); return -1; }

  static_inc_uhlist = i_list_create ();
  if (!static_inc_uhlist)
  { i_printf (1, "l_incident_enable failed to create static_inc_uhlist"); l_incident_disable (self); return -1; }

  static_cleared_list = i_list_create ();
  if (!static_cleared_list)
  { i_printf (1, "l_incident_enable failed to create static_cleared_list"); l_incident_disable (self); return -1; }

  /* 
   * Add message handlers 
   */
  
  static_report_handler = i_msgproc_handler_add (self, self->core_socket, MSG_INC_REPORT, l_incident_handler_report, NULL);
  if (!static_report_handler)
  { i_printf (1, "l_incident_enable failed to add message handler for MSG_INC_REPORT"); return -1; }

  static_transition_handler = i_msgproc_handler_add (self, self->core_socket, MSG_INC_TRANSITION, l_incident_handler_transition, NULL);
  if (!static_transition_handler)
  { i_printf (1, "l_incident_enable failed to add message handler for MSG_INC_TRANSITION"); return -1; }

  static_clear_handler = i_msgproc_handler_add (self, self->core_socket, MSG_INC_CLEAR, l_incident_handler_clear, NULL);
  if (!static_clear_handler)
  { i_printf (1, "l_incident_enable failed to add message handler for MSG_INC_CLEAR"); return -1; }

  static_clearall_handler = i_msgproc_handler_add (self, self->core_socket, MSG_INC_CLEARALL, l_incident_handler_clearall, NULL);
  if (!static_clear_handler)
  { i_printf (1, "l_incident_enable failed to add message handler for MSG_INC_CLEARALL"); return -1; }

  return 0;
}

int l_incident_disable (i_resource *self)
{
  static_enabled = 0;

  /* Free Hashtables */
  if (static_inc_table)
  { i_hashtable_free (static_inc_table); static_inc_table = NULL; }
  if (static_cleared_table)
  { i_hashtable_free (static_cleared_table); static_cleared_table = NULL; }

  /* Free lists */
  if (static_inc_list)
  { i_list_free (static_inc_list); static_inc_list = NULL; }
  if (static_inc_uhlist)
  { i_list_free (static_inc_uhlist); static_inc_uhlist = NULL; }
  if (static_cleared_list)
  { i_list_free (static_cleared_list); static_cleared_list = NULL; }

  /* Remove Message Handlers */
  if (static_report_handler)
  { i_msgproc_handler_remove (self, static_report_handler); static_report_handler = NULL; }
  if (static_clear_handler)
  { i_msgproc_handler_remove (self, static_clear_handler); static_clear_handler = NULL; }
  if (static_clearall_handler)
  { i_msgproc_handler_remove (self, static_clearall_handler); static_clearall_handler = NULL; }

  return 0;
}
