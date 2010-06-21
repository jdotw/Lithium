#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "hierarchy.h"
#include "postgresql.h"
#include "cement.h"
#include "callback.h"
#include "sqltables.h"
#include "entity.h"

/** \addtogroup entity_refresh Entity Refresh
 * @ingroup entity
 * @{
 */

/* 
 * Entity refresh confiuration
 */

/* Struct manipulation */

i_entity_refresh_config* i_entity_refresh_config_create ()
{
  i_entity_refresh_config *config;

  config = (i_entity_refresh_config *) malloc (sizeof(i_entity_refresh_config));
  if (!config)
  { i_printf (1, "i_entity_refresh_config_create failed to malloc i_entity_refresh_config struct"); return NULL; }
  memset (config, 0, sizeof(i_entity_refresh_config));

  return config;
}

void i_entity_refresh_config_free (void *configptr)
{
  i_entity_refresh_config *config = configptr;

  if (!config) return;

  free (config);
}

/* Load and Apply a Configuration */

int i_entity_refresh_config_loadapply (i_resource *self, i_entity *ent, i_entity_refresh_config *defaults)
{
  /* Load the entities refresh config. If no
   * config is loaded, the supplied default will
   * be used or the refresh settings of the 
   * entities parent will be inherited
   *
   * The configuration will then be applied 
   */

  int num;
  int free_defaults = 0;

  /* FIX HACK Castnet */
  if (!defaults)
  { defaults = i_entity_refresh_config_running (self, ent->parent); free_defaults = 1; }
  if (!defaults) return 0;        
  i_entity_refresh_config_apply (self, ent, defaults);
  if (free_defaults == 1) i_entity_refresh_config_free (defaults); 
  return 0;
  /* END HACK */  

  num = i_entity_refresh_config_load (self, self->hierarchy, ent, i_entity_refresh_config_loadapply_loadcb, defaults);
  if (num != 0)
  { 
    /* Load failed, call the i_entity_refresh_config_loadapply_loadcb and 
     * have it attempt defaults/parent values
     */
    return i_entity_refresh_config_loadapply_loadcb (self, ent, NULL, defaults);
  }

  return 0;
}

int i_entity_refresh_config_loadapply_loadcb (i_resource *self, i_entity *ent, i_entity_refresh_config *config, void *passdata)
{
  /* Apply the supplied config */
  int num;
  i_entity_refresh_config *defaults = passdata;
  
  /* Attempt to use supplied config */
  if (config)
  {
    num = i_entity_refresh_config_apply (self, ent, config);
    if (num == 0)
    { 
      /* Successfully applied loaded config */
      return 0;
    }
  }

  /* Attempt to use defaults */
  if (defaults)
  {
    /* Apply defaults */
    num = i_entity_refresh_config_apply (self, ent, defaults);
    if (num == 0)
    {
      /* Successfully applied defaults */
      return 0;
    }
    else
    { 
      i_printf (1, "i_entity_refresh_config_loadapply_loadcb failed to apply default config for %s entity %s; trying parent values",
        i_entity_typestr (ent->ent_type), ent->name_str);
    }
  }

  /* Attempt to inherit parents config */
  if (ent->parent)
  {
    i_entity_refresh_config *pconfig;

    pconfig = i_entity_refresh_config_running (self, ent->parent);
    if (pconfig)
    {
      num = i_entity_refresh_config_apply (self, ent, pconfig);
      i_entity_refresh_config_free (pconfig);
      if (num != 0)
      {
        i_printf (1, "i_entity_refresh_config_loadapply_loadcb failed to apply parents refresh config for %s entity %s",
          i_entity_typestr (ent->ent_type), ent->name_str);
        return -1;
      }
      else
      { 
        /* Successfully applied parent values */
        return 0; 
      }
    }
    else
    {
      i_printf (1, "i_entity_refresh_config_loadapply_loadcb failed to retrieve current refresh config from parent for %s entity %s",
        i_entity_typestr (ent->ent_type), ent->name_str);
      return -1;
    }
  }

  /* All avenues exhausted */
  i_printf (1, "i_entity_refresh_config_loadapply_loadcb failed, no loaded config, default config or parent config applicable for %s entity %s",
    i_entity_typestr (ent->ent_type), ent->name_str);

  return -1;
}

/* Apply configuration */

