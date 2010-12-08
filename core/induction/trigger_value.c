#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "hierarchy.h"
#include "callback.h"
#include "postgresql.h"
#include "name.h"
#include "entity.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "value.h"
#include "trigger.h"

/** \addtogroup trigger Triggers
 * @ingroup entity
 * @{
 */

/* 
 * CEMent Trigggers - Value Manipulation
 */

/* Value Loading */

i_callback* i_trigger_value_load (i_resource *self, i_trigger *trg, int (*cbfunc) (i_resource *self, i_trigger *trg, i_value *val, i_value *yval, void *passdata), void *passdata)
{
  /* Load a value from the SQL database for
   * the specified trigger. The select statement
   * will also attempt to load 
   * customer/site/device/container/object/metric
   * default values.
   */

  int num;
  char *site_esc;
  char *device_esc;
  char *cnt_esc;
  char *obj_esc;
  char *met_esc;
  char *trg_esc;
  char *query;
  i_pg_async_conn *conn;
  i_callback *cb;

  /* Open SQL Connection */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { i_printf (1, "i_trigger_value_load failed to connect to SQL database for trigger %s", trg->name_str); return NULL; }

  /* Create query */
  site_esc = i_postgres_escape (self->hierarchy->site_id);
  device_esc = i_postgres_escape (self->hierarchy->device_id);
  cnt_esc = i_postgres_escape (trg->met->obj->cnt->name_str);
  obj_esc = i_postgres_escape (trg->met->obj->name_str);
  met_esc = i_postgres_escape (trg->met->name_str);
  trg_esc = i_postgres_escape (trg->name_str);
  asprintf (&query, "SELECT valnum, valstr, yvalnum, yvalstr FROM %s_triggers WHERE (site IS NULL OR site='%s') AND (device IS NULL OR device='%s') AND (container IS NULL or container='%s') AND (object IS NULL OR object='%s') AND (metric IS NULL OR metric='%s') AND (trigger IS NULL OR trigger='%s') ORDER BY trigger, metric, object, container, device, site, LIMIT 1",
    cnt_esc, site_esc, device_esc, cnt_esc, obj_esc, met_esc, trg_esc);
  free (site_esc);
  free (device_esc);
  free (cnt_esc);
  free (obj_esc);
  free (met_esc);
  free (trg_esc);

  /* Create callback */
  cb = i_callback_create ();
  cb->func = cbfunc;
  cb->data = trg;
  cb->passdata = passdata;

  /* Execute Query */
  num = i_pg_async_query_exec (self, conn, query, 0, i_trigger_value_load_sqlcb, cb);
  free (query);
  if (num != 0)
  { 
    i_printf (1, "i_trigger_value_load failed to execute SQL SELECT query for trigger %s", trg->name_str);
    i_pg_async_conn_close (conn);
    return NULL;
  }

  return cb;
}

int i_trigger_value_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  /* Called when the SQL query for the trigger
   * value retrieval is complete
   */
  i_callback *cb = passdata;
  i_trigger *trg = cb->data;
  i_value *val = NULL;
  i_value *yval = NULL;

  /* Check/Interpret Result */
  if (result && PQntuples (result) > 0)
  {
    /* Data to intepret */
    int i;
    int field_count;
    double valflt = 0;
    char *valstr = NULL;
    double yvalflt = 0;
    char *yvalstr = NULL;

    /* Interpret SQL row */
    field_count = PQnfields (result);
    for (i=0; i < field_count; i++)
    {
      char *fname;
      char *fval;

      fname = PQfname (result, i);
      fval = PQgetvalue (result, 0, i);

      if (fname && !strcmp(fname, "valnum"))
      { valflt = atof (fval); }
      else if (fname && !strcmp(fname, "valstr"))
      { valstr = strdup (fval); }
      if (fname && !strcmp(fname, "yvalnum"))
      { yvalflt = atof (fval); }
      else if (fname && !strcmp(fname, "yvalstr"))
      { yvalstr = strdup (fval); }
    }

    /* Create/Set Value */
    if (i_value_isnum (trg->val_type) == 1)
    {
      /* Numeric Value */
      val = i_value_valflt_set (trg->val_type, NULL, valflt);
      if (trg->trg_type == TRGTYPE_RANGE)
      { yval = i_value_valflt_set (trg->val_type, NULL, yvalflt); }
    }
    else
    {
      /* String Value */
      val = i_value_valstr_set (trg->val_type, NULL, valstr);
    }

    /* Cleanup */
    if (valstr) free (valstr);
    if (yvalstr) free (yvalstr);
  }

  /* Run Callback */
  if (cb && cb->func)
  { cb->func (self, trg, val, yval, cb->passdata); }
  if (cb)
  { i_callback_free (cb); }

  /* Free values */
  if (val) i_value_free (val);
  if (yval) i_value_free (yval);

  /* Close SQL Connection */
  i_pg_async_conn_close (conn); 

  return 0;
}

