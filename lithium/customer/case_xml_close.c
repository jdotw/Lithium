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

int xml_case_close (i_resource *self, i_xml_request *req)
{
  int num;
  char *caseid_str = NULL;
  char *logentry_str = NULL;
  xmlNodePtr case_node = NULL;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_USER)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Get variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
      if (!strcmp((char *)node->name, "caseid") && str) caseid_str = strdup(str);
      else if (!strcmp((char *)node->name, "logentry") && str) logentry_str = strdup(str);
      xmlFree (str);
    }
  }
  else
  { i_printf (1, "xml_case_close error, no xml data received"); return -1; }

  if (!caseid_str)
  { i_printf (1, "xml_case_close error, no caseid specified"); if (logentry_str) free (logentry_str); return -1; }

  /* Close case */
  num = l_case_close (self, atol(caseid_str));
  if (num != 0) { if (logentry_str) free (logentry_str); return -1; }

  /* Log entry */
  if (logentry_str)
  {
    /* Create entry */
    l_case_logentry *log;
    log = l_case_logentry_create ();
    gettimeofday (&log->tstamp, NULL);
    log->author_str = strdup (req->auth->username);
    log->entry_str = strdup ((char *) logentry_str);
    free (logentry_str);

    /* Insert log entry */
    num = l_case_logentry_sql_insert (self, atol(caseid_str), log);
    l_case_logentry_free (log);
  }

  /* Create return case XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  case_node = xmlNewNode (NULL, BAD_CAST "case");
  xmlNewChild (case_node, NULL, BAD_CAST "id", BAD_CAST caseid_str);
  free (caseid_str);
  xmlDocSetRootElement (req->xml_out->doc, case_node);
  
  return 1;
}