int i_entity_refresh_config_apply (i_resource *self, i_entity *ent, i_entity_refresh_config *config)
{
  /* Take the specfied configuration and apply it to 
   * the refresh operation of the specified entity
   */

  /* Apply configuration values */
  ent->refresh_method = config->refresh_method;
  ent->refresh_int_sec = config->refresh_int_sec;
  ent->refresh_maxcolls = config->refresh_maxcolls;

  /* If the entity's refresh method is currently 
   * REFMETHOD_PARENT, and the new config is NOT
   * REFMETHOD_PARENT, AND the parents refresh func
   * is i_entity_refresh_children then i_entity_refresh_children_removechild
   * should be called just incase the parent has this
   * entity queued up in a refresh op
   */
  if ((ent->refresh_method & REFMETHOD_ALL) == REFMETHOD_PARENT && (config->refresh_method & REFMETHOD_ALL) != REFMETHOD_PARENT)
  {
    if (ent->parent && ent->parent->refresh_func == i_entity_refresh_children)
    { 
      i_entity_refresh_children_removechild (ent->parent, ent); 
    }
  }

  /* Action the config */
  switch (config->refresh_method & REFMETHOD_ALL)
  {
    case REFMETHOD_TIMER:          /* Entity is to have a timer refer timer installed */
      if (ent->refresh_int_sec > 0)
      {
        /* refresh_int_sec > 0 == timer enabled */
        if (!ent->refresh_timer)
        {
          /* Create new timer */
          ent->refresh_timer = i_timer_add (self, config->refresh_int_sec, 0, i_entity_refresh_timercb, ent);
          if (!ent->refresh_timer)
          {
            i_printf (1, "i_entity_refresh_config_apply failed to add refresh timer for %s entity %s",
              i_entity_typestr (ent->ent_type), ent->name_str); 
            return -1; 
          }
        }
        else
        {
          /* Adjust existing timer */
          i_timer_set_timeout (ent->refresh_timer, config->refresh_int_sec, 0);
        }
      }
      else
      { 
        /* refresh_int_sec < 1 == Timer Disabled */
        if (ent->refresh_timer) 
        { i_timer_remove (ent->refresh_timer); ent->refresh_timer = NULL; }
      }
      ent->refresh_method = REFMETHOD_TIMER;
      ent->refresh_int_sec = config->refresh_int_sec;
      ent->refresh_maxcolls = config->refresh_maxcolls;
      ent->refresh_state = REFSTATE_IDLE;
      break;
    default:
      /* Default = REFMETHOD_NONE, REF_METHOD_PARENT, REF_METHOD_EXTERNAL and unsupported values */
      ent->refresh_method = config->refresh_method;
      ent->refresh_int_sec = 0;
      if (ent->refresh_state == REFSTATE_BUSY)
      { 
        i_entity_refresh_terminate (ent); 
        ent->refresh_state = REFSTATE_IDLE; 
      }
      if (ent->refresh_timer)
      { i_timer_remove (ent->refresh_timer); ent->refresh_timer = NULL; }
  }

  return 0;
}

/* Load config from SQL */

