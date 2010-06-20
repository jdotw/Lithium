#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/postgresql.h>
#include <induction/list.h>

#include "infstat.h"

/* infstat - Infrastructure Status 
 *
 * User configurable and updated general purpose
 * infrastructure status tracking system
 */

static int static_enabled = 0;

/* Variable Retrieval */

int l_infstat_enabled ()
{ return static_enabled; }

/* Enable/Disable */

int l_infstat_enable (i_resource *self)
{
  int num;

  if (static_enabled == 1)
  { i_printf (1, "l_infstat_enable warning, sub-system already enabled"); return 0; }

  static_enabled = 1;

  /* Initialise SQL */
  num = l_infstat_cat_sql_init (self);
  if (num != 0)
  { i_printf (1, "l_infstat_enable failed to initialise infstat_cat_sql"); l_infstat_disable (self); return -1; }
  num = l_infstat_issue_sql_init (self);
  if (num != 0)
  { i_printf (1, "l_infstat_enable failed to initialise infstat_issue_sql"); l_infstat_disable (self); return -1; }

  /* Load infstat categories */
  num = l_infstat_cat_load (self);
  if (num != 0)
  { i_printf (1, "l_infstat_enable failed to call l_infstat_cat_load"); l_infstat_disable (self); return -1; }

  return 0;
}

int l_infstat_disable (i_resource *self)
{
  if (static_enabled == 0)
  { i_printf (1, "l_infstat_disable warning, sub-system already disabled"); return 0; }

  static_enabled = 0; 

  return 0;
}

