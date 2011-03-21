#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/callback.h>
#include <induction/auth.h>
#include <induction/hierarchy.h>
#include <induction/name.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/interface.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>

#include "modb.h"

/* modb - Module Builder Sub-System */

l_modb_metric* l_modb_metric_create ()
{
  l_modb_metric *met;

  met = (l_modb_metric *) malloc (sizeof(l_modb_metric));
  memset (met, 0, sizeof(l_modb_metric));
  met->kbase = 1000;
  met->multiply_by = 1;
  met->enum_list = i_list_create ();
  i_list_set_destructor(met->enum_list, i_metric_enumstr_free);

  return met;
}

void l_modb_metric_free (void *metptr)
{
  l_modb_metric *met = metptr;

  if (met->name_str) free (met->name_str);
  if (met->desc_str) free (met->desc_str);
  if (met->oid_str) free (met->oid_str);
  if (met->xmet_desc) free (met->xmet_desc);
  if (met->ymet_desc) free (met->ymet_desc); 
  if (met->unit_str) free (met->unit_str);
  if (met->countermet_desc) free (met->countermet_desc); 
  if (met->gaugemet_desc) free (met->gaugemet_desc); 
  if (met->maxmet_desc) free (met->maxmet_desc);
  if (met->enum_list) i_list_free (met->enum_list);

  free (met);
}
