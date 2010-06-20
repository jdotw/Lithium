#include <stdlib.h> 
#include <string.h> 
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "procpro.h"

/*
 * Action XML functions
 */

xmlNodePtr l_procpro_xml (l_procpro *procpro)
{
  char *str;
  xmlNodePtr procpro_node;

  /* Create procpro node */
  procpro_node = xmlNewNode (NULL, BAD_CAST "procpro");

  /* procpro info */
  asprintf (&str, "%li", procpro->id);
  xmlNewChild (procpro_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  if (procpro->desc_str) xmlNewChild (procpro_node, NULL, BAD_CAST "desc", BAD_CAST procpro->desc_str);
  if (procpro->match_str) xmlNewChild (procpro_node, NULL, BAD_CAST "match", BAD_CAST procpro->match_str);
  if (procpro->argmatch_str) xmlNewChild (procpro_node, NULL, BAD_CAST "argmatch", BAD_CAST procpro->argmatch_str);

  if (procpro->obj)
  {
    xmlNodePtr ent_node = i_entity_descriptor_xml (i_entity_descriptor_struct_static(ENTITY(procpro->obj)));
    if (ent_node) xmlAddChild (procpro_node, ent_node);
  }

  return procpro_node;
}

