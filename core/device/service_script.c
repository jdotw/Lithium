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

#include "service.h"

/* Script Struct Manipulation */

l_service_script* l_service_script_create ()
{
  l_service_script *script;

  /* Create script */
  script = (l_service_script *) malloc (sizeof(l_service_script));
  memset (script, 0, sizeof(l_service_script));

  /* Create configvar list */
  script->configvar_list = i_list_create ();
  i_list_set_destructor (script->configvar_list, l_service_configvar_free);

  /* 
   * Add default config variables 
   */
  l_service_configvar *var;

  /* Alternative IP */
  var = l_service_configvar_create ();
  var->name_str = strdup ("alt_ip");
  var->desc_str = strdup ("Alternate IP/Hostname:");
  i_list_enqueue (script->configvar_list, var);

  /* Alternative Port */
  var = l_service_configvar_create ();
  var->name_str = strdup ("alt_port");
  var->desc_str = strdup ("Alternate Port:");
  i_list_enqueue (script->configvar_list, var);
  
  return script;
}

void l_service_script_free (void *scriptptr)
{
  l_service_script *script = scriptptr;
  if (script->name_str) free (script->name_str);
  if (script->desc_str) free (script->desc_str);
  if (script->info_str) free (script->info_str);
  if (script->version_str) free (script->version_str);
  if (script->protocol_str) free (script->protocol_str);
  if (script->transport_str) free (script->transport_str);
  if (script->port_str) free (script->port_str);
  i_list_free (script->configvar_list);
  free (script);
}


