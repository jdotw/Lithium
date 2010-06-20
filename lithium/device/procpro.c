#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/postgresql.h>

#include "procpro.h"

/*
 * 'Process Profiling' -- Monitoring of the hrSwRun table
 *
 */

/* Enable Sub-System */

xmlNodePtr l_procpro_xml (l_procpro *procpro);

static i_container *static_cnt = NULL;
static i_list *static_list = NULL;

i_container* l_procpro_cnt ()
{ return static_cnt; }

i_list* l_procpro_list ()
{ return static_list; }

int l_procpro_enable (i_resource *self)
{
  PGconn *pgconn;
  PGresult *pgres;

  if (!self->hierarchy->dev->swrun) return 0;

  /* Connect to SQL */
  pgconn = i_pg_connect_customer (self);
  if (!pgconn)
  { i_printf (1, "l_procpro_enable failed to connect to postgres database"); return -1; }

  /* Check procpro table */
  pgres = PQexec (pgconn, "SELECT relname FROM pg_class WHERE relname = 'procpro' AND relkind = 'r'");
  if (!pgres || PQresultStatus(pgres) != PGRES_TUPLES_OK || (PQntuples(pgres)) < 1)
  {
    /* Table not found, create it */
    pgres = PQexec (pgconn, "CREATE TABLE procpro (id serial, descr varchar, match varchar, argmatch varchar, site varchar, device varchar)");
    if (!pgres || PQresultStatus(pgres) != PGRES_COMMAND_OK)
    { i_printf (1, "l_procpro_enable failed to create procpros table (%s)", PQresultErrorMessage (pgres)); }
  }
  PQclear (pgres);

  /* 5.0.7 - Added 'argmatch' column */
  PGresult *result = PQexec (pgconn, "SELECT column_name from information_schema.columns WHERE table_name='procpro' AND column_name='argmatch' ORDER BY ordinal_position");
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    if (result) { PQclear(result); result = NULL; }
    i_printf (0, "l_procpro_enable version-specific check: 'procpro.argmatch' column missing, attempting to add it");
    result = PQexec (pgconn, "ALTER TABLE procpro ADD COLUMN argmatch varchar");
    if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
    { i_printf (1, "l_procpro_enable failed to add group column (%s)", PQresultErrorMessage (result)); }
  }
  if (result) { PQclear(result); result = NULL; }

  /* Close SQL */
  i_pg_close (pgconn);

  /* Create/Config Container */
  static_cnt = i_container_create ("procpro", "Processes");
  if (!static_cnt)
  { i_printf (1, "l_procpro_enable failed to create container"); return -1; }
  
  /* Register entity */
  int num = i_entity_register (self, ENTITY(self->hierarchy->dev), ENTITY(static_cnt));
  if (num != 0)
  { i_printf (1, "l_procpro_enable failed to register container"); return -1; }
  
  /* Load/Apply refresh config */
  static i_entity_refresh_config defrefconfig;
  memset (&defrefconfig, 0, sizeof(i_entity_refresh_config));
  defrefconfig.refresh_method = REFMETHOD_EXTERNAL;
  defrefconfig.refresh_int_sec = REFDEFAULT_REFINTSEC;
  defrefconfig.refresh_maxcolls = REFDEFAULT_MAXCOLLS;
  num = i_entity_refresh_config_loadapply (self, ENTITY(static_cnt), &defrefconfig);
  if (num != 0)  { i_printf (1, "l_procpro_enable failed to load and apply container refresh config"); return -1; }
  
  /* Create item list */
  static_cnt->item_list = i_list_create ();
  static_cnt->item_list_state = ITEMLIST_STATE_NORMAL;

  /* Process Running Trigger */
  i_triggerset *tset;
  tset = i_triggerset_create ("status", "Status", "status");
  i_triggerset_addtrg (self, tset, "not_running", "Not Running", VALTYPE_INTEGER, TRGTYPE_EQUAL, 0, NULL, 0, NULL, 0, ENTSTATE_FATAL, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Process Count Trigger */
  tset = i_triggerset_create ("count", "Process Count", "count");
  i_triggerset_addtrg (self, tset, "low", "Low", VALTYPE_INTEGER, TRGTYPE_LT, 5, NULL, 0, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "high", "High", VALTYPE_INTEGER, TRGTYPE_GT, 20, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 0;
  i_triggerset_assign (self, static_cnt, tset);

  /* Total CPU Usage Trigger */
  tset = i_triggerset_create ("cpu_total", "Total CPU Usage", "cpu_total");
  tset->default_duration = 15 * 60; /* 15-minute hold-down */
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 95, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 95, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Max-Single CPU Usage Trigger */
  tset = i_triggerset_create ("cpu_maxsingle", "Max Single Proc. CPU Usage", "cpu_maxsingle");
  tset->default_duration = 15 * 60; /* 15-minute hold-down */
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, 80, NULL, 95, NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, 95, NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  i_triggerset_assign (self, static_cnt, tset);

  /* Total Mem Usage Trigger */
  tset = i_triggerset_create ("mem_total", "Total Memory Usage", "mem_total");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, (500.0 * 1024.0 * 1024.0), NULL, (1000.0 * 1024.0 * 1024.0), NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, (1000.0 * 1024.0 * 1024.0), NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 0;
  i_triggerset_assign (self, static_cnt, tset);

  /* Max-Single Mem Usage Trigger */
  tset = i_triggerset_create ("mem_maxsingle", "Max Single Proc. Memory Usage", "mem_maxsingle");
  i_triggerset_addtrg (self, tset, "warning", "Warning", VALTYPE_FLOAT, TRGTYPE_RANGE, (500.0 * 1024.0 * 1024.0), NULL, (1000.0 * 1024.0 * 1024.0), NULL, 0, ENTSTATE_WARNING, TSET_FLAG_VALAPPLY);
  i_triggerset_addtrg (self, tset, "impaired", "Impaired", VALTYPE_FLOAT, TRGTYPE_GT, (1000.0 * 1024.0 * 1024.0), NULL, 0, NULL, 0, ENTSTATE_IMPAIRED, TSET_FLAG_VALAPPLY);
  tset->default_applyflag = 0;
  i_triggerset_assign (self, static_cnt, tset);

  /* Load procpros */
  l_procpro_sql_load_list (self, NULL, self->hierarchy->site->name_str, self->hierarchy->dev->name_str, l_procpro_enable_loadcb, NULL);

  return 0;
}

int l_procpro_enable_loadcb (i_resource *self, i_list *list, void *passdata)
{
  /* Create objects for procpros */
  l_procpro *procpro;
  for (i_list_move_head(list); (procpro=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    l_procpro_object_add (self, procpro);
  }

  static_list = list;

  return 1;     /* Return 1 to prevent sqlcb freeing the list */
}

/* Struct */

l_procpro* l_procpro_create ()
{
  l_procpro *procpro = malloc (sizeof(l_procpro));
  memset (procpro, 0, sizeof(l_procpro));

  return procpro;
}

void l_procpro_free (void *procproptr)
{
  l_procpro *procpro = procproptr;

  if (procpro->desc_str) free (procpro->desc_str);
  if (procpro->match_str) free (procpro->match_str);
  if (procpro->argmatch_str) free (procpro->argmatch_str);

  free (procpro);
}

