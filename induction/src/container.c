#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "induction.h"
#include "list.h"
#include "hashtable.h"
#include "auth.h"
#include "path.h"
#include "form.h"
#include "postgresql.h"
#include "navtree.h"
#include "navform.h"
#include "entity.h"
#include "device.h"
#include "object.h"
#include "metric.h"
#include "str.h"
#include "container.h"

/** \addtogroup container Containers
 * @ingroup entity
 * @{
 */

/* 
 * CEMent Containers 
 */

/* Struct manipulation */

i_container* i_container_create (char *name_str, char *desc_str)
{
  i_container *cnt;

  cnt = (i_container *) i_entity_create (name_str, desc_str, ENT_CONTAINER, sizeof(i_container));
  if (!cnt)
  { i_printf (1, "i_container_create failed to create container entity"); return NULL; }
  cnt->refresh_func = i_entity_refresh_children;  /* Default refresh func */
  cnt->authorative = 1;

  /* Triggerset List */
  cnt->tset_list = i_list_create ();
  if (!cnt->tset_list)
  { i_printf (1, "i_container_create failed to create triggerset list"); i_entity_free (ENTITY(cnt)); return NULL; }

  /* Triggerset Hashtable */
  cnt->tset_ht = i_hashtable_create (CNT_TSETHT_SIZE);
  if (!cnt->tset_ht)
  { i_printf (1, "i_container_create failed to create triggerset hashtable"); i_entity_free (ENTITY(cnt)); return NULL; }

  /* Metric Template list */
  cnt->met_templates = i_list_create ();

  /* Triggerset List */
  cnt->wview_metrics = i_list_create ();

  
  return cnt;
}

void i_container_free (void *cntptr)
{
  /* Just free the container-specific 
   * portions of the struct. The struct
   * and everything else will be freed
   * by i_entity_free
   */
  i_container *cnt = cntptr;

  if (!cnt) return;

  if (cnt->item_list) i_list_free (cnt->item_list);
  if (cnt->tset_list) i_list_free (cnt->tset_list);
  if (cnt->tset_ht) i_hashtable_free (cnt->tset_ht);
  if (cnt->met_templates) i_list_free (cnt->met_templates);
  if (cnt->cview1_str) free (cnt->cview1_str);
  if (cnt->cview2_str) free (cnt->cview2_str);
  if (cnt->cview3_str) free (cnt->cview3_str);
  if (cnt->cview4_str) free (cnt->cview4_str);
  if (cnt->wview_metrics) i_list_free (cnt->wview_metrics);

  /* No need to free the cnt itself!! */
}

/* 
 * Container-specific registration tasks
 * Should only be called by i_entity_register
 */

int i_container_register (i_resource *self, i_device *dev, i_container *cnt)
{
  /* Register the specificed container cnt 
   * to the current device. 
   *
   * Returns -1 on failure.
   * Once a container has been successfully been registered it
   * should not be freed unless through i_container_deregister
   */


  return 0;
}

int i_container_register_sqlselectcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  char *cntname_str = passdata;

  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  {
    /* Metrics table does not exist */
    int num;
    char *cnt_esc;
    char *query;

    /* Create query */
    cnt_esc = i_postgres_escape (cntname_str);
    asprintf (&query, "CREATE TABLE %s_metrics (record_id serial PRIMARY KEY, site varchar, device varchar, object varchar, metric varchar, mday int, month int, year int, valstr varchar, valnum numeric, tstamp integer)", cnt_esc);
    free (cnt_esc);

    /* Exec query */
    num = i_pg_async_query_exec (self, conn, query, 0, i_container_register_sqlcreatecb, cntname_str);
    free (query);
    if (num != 0)
    { 
      i_printf (1, "i_container_register_sqlselectcb failed to execute CREATE TABLE query for container %s", cntname_str);
      i_pg_async_conn_close (conn);
      free (cntname_str);
      return -1;
    }
  }
  else
  {
    /* Table exists */
    i_pg_async_conn_close (conn);
    free (cntname_str);
  }

  return 0;
}

int i_container_register_sqlcreatecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  char *cntname_str = passdata;

  if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
  { i_printf (1, "i_container_register_sqlcreatecb warning, failed to create metrics table (%s)", cntname_str); }

  if (cntname_str)
  {
    /* Now create the triggers table */
    int num;
    char *cnt_esc;
    char *query;

    /* Create query */
    cnt_esc = i_postgres_escape (cntname_str);
    asprintf (&query, "CREATE TABLE %s_triggers (site varchar, device varchar, object varchar, metric varchar, trigger valstr, valstr varchar, valnum numeric, yvalstr varchar, yvalnum numeric)", cnt_esc);
    free (cnt_esc);

    /* Exec query */
    num = i_pg_async_query_exec (self, conn, query, 0, i_container_register_sqlcreatecb, NULL);
    free (query);
    if (num != 0)
    { i_printf (1, "i_container_register_sqlselectcb failed to execute CREATE TABLE query for container %s", cntname_str); }

    free (cntname_str);
  }
  else
  { i_pg_async_conn_close (conn); }

  return 0;
}

/* 
 * Container-specific deregistration tasks
 * Should only be called by i_entity_deregister
 */

int i_container_deregister (i_resource *self, i_container *cnt)
{ 
  return 0; 
}

/* @} */
