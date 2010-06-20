#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "induction.h"
#include "list.h"
#include "hierarchy.h"
#include "callback.h"
#include "postgresql.h"
#include "ip.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"

/** \addtogroup metric_record Recording and Graphing
 * @ingroup metric
 * @{
 */

/* Metric Values - Loading of historic values */

i_callback* i_metric_load_sql (i_resource *self, i_metric *met, unsigned short period, time_t ref_sec, int (*cbfunc) (i_resource *self, i_metric *met, i_list *val_list, void *passdata), void *passdata)
{
  /* Load all records from the SQL database for the
   * specified metric that fall within the period
   * set by the period variable and in which the
   * ref_sec time occurs.
   */
  int num;
  int mday = 0;
  int month = 0;
  int year = 0;
  i_callback *cb;
  char *query;
  char *table_str;
  struct tm ref_tm;
  i_pg_async_conn *conn;

  /* Convert ref_sec to a ref_tm */
  localtime_r (&ref_sec, &ref_tm);

  /* Create period-specific condition string */
  switch (period)
  {
    case VALPERIOD_DAY:
      mday = ref_tm.tm_mday;
    case VALPERIOD_MONTH:
      month = ref_tm.tm_mon+1;
    case VALPERIOD_YEAR:
      year = ref_tm.tm_year+1900;
      break;
    default:
      i_printf (1, "i_metric_load_sql failed, unspported period (%i) specified", period);
      return NULL;
  }

  /* Open SQL Database connection */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_metric_load_sql failed to open SQL database connection for metric %s", met->name_str); return NULL; }
  
  /* Create query */
  asprintf (&table_str, "%s_metrics", met->obj->cnt->name_str);
  asprintf (&query, "SELECT valstr, valnum, tstamp FROM %s WHERE site='%s' AND device='%s' AND object='%s' AND metric='%s' AND mday='%i' AND month='%i' AND year='%i'", 
    table_str, self->hierarchy->site_id, self->hierarchy->device_id, met->obj->name_str, met->name_str, mday, month, year);
  free (table_str);

  /* Create callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->passdata = passdata;
  cb->data = i_entity_address_string (ENTITY(met), NULL);

  /* Execute the query */
  num = i_pg_async_query_exec (self, conn, query, 0, i_metric_load_sql_sqlcb, cb);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_metric_load_sql failed to execute SELECT query for metric %s", met->name_str);
    i_callback_free (cb);
    return NULL;
  }

  return cb;
}

