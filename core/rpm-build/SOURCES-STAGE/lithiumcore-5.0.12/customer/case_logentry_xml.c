#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/entity.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/xml.h>

#include "case.h"
#include "case_xml.h"

/*
 * Case Log Entry XML functions
 */

xmlNodePtr l_case_logentry_xml (l_case_logentry *log)
{
  char *str;
  xmlNodePtr log_node;

  /* Create case node */
  log_node = xmlNewNode (NULL, BAD_CAST "log_entry");

  /* Entry info */
  asprintf (&str, "%li", log->id);
  xmlNewChild (log_node, NULL, BAD_CAST "entryid", BAD_CAST str);
  free (str);
  asprintf (&str, "%li", log->caseid);
  xmlNewChild (log_node, NULL, BAD_CAST "caseid", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", log->type);
  xmlNewChild (log_node, NULL, BAD_CAST "type_num", BAD_CAST str);
  free (str);
  xmlNewChild (log_node, NULL, BAD_CAST "author", BAD_CAST log->author_str);

  /* Time stamp */
  asprintf (&str, "%li", log->tstamp.tv_sec);
  xmlNewChild (log_node, NULL, BAD_CAST "tstamp_sec", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", (int)log->tstamp.tv_usec);
  xmlNewChild (log_node, NULL, BAD_CAST "tstamp_usec", BAD_CAST str);
  free (str);

  /* Entry text */
  xmlNewChild (log_node, NULL, BAD_CAST "entry", BAD_CAST log->entry_str);

  return log_node;
}

