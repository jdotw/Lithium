#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/xml.h>
#include <induction/callback.h>

#include "case.h"
#include "case_xml.h"

int xml_case_reopen (i_resource *self, i_xml_request *req)
{
  int num;
  char *id_str = NULL;
  char *logentry_str = NULL;
  i_callback *cb;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_USER)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Get variables */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);

      if (!strcmp((char *)node->name, "caseid") && str) id_str = strdup (str);
      else if (!strcmp((char *)node->name, "logentry") && str) logentry_str = strdup (str);

      /* Free string */
      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_case_reopen error, no xml data received");
    return -1; 
  }
  if (!id_str)
  { i_printf (1, "xml_case_reopen error, caseid not received in xml"); if (logentry_str) free (logentry_str); return -1; }

  /* Record Log Entry */
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
    num = l_case_logentry_sql_insert (self, atol(id_str), log);
    l_case_logentry_free (log);
  }

  /* Load specified case */
  cb = l_case_sql_list (self, id_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, l_case_xml_reopen_casecb, req);
  if (id_str) free (id_str);
  if (!cb) return -1;
  
  return 0;
}

int l_case_xml_reopen_casecb (i_resource *self, i_list *list, void *passdata)
{
  int num;
  l_case *cas;
  i_xml_request *req = passdata;

  /* Get case */
  i_list_move_head (list);
  cas = i_list_restore (list);

  /* Check if case was found */
  if (cas)
  {
    /* Re-Open case */
    num = l_case_reopen (self, cas, req->auth->username);
    if (num != 0)
    {
      return -1;
    }
  
    /* Create return XML */
    xmlNodePtr case_node;
    req->xml_out = i_xml_create ();
    req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
    case_node = l_case_xml (cas);
    xmlDocSetRootElement (req->xml_out->doc, case_node);
  }

  /* Deliver */
  i_xml_deliver (self, req);

  return -1;  /* Don't keep list */
}
