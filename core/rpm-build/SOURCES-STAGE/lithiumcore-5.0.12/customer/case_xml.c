#include <stdlib.h> 
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/xml.h>

#include "case.h"
#include "case_xml.h"

/*
 * Case XML functions
 */

xmlNodePtr l_case_xml (l_case *cas)
{
  char *str;
  xmlNodePtr case_node;

  /* Create case node */
  case_node = xmlNewNode (NULL, BAD_CAST "case");

  /* Case info */
  asprintf (&str, "%li", cas->id);
  xmlNewChild (case_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  xmlNewChild (case_node, NULL, BAD_CAST "state", BAD_CAST l_case_statestr (cas->state));
  asprintf (&str, "%i", cas->state);
  xmlNewChild (case_node, NULL, BAD_CAST "state_num", BAD_CAST str);
  free (str);
  xmlNewChild (case_node, NULL, BAD_CAST "hline", BAD_CAST cas->hline_str);
  xmlNewChild (case_node, NULL, BAD_CAST "owner", BAD_CAST cas->owner_str);
  xmlNewChild (case_node, NULL, BAD_CAST "requester", BAD_CAST cas->requester_str);

  /* Time stamps */
  asprintf (&str, "%li", cas->start.tv_sec);
  xmlNewChild (case_node, NULL, BAD_CAST "start_sec", BAD_CAST str);
  free (str);
  asprintf (&str, "%li", cas->end.tv_sec);
  xmlNewChild (case_node, NULL, BAD_CAST "end_sec", BAD_CAST str);
  free (str);

  /* Entities */
  i_entity_descriptor *ent;
  for (i_list_move_head(cas->ent_list); (ent=i_list_restore(cas->ent_list))!=NULL; i_list_move_next(cas->ent_list))
  {
    xmlNodePtr ent_node;
    ent_node = i_entity_descriptor_xml (ent);
    if (ent_node) xmlAddChild (case_node, ent_node);
  }

  /* Log entries */
  l_case_logentry *log;
  for (i_list_move_head(cas->log_list); (log=i_list_restore(cas->log_list))!=NULL; i_list_move_next(cas->log_list))
  {
    xmlNodePtr log_node;
    log_node = l_case_logentry_xml (log);
    if (log_node) xmlAddChild (case_node, log_node);
  }

  return case_node;
}

