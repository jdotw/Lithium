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

int xml_case_logentry_insert (i_resource *self, i_xml_request *req)
{
  int num;
  char *str;
  long caseid = -1;
  l_case_logentry *log;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_USER)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Create log */
  log = l_case_logentry_create ();
  gettimeofday (&log->tstamp, NULL);
  log->author_str = strdup (req->auth->username);

  /* Interpret fields */
  if (req->xml_in)
  {
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "caseid") && str) caseid = atol (str);
      else if (!strcmp((char *)node->name, "type_num") && str) log->type = atoi (str);
      else if (!strcmp((char *)node->name, "timespent_sec") && str) log->timespent_sec = atol (str);
      else if (!strcmp((char *)node->name, "entry") && str) log->entry_str = strdup (str);
      xmlFree (str);
    }
  }
  if (caseid == -1) { l_case_logentry_free (log); return -1; }

  /* Insert entry */
  num = l_case_logentry_sql_insert (self, caseid, log);
  i_printf (1, "xml_case_logentry_insert l_case_logentry_sql_insert returned %i", num);
  if (num != 0) return -1;

  /* Prepare to call xml_case_logentry_list */
  if (req->xml_in) i_xml_free (req->xml_in);
  req->xml_in = i_xml_create ();
  req->xml_in->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "criteria");
  xmlDocSetRootElement (req->xml_in->doc, root_node);
  asprintf (&str, "%li", caseid);
  xmlNewChild (root_node, NULL, BAD_CAST "caseid", BAD_CAST str);
  free (str);

  return xml_case_logentry_list (self, req);
}

