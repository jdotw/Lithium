#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "path.h"
#include "postgresql.h"
#include "hierarchy.h"
#include "cement.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "str.h"

/** \addtogroup metric_record Recording and Graphing
 * @ingroup metric
 * @{
 */

/* 
 * Metric SQL Recording
 */

/* Recording data struct manipulation */

i_metric_record_sql_data* i_metric_record_sql_data_create ()
{
  i_metric_record_sql_data *data;

  data = (i_metric_record_sql_data *) malloc (sizeof(i_metric_record_sql_data));
  if (!data)
  { i_printf (1, "i_metric_record_sql_data_create failed to malloc i_metric_record_sql_data struct"); return NULL; }
  memset (data, 0, sizeof(i_metric_record_sql_data));

  return data;
}

void i_metric_record_sql_data_free (void *dataptr)
{
  i_metric_record_sql_data *data = dataptr;

  if (!data) return;

  if (data->valstr) free (data->valstr);
  if (data->tablestr) free (data->tablestr);
  if (data->metnamestr) free (data->metnamestr);
  if (data->objnamestr) free (data->objnamestr);

  free (data);
}

int i_metric_record_sql (i_resource *self, i_metric *met)
{
  int num;
  char *query;
  struct tm ref_tm;
  i_pg_async_conn *conn;
  i_metric_value *curval;
  i_metric_value *preval;
  i_metric_record_sql_data *data;

  /* Retrieve current value */
  i_list_move_head (met->val_list);
  curval = i_list_restore (met->val_list);
  if (!curval)
  { i_printf (1, "i_metric_record_sql failed, no current value for metric %s", met->name_str); return -1; }

  /* Retrieve previous value */
  i_list_move_next (met->val_list);
  preval = i_list_restore (met->val_list);
  if (preval)
  {
    /* Compare values to see if update is really necessary */
    if (i_metric_valisnum(met) == 0)
    {
      float curflt = i_metric_valflt (met, curval);
      float preflt = i_metric_valflt (met, preval);
      if (curflt == preflt)
      { 
        /* UPDATE is not necessary, previous and current 
         * values are identical. 
         */
        return 0;
      }
    }
  }

  /* Check Metrics SQL Table */
  i_container *cnt = met->obj->cnt;
  if (cnt->metric_sql_checked == 0)
  {
    conn = i_pg_async_conn_open_customer (self);
    if (conn)
    {
      char *query;
      char *cnt_esc;

      /* Create query */
      cnt_esc = i_postgres_escape (cnt->name_str);
      char *lower_esc = i_string_to_lower (cnt_esc);
      free (cnt_esc);
      asprintf (&query, "SELECT relname FROM pg_class WHERE relname = '%s_metrics' AND relkind = 'r'", lower_esc);
      free (lower_esc);

      /* Exec query */
      num = i_pg_async_query_exec (self, conn, query, 0, i_container_register_sqlselectcb, strdup(cnt->name_str));
      free (query);
      if (num != 0)
      { i_printf (1, "i_metric_record_sql warning, failed to exec SQL query to check for container %s's metrics table", cnt->name_str); }
    }
    else
    { i_printf (1, "i_metric_record_sql warning, failed to open SQL database connection to create the metrics table"); }
    
    cnt->metric_sql_checked = 1;
  }

  /* Create callback data */
  data = i_metric_record_sql_data_create ();
  if (!data)
  { i_printf (1, "i_metric_record_sql failed to create data struct"); return -1; }
  data->ent_type = met->ent_type;
  data->metnamestr = strdup (met->name_str);
  data->objnamestr = strdup (met->obj->name_str);
  data->tstamp_sec = time (NULL);
  
  /* Create table str */
  asprintf (&data->tablestr, "%s_metrics", met->obj->cnt->name_str); 

  /* Set data->valstr or data->valflt depending
   * on whether or not the metrics value can be
   * represented as a floating point (valflt)
   * or not (valstr)
   */

  if (i_metric_valisnum(met) == 0)
  {
    /* Numeric representation */
    data->valflt = i_metric_valflt (met, curval);
    data->valstr = strdup ("");
  }
  else
  {
    /* Retrieve current value raw string form */
    data->valstr = i_metric_valstr_raw (met, curval);
    if (!data->valstr)
    { 
      i_printf (1, "i_metric_record_sql failed to retrieve current value in raw string form for %s %s",
        i_entity_typestr(met->ent_type), met->name_str);
      i_metric_record_sql_data_free (data);
      return -1;
    }
  }

  /* Configure the mday/month/year values based
   * on the metrics val_period flag
   */
  localtime_r ((time_t *) &curval->tstamp.tv_sec, &ref_tm);
  switch (met->val_period)
  {
    case VALPERIOD_DAY:
      data->mday  = ref_tm.tm_mday;
      data->month = ref_tm.tm_mon+1;
      data->year  = ref_tm.tm_year+1900;
      break;
    case VALPERIOD_MONTH:
      data->mday  = 0;
      data->month = ref_tm.tm_mon+1;
      data->year  = ref_tm.tm_year+1900;
      break;
    case VALPERIOD_YEAR:
      data->mday  = 0;
      data->month = 0;
      data->year  = ref_tm.tm_year+1900;
      break;
    case VALPERIOD_INSTANT:
      data->mday  = 0;
      data->month = 0;
      data->year  = 0;
      break;
    default:
      i_printf (1, "i_metric_record_sql unknown met->val_period %i, value not recorded for %s %s",
        met->val_period, i_entity_typestr(met->ent_type), met->name_str);
      i_metric_record_sql_data_free (data);
      return -1;
  }

  /* Create the 'UPDATE' query str */
  asprintf (&query, "UPDATE %s SET valstr='%s', valnum='%.2f', tstamp='%li' WHERE site='%s' AND device='%s' AND object='%s' AND metric='%s' AND mday='%i' AND month='%i' AND year='%i'",
    data->tablestr, data->valstr, data->valflt, data->tstamp_sec, self->hierarchy->site_id, self->hierarchy->device_id, met->obj->name_str, met->name_str, 
    data->mday, data->month, data->year);

  /* Open database connection */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { 
    i_printf (1, "i_metric_record_sql failed to open SQL database connection for %s %s",
      i_entity_typestr(met->ent_type), met->name_str);
    i_metric_record_sql_data_free (data);
    free (query);
    return -1;
  }
  
  /* Execute the update */
  num = i_pg_async_query_exec (self, conn, query, 0, i_metric_record_sql_updatecb, data);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_metric_record_sql failed to execute UPDATE query for %s %s",
      i_entity_typestr(met->ent_type), met->name_str);
    i_metric_record_sql_data_free (data);
    free (query);
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

