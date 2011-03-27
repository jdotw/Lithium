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
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/timer.h>
#include <induction/xml.h>
#include <induction/auth.h>
#include <induction/user.h>
#include <induction/configfile.h>
#include <induction/files.h>

#include "device.h"
#include "navtree.h"
#include "site.h"
#include "case.h"
#include "user.h"
#include "user_xml.h"

/* List all users */

int xml_user_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  i_user *user;
  i_list *user_list;
  xmlNodePtr root_node = NULL;
  xmlNodePtr user_node;
  char *str;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "user_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* User list */
  user_list = i_user_sql_list(self);
  for (i_list_move_head(user_list); (user=i_list_restore(user_list))!=NULL; i_list_move_next(user_list))
  {
    user_node = xmlNewNode (NULL, BAD_CAST "user");
    xmlNewChild (user_node, NULL, BAD_CAST "username", BAD_CAST user->auth->username);
    xmlNewChild (user_node, NULL, BAD_CAST "password", BAD_CAST "********");
    xmlNewChild (user_node, NULL, BAD_CAST "confirm_password", BAD_CAST "********");
    if (user->auth->level >= AUTH_LEVEL_ADMIN)
    { xmlNewChild (user_node, NULL, BAD_CAST "level", BAD_CAST "Administrator"); }
    else if (user->auth->level == AUTH_LEVEL_USER)
    { xmlNewChild (user_node, NULL, BAD_CAST "level", BAD_CAST "Normal User"); }
    else
    { xmlNewChild (user_node, NULL, BAD_CAST "level", BAD_CAST "Read-Only"); }
    asprintf (&str, "%i", user->auth->level);
    xmlNewChild (user_node, NULL, BAD_CAST "level_num", BAD_CAST str);
    free (str);
    xmlNewChild (user_node, NULL, BAD_CAST "fullname", BAD_CAST user->fullname);

    xmlAddChild (root_node, user_node);
  }
  i_list_free (user_list);
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}


