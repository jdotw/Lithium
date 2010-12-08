#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/postgresql.h>
#include <induction/auth.h>
#include <induction/entity.h>
#include <induction/device.h>
#include <induction/entity_xml.h>
#include <induction/xml.h>

#include "case.h"
#include "case_xml.h"

int xml_case_open (i_resource *self, i_xml_request *req)
{
  int num;
  l_case *cas;
  i_list *ent_list;
  char *logentry_str = NULL;
  xmlNodePtr case_node;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_USER)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Create case */
    cas = l_case_create ();
    cas->state = CASE_STATE_OPEN;
    gettimeofday (&cas->start, NULL);
    ent_list = i_list_create ();
    i_list_set_destructor (ent_list, i_entity_descriptor_free);
    
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      if (!strcmp((char *)node->name, "entity_descriptor"))
      {
        /* Entity descriptor */
        i_entity_descriptor *ent;
        ent = i_entity_descriptor_fromxml (req->xml_in, node);
        if (ent) { i_list_enqueue (ent_list, ent); }
        continue;
      }
      
      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "hline") && str) cas->hline_str = strdup(str);
      else if (!strcmp((char *)node->name, "owner") && str) cas->owner_str= strdup(str);
      else if (!strcmp((char *)node->name, "requester") && str) cas->requester_str = strdup(str);
      else if (!strcmp((char *)node->name, "logentry") && str) logentry_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_case_open error, no xml data received");
    return -1; 
  }

  /* Open case */
  num = l_case_open (self, cas, ent_list);
  i_list_free (ent_list);
  if (num != 0) { l_case_free (cas); if (logentry_str) free (logentry_str); return -1; }

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
    num = l_case_logentry_sql_insert (self, cas->id, log);
    l_case_logentry_free (log);
  }

  /* Create return case XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  case_node = l_case_xml (cas);
  l_case_free (cas);
  xmlDocSetRootElement (req->xml_out->doc, case_node);
  
  return 1;
}

