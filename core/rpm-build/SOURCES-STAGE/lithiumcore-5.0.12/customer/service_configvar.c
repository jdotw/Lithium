#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "../device/service.h"

/* Struct manipulation */

l_service_configvar* l_service_configvar_create ()
{
  l_service_configvar *var;
  var = malloc (sizeof(l_service_configvar));
  memset (var, 0, sizeof(l_service_configvar));
  return var;
}

void l_service_configvar_free (void *varptr)
{
  l_service_configvar *var = varptr;
  if (var->name_str) free (var->name_str);
  if (var->desc_str) free (var->desc_str);
  if (var->value_str) free (var->value_str);
  free (var);
}

