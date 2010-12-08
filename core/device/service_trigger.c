#include <stdlib.h> 
#include <string.h> 
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/cement.h>
#include <induction/container.h>
#include <induction/metric.h>
#include <induction/object.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/str.h>
#include <induction/xml.h>

#include "service.h"

/*
 * Service Trigger Functions
 */

int l_service_trigger_fromxml (i_resource *self, i_triggerset *tset, i_xml *xml, xmlNodePtr trg_node)
{
  xmlNodePtr node;
  char *name_str = NULL;
  char *desc_str = NULL;
  char *valtype_str = NULL;
  char *trgtype_str = NULL;
  char *xval_str = NULL;
  char *xvalnum_str = NULL;
  char *yval_str = NULL;
  char *yvalnum_str = NULL;
  char *condition_str = NULL;

  /* Parse tree */
  for (node = trg_node->children; node; node = node->next)
  {
    char *str;
    str = (char *) xmlNodeListGetString (xml->doc, node->xmlChildrenNode, 1);
    if (!str) continue;

    /* Pre-creation variables */
    if (!strcmp((char *)node->name, "name") && str) name_str = strdup (str);
    else if (!strcmp((char *)node->name, "desc") && str) desc_str = strdup (str);
    else if (!strcmp((char *)node->name, "valtype") && str) valtype_str = strdup (str);
    else if (!strcmp((char *)node->name, "trgtype") && str) trgtype_str = strdup (str);
    else if (!strcmp((char *)node->name, "xval_string") && str) xval_str = strdup (str);
    else if (!strcmp((char *)node->name, "xval_num") && str) xvalnum_str = strdup (str);
    else if (!strcmp((char *)node->name, "yval_string") && str) yval_str = strdup (str);
    else if (!strcmp((char *)node->name, "yval_num") && str) yvalnum_str = strdup (str);
    else if (!strcmp((char *)node->name, "condition") && str) condition_str = strdup (str);

    xmlFree (str);
  }
  
  if (name_str && desc_str && valtype_str && trgtype_str && condition_str)
  {
    if (!xvalnum_str) xvalnum_str = strdup ("0");
    if (!yvalnum_str) yvalnum_str = strdup ("0");

    /* Create trigger */
    i_triggerset_addtrg (self, tset, name_str, desc_str, atoi (valtype_str), atoi(trgtype_str), atof(xvalnum_str), xval_str, atof(yvalnum_str), yval_str, 0, atoi (condition_str), TSET_FLAG_VALAPPLY);
  }
  
  if (name_str) free (name_str);
  if (desc_str) free (desc_str);
  if (valtype_str) free (valtype_str);
  if (trgtype_str) free (trgtype_str);
  if (xval_str) free (xval_str);
  if (yval_str) free (yval_str);
  if (xvalnum_str) free (xvalnum_str);
  if (yvalnum_str) free (yvalnum_str);
  if (condition_str) free (condition_str);

  return 0;
} 