int i_metric_record_sql_updatecb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  int num;
  char *cmdtuples;
  i_metric_record_sql_data *data = passdata;

  /* Check how many rows were affected */
  if (result) 
  { cmdtuples = PQcmdTuples (result); }
  else 
  { cmdtuples = NULL; }
  if (!cmdtuples || atoi(cmdtuples) == 0)
  {
    /* No rows affected, perform the INSERT query for 
     * this value. This is necessary when this value is
     * the first entry for a given period.
     */
    char *query;

    /* Create INSERT query str */
    asprintf (&query, "INSERT INTO %s (site, device, object, metric, mday, month, year, valstr, valnum, tstamp) VALUES ('%s', '%s', '%s', '%s', '%i', '%i', '%i', '%s', '%.2f', '%li')",
      data->tablestr, self->hierarchy->site_id, self->hierarchy->device_id, data->objnamestr, data->metnamestr,
      data->mday, data->month, data->year, data->valstr, data->valflt, data->tstamp_sec);

    /* Execute the INSERT query */
    num = i_pg_async_query_exec (self, conn, query, 0, i_metric_record_sql_insertcb, data);
    free (query);
    if (num != 0)
    {
      i_printf (1, "i_metric_record_sql_updatecb failed to execute INSERT query for %s %s",
        i_entity_typestr(data->ent_type), data->metnamestr);
      i_metric_record_sql_data_free (data);
      free (query);
      i_pg_async_conn_close (conn);
      return -1;
    }
  }
  else
  {
    /* Update was successful */
    i_pg_async_conn_close (conn);
    i_metric_record_sql_data_free (data);
  }

  return 0;
}

int i_metric_record_sql_insertcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  i_metric_record_sql_data *data = passdata;

  if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
  {
    i_printf (1, "i_metric_record_sql_insertcb failed to execute INSERT query for %s %s",
      i_entity_typestr(data->ent_type), data->metnamestr);
  }

  i_pg_async_conn_close (conn);
  i_metric_record_sql_data_free (data);

  return 0;
}

/* @} */
