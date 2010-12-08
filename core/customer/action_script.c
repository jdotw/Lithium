#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>

#include "action.h"

/* Script Struct Manipulation */

l_action_script* l_action_script_create ()
{
  l_action_script *script;
  script = (l_action_script *) malloc (sizeof(l_action_script));
  memset (script, 0, sizeof(l_action_script));
  script->configvar_list = i_list_create ();
  i_list_set_destructor (script->configvar_list, l_action_configvar_free);
  return script;
}

void l_action_script_free (void *scriptptr)
{
  l_action_script *script = scriptptr;
  if (script->name_str) free (script->name_str);
  if (script->desc_str) free (script->desc_str);
  if (script->info_str) free (script->info_str);
  if (script->version_str) free (script->version_str);
  i_list_free (script->configvar_list);
  free (script);
}


