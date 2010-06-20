#include <stdlib.h>

#include "induction.h"
#include "postgresql.h"
#include "loop.h"
#include "callback.h"

static int static_enabled = 0;
static i_callback *static_fdset_preprocessor = NULL;
static i_callback *static_fdset_postprocessor = NULL;

/* ###################################
 * Enable / Disable Functions
 */

int i_pg_async_enable (i_resource *self)
{
  int num;
  
  if (static_enabled == 1)
  { i_printf (1, "i_pg_async_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  num = i_pg_async_conn_enable (self);
  if (num != 0)
  { i_printf (1, "i_pg_async_enable failed to enable pg_async_conn sub-system"); i_pg_async_disable (self); return -1; }

  static_fdset_preprocessor = i_loop_fdset_preprocessor_add (self, i_pg_async_fdset_preprocessor, NULL);
  if (!static_fdset_preprocessor)
  { i_printf (1, "i_pg_async_enable failed to add fdset preprocessor"); i_pg_async_disable (self); return -1; }

  static_fdset_postprocessor = i_loop_fdset_postprocessor_add (self, i_pg_async_fdset_postprocessor, NULL);
  if (!static_fdset_postprocessor)
  { i_printf (1, "i_pg_async_enable failed to add fdset postprocessor"); i_pg_async_disable (self); return -1; }

  return 0;
}

int i_pg_async_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "i_pg_disable warning, sub-system already disabled"); return 0; }
  
  static_enabled = 0;

  i_pg_async_conn_disable (self);

  if (static_fdset_preprocessor) 
  { i_loop_fdset_preprocessor_remove (self, static_fdset_preprocessor); static_fdset_preprocessor = NULL; }
  if (static_fdset_postprocessor)
  { i_loop_fdset_postprocessor_remove (self, static_fdset_postprocessor); static_fdset_postprocessor = NULL; }

  return 0;
}