int i_entity_refresh_config_load (i_resource *self, i_hierarchy *hier, i_entity *ent, int (*cbfunc) (i_resource *self, i_entity *ent, i_entity_refresh_config *config, void *passdata), void *passdata)
{
  /* Attempt to load the entities refresh config. 
   * The SELECT statement will fetch the specific config if 
   * available, or will fetch the partents default, or the parents
   * parents default, etc etc. 
   */

  int num;
  char *esc_str;
  char *site_esc;
  char *device_esc;
  char *cnt_query;
  char *obj_query;
  char *met_query;
  char *query;
  i_entity *orig_ent;
  i_pg_async_conn *conn;
  i_callback *cb;

  /* Open SQL Connection */
  conn = i_pg_async_conn_open_customer (self);
  if (!conn)
  { 
    i_printf (1, "i_entity_refresh_config_load failed to connect to SQL database for %s entity %s",
      i_entity_typestr (ent->ent_type), ent->name_str); 
    return -1; 
  }

  /* Set strings to default */
  cnt_query = strdup ("");
  obj_query = strdup ("");
  met_query = strdup ("");

  /* Create sub-query strings 
   *
   * Note: this is a funny looking select, the first 'case' to 
   *       set the switch off is starting point, but no breaks are 
   *       used and the ent is moved to ent->parent at each statement. 
   *       This is to allow a 'walking' of the cement tree for the entity
   *       to find its parents as applicable.
   */
  orig_ent = ent;
  switch (ent->ent_type)
  {
    case ENT_METRIC:
      esc_str = i_postgres_escape (ent->name_str);
      free (met_query);
      asprintf (&met_query, " OR metric='%s'", esc_str);
      free (esc_str);
      ent = ent->parent;
    case ENT_OBJECT:
      esc_str = i_postgres_escape (ent->name_str);
      free (obj_query);
      asprintf (&obj_query, " OR object='%s'", esc_str);
      free (esc_str);
      ent = ent->parent;
    case ENT_CONTAINER:
      esc_str = i_postgres_escape (ent->name_str);
      free (cnt_query);
      asprintf (&cnt_query, " OR container='%s'", esc_str);
      free (esc_str);
      ent = ent->parent;
  }
  ent = orig_ent;
      
  /* Create query str */
  site_esc = i_postgres_escape (hier->site_id);
  device_esc = i_postgres_escape (hier->device_id);
  asprintf (&query, "SELECT refresh_method, refresh_int_sec, refresh_maxcolls FROM %s WHERE (site IS NULL OR site='%s') AND (device IS NULL OR device='%s') AND (container IS NULL%s) AND (object IS NULL%s) AND (metric IS NULL%s) ORDER BY metric, object, container, device, site LIMIT 1",
    SQLTABLE_ENTREF_CONFIG, site_esc, device_esc, cnt_query, obj_query, met_query);
  free (site_esc);
  free (device_esc);
  free (cnt_query);
  free (obj_query);
  free (met_query);

  /* Create callback */
  cb = i_callback_create ();
  if (!cb)
  { i_printf (1, "i_entity_refresh_config_load failed to create i_callback struct"); free (query); i_pg_async_conn_close (conn); return -1; }
  cb->func = cbfunc;
  cb->data = ent;
  cb->passdata = passdata;

  /* Execute Query */
  num = i_pg_async_query_exec (self, conn, query, 0, i_entity_refresh_config_load_sqlcb, cb);
  free (query);
  if (num != 0)
  {
    i_printf (1, "i_entity_refresh_config_load failed to exec SQL command");
    i_callback_free (cb);
    i_pg_async_conn_close (conn);
    return -1;
  }

  return 0;
}

int i_entity_refresh_config_load_sqlcb (i_resource *self, i_pg_async_conn *conn, int operation, PGresult *result, void *passdata)
{
  i_callback *cb = passdata;
  i_entity_refresh_config *config = NULL;

  /* Check/Interpret Result */
  if (result && PQntuples (result) > 0)
  {
    /* Data to interpret */
    int i;
    int field_count;

    /* Create config struct */
    config = i_entity_refresh_config_create ();

    /* Interpret SQL row */
    field_count = PQnfields (result);
    for (i=0; i < field_count; i++)
    {
      char *fname;
      char *fval;

      fname = PQfname (result, i);
      fval = PQgetvalue (result, 0, i);

      if (fname && !strcmp(fname, "refresh_method"))
      { if (fval) config->refresh_method = atoi (fval); }
      else if (fname && !strcmp(fname, "refresh_int_sec"))
      { if (fval) config->refresh_int_sec = atoi (fval); }
      else if (fname && !strcmp(fname, "refresh_maxcolls"))
      { if (fval) config->refresh_maxcolls = atoi (fval); }
    }
  }

  /* Run callback */
  if (cb && cb->func)
  { cb->func (self, (i_entity *) cb->data, config, cb->passdata); }
  if (cb)
  { i_callback_free (cb); }

  /* Close SQL Connection */
  i_pg_async_conn_close (conn);

  return 0;
}

/* Retrieve running refresh config from an entity */

i_entity_refresh_config* i_entity_refresh_config_running (i_resource *self, i_entity *ent)
{
  i_entity_refresh_config *config;

  config = i_entity_refresh_config_create ();
  if (!config)
  { i_printf (1, "i_entity_refresh_config_running failed to create i_entity_refresh_config struct"); return NULL; }

  config->refresh_method = ent->refresh_method;
  config->refresh_int_sec = ent->refresh_int_sec;
  config->refresh_maxcolls = ent->refresh_maxcolls;

  return config;
}


/* @} */