/* Value Application */

int i_trigger_value_apply (i_resource *self, i_trigger *trg, i_value *val, i_value *yval)
{
  /* Apply the specified value to the 
   * trigger. If an existing value is 
   * present it will be freed. After the
   * new value is applied, trigger_process 
   * function will be called to process 
   * the new trigger value.
   */

  if (val)
  {
    /* Check/free existing */
    if (trg->val)
    { i_value_free (trg->val); }

    /* Attach new value */
    trg->val = val;
  }

  if (yval)
  {
    /* Check/free existing */
    if (trg->yval)
    { i_value_free (trg->yval); }

    /* Attach new value */
    trg->yval = yval;
  }

  /* If the trigger is registered, 
   * call i_trigger_process to process
   * the new value
   */
  if (trg->registered == 1)
  { i_trigger_process (self, trg); }

  return 0;
}

/* Value Load+Apply Utility */

i_trigger_value_loadapply_cbdata* i_trigger_value_loadapply_cbdata_create ()
{
  i_trigger_value_loadapply_cbdata *cbdata;

  cbdata = (i_trigger_value_loadapply_cbdata *) malloc (sizeof(i_trigger_value_loadapply_cbdata));
  if (!cbdata) 
  { i_printf (1, "i_trigger_value_loadapply_cbdata failed to malloc i_trigger_value_loadapply_cbdata"); return NULL; }
  memset (cbdata, 0, sizeof (i_trigger_value_loadapply_cbdata));

  return cbdata;
}

void i_trigger_value_loadapply_cbdata_free (void *cbdataptr)
{
  i_trigger_value_loadapply_cbdata *cbdata = cbdataptr;

  if (!cbdata) return;

  if (cbdata->def_val) i_value_free (cbdata->def_val);
  if (cbdata->def_yval) i_value_free (cbdata->def_yval);

  free (cbdata);
}

i_callback* i_trigger_value_loadapply (i_resource *self, i_trigger *trg, i_value *def_val, i_value *def_yval)
{
  /* Attempt to load a value for the 
   * specified trigger. If a value is found,
   * that value will be used. Otherwise, the
   * defval (default value) will be applied
   * if specified.
   */

  i_callback *cb;
  i_trigger_value_loadapply_cbdata *cbdata;

  /* Create cbdata */
  cbdata = i_trigger_value_loadapply_cbdata_create ();
  
  /* Duplicate default values */
  if (def_val)
  { cbdata->def_val = i_value_duplicate (def_val); }
  if (def_yval)
  { cbdata->def_yval = i_value_duplicate (def_yval); }

  /* Begin load operation */
  cb = i_trigger_value_load (self, trg, i_trigger_value_loadapply_loadcb, cbdata);
  if (!cb)
  { 
    i_printf (1, "i_trigger_value_loadapply failed to call i_trigger_value_load for trigger %s", trg->name_str);
    i_trigger_value_loadapply_cbdata_free (cbdata);
    return NULL; 
  }

  return cb;
}

int i_trigger_value_loadapply_loadcb (i_resource *self, i_trigger *trg, i_value *val, i_value *yval, void *passdata)
{
  /* Called when the load operation for a 
   * trigger value has completed. If a value
   * is found, apply it. Otherwise, apply the 
   * default value stored in the data ptr 
   * of the callback struct.
   */

  int num;
  i_trigger_value_loadapply_cbdata *cbdata = passdata;

  /*
   * Check if values were found 
   */

  if (!val)
  {
    /* No Specific value found, use default */
    if (cbdata->def_val)
    { val = cbdata->def_val; }
  }

  if (!yval)
  {
    /* No Specific value found, use default */
    if (cbdata->def_yval)
    { yval = cbdata->def_yval; }
  }

  /* 
   * If a values are present, duplicate and apply them
   */

  if (val || yval)
  {
    i_value *dupval = NULL;
    i_value *dupyval = NULL;

    if (val) dupval = i_value_duplicate (val);
    if (yval) dupyval = i_value_duplicate (yval);
    
    num = i_trigger_value_apply (self, trg, dupval, dupyval);
    if (num != 0)
    { 
      i_printf (1, "i_trigger_value_loadapply_loadcb failed to apply value to trigger %s", trg->name_str);
      if (dupval) i_value_free (dupval);
      if (dupyval) i_value_free (dupyval);
    }
  }
  
  /* Free the cbdata */
  i_trigger_value_loadapply_cbdata_free (cbdata);

  return 0;
}

/* @} */
