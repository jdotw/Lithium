#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/callback.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/incident.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/timer.h>
#include <induction/xml.h>

#include "case.h"
#include "incident.h"
#include "action.h"

/* Cancel pending actions */

int xml_action_cancelpending (i_resource *self, i_xml_request *req)
{
  unsigned long incid = 0;
  unsigned long actionid = 0;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Interpret XML */
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
    if (!strcmp((char *)node->name, "incid") && str)
    { incid = atol (str); }
    else if (!strcmp((char *)node->name, "actionid") && str)
    { actionid = atol (str); }

    if (str) xmlFree(str);
  }

  /* Perform Cancel */
  if (incid > 0)
  {
    /* Retrieve incident */
    i_hashtable *inc_table = l_incident_table ();
    i_hashtable_key *key = i_hashtable_create_key_long (incid, inc_table->size);
    i_incident *inc = i_hashtable_get (inc_table, key);
    i_hashtable_key_free (key);
    if (inc)
    {
      /* Find action */
      l_action *action;
      for (i_list_move_head(inc->action_list); (action=i_list_restore(inc->action_list))!=NULL; i_list_move_next(inc->action_list))
      {
        /* Cancel any pending activity */
        if (actionid == 0 || action->id == actionid)
        {
          action->runstate = 0;
          if (action->delay_timer)
          { i_timer_remove (action->delay_timer); action->delay_timer = NULL; }
          if (action->rerun_timer)
          { i_timer_remove (action->rerun_timer); action->rerun_timer = NULL; }
          action->activation = 0;
        }
      }
    }
  }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "action_cancelpending");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  return 1;
}