int i_metric_load_sql_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *res, void *passdata)
{
  int num;
  i_list *val_list;
  i_callback *cb = passdata;

  /* Find metric.
   *
   * This has to be done because unfortunately, the metric we've
   * done the load for MAY have been killed off before we return with
   * it's result. Typically, this happens when dupname handling in
   * snmp objectfact is enabled.
   */
  i_entity_address *metaddr = i_entity_address_struct ((char *)cb->data);
  i_metric *met = (i_metric *) i_entity_local_get (self, metaddr);

  if (!met)
  {
    /* Metric doesn't exist any more, fail without 
     * calling the callback
     */
    if (cb) 
    {
      if (cb->data) free (cb->data);  /* Address string */
      i_callback_free (cb);
    }
    i_pg_async_conn_close (conn);
    return 0;
  }
  
  /* Create val_list */
  val_list = i_list_create ();
  i_list_set_destructor (val_list, i_metric_value_free);

  if (res && PQresultStatus(res) == PGRES_TUPLES_OK)
  {
    /* Rows (0 or more) present. Create the
     * value list and interpret each row
     */
    int y;
    int row_count;

    /* Loop through rows */
    row_count = PQntuples (res);
    for (y=0; y < row_count; y++)
    {
      int x;
      int field_count;
      double valflt = 0;
      time_t tstamp_sec = 0;
      char *valstr = NULL;
      i_metric_value *val;

      /* Loop through each field */
      field_count = PQnfields (res);
      for (x=0; x < field_count; x++)
      {
        char *fval;

        fval = PQgetvalue (res, y, x);
        switch (x)
        {
          case 0:
            /* Value String */
            valstr = fval;
            break;
          case 1:
            /* Value Numeric */
            valflt = atof (fval);
            break;
          case 2:
            /* Timestamp */
            tstamp_sec = atol (fval);
            break;
        }
      }

      /* Create value */
      val = i_metric_value_create ();

      /* Set value */
      if (i_metric_valisnum(met) == 0)
      {
        /* Numeric representation */
        i_metric_valflt_set (met, val, valflt);
      }
      else
      {
        /* String representation */
        i_metric_valstr_set (met, val, valstr);
      }

      /* Set timestamp */
      val->tstamp.tv_sec = tstamp_sec;
      val->tstamp.tv_usec = 0;

      /* Enqueue */
      num = i_list_enqueue (val_list, val);
      if (num != 0)
      { 
        i_printf (1, "i_metric_load_sql_sqlcb warning, failed to enqueue a value");
        i_metric_value_free (val);
      }

      /* End of row processing */
    }
    /* End of result processing */
  }

  /* Run the callback */
  if (cb && cb->func)
  { cb->func (self, met, val_list, cb->passdata); }
  if (cb)
  {
    if (cb->data) free (cb->data); 
    i_callback_free (cb); 
  }

  /* Free list */
  if (val_list) i_list_free (val_list);

  /* Close connection */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Same as above, but synchronous */

i_list* i_metric_load_sql_sync (i_resource *self, i_metric *met, unsigned short period, time_t ref_sec)
{
  /* Load all records from the SQL database for the
   * specified metric that fall within the period
   * set by the period variable and in which the
   * ref_sec time occurs.
   */
  int num;
  int mday = 0;
  int month = 0;
  int year = 0;
  char *query;
  char *table_str;
  struct tm ref_tm;
  PGconn *conn;
  PGresult *res;

  /* Convert ref_sec to a ref_tm */
  localtime_r (&ref_sec, &ref_tm);

  /* Create period-specific condition string */
  switch (period)
  {
    case VALPERIOD_DAY:
      mday = ref_tm.tm_mday;
    case VALPERIOD_MONTH:
      month = ref_tm.tm_mon+1;
    case VALPERIOD_YEAR:
      year = ref_tm.tm_year+1900;
      break;
    default:
      i_printf (1, "i_metric_load_sql_sync failed, unspported period (%i) specified", period);
      return NULL;
  }
  
  /* Connect */
  conn = i_pg_connect_customer (self);
  if (!conn)
  { i_printf (1, "i_metric_load_sql_sync failed to open SQL database connection for metric %s", met->name_str); return NULL; }

  /* Create query */
  asprintf (&table_str, "%s_metrics", met->obj->cnt->name_str);
  asprintf (&query, "SELECT valstr, valnum, tstamp FROM %s WHERE site='%s' AND device='%s' AND object='%s' AND metric='%s' AND mday='%i' AND month='%i' AND year='%i'", 
    table_str, self->hierarchy->site_id, self->hierarchy->device_id, met->obj->name_str, met->name_str, mday, month, year);
  free (table_str);

  /* Execute query */
  i_pg_begin (conn);
  res = PQexec (conn, query);
  free (query);
  i_pg_end (conn);

  /* Create val_list */
  i_list *val_list = i_list_create ();
  i_list_set_destructor (val_list, i_metric_value_free);

  if (res && PQresultStatus(res) == PGRES_TUPLES_OK)
  {
    /* Rows (0 or more) present. Create the
     * value list and interpret each row
     */
    int y;
    int row_count;

    /* Loop through rows */
    row_count = PQntuples (res);
    for (y=0; y < row_count; y++)
    {
      int x;
      int field_count;
      double valflt = 0;
      time_t tstamp_sec = 0;
      char *valstr = NULL;
      i_metric_value *val;

      /* Loop through each field */
      field_count = PQnfields (res);
      for (x=0; x < field_count; x++)
      {
        char *fval;

        fval = PQgetvalue (res, y, x);
        switch (x)
        {
          case 0:
            /* Value String */
            valstr = fval;
            break;
          case 1:
            /* Value Numeric */
            valflt = atof (fval);
            break;
          case 2:
            /* Timestamp */
            tstamp_sec = atol (fval);
            break;
        }
      }

      /* Create value */
      val = i_metric_value_create ();

      /* Set value */
      if (i_metric_valisnum(met) == 0)
      {
        /* Numeric representation */
        i_metric_valflt_set (met, val, valflt);
      }
      else
      {
        /* String representation */
        i_metric_valstr_set (met, val, valstr);
      }

      /* Set timestamp */
      val->tstamp.tv_sec = tstamp_sec;
      val->tstamp.tv_usec = 0;

      /* Enqueue */
      num = i_list_enqueue (val_list, val);
      if (num != 0)
      { 
        i_printf (1, "i_metric_load_sql_sync warning, failed to enqueue a value");
        i_metric_value_free (val);
      }

      /* End of row processing */
    }
    /* End of result processing */
  }

  /* Close connection */
  PQclear (res);
  i_pg_close (conn);

  return val_list;
}

/* @} */
