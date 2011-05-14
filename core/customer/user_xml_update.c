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
#include <induction/user.h>
#include <induction/auth.h>

#include "user.h"

/* Add/Edit a User */

int xml_user_update (i_resource *self, i_xml_request *req)
{
  int num;
  int perform_delete = 0;
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;
  i_user *user = NULL;

  if (!req->xml_in) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  char *username_str = NULL;
  char *password_str = NULL;
  char *fullname_str = NULL;
  int auth_level = 0;

  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str;

    str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);

    if (!strcmp((char *)node->name, "username") && str)
    { username_str = strdup (str); }
    else if (!strcmp((char *)node->name, "password") && str)
    { password_str = strdup (str); }
    else if (!strcmp((char *)node->name, "fullname") && str)
    { fullname_str = strdup (str); }
    else if (!strcmp((char *)node->name, "level_num") && str)
    { auth_level = atoi (str); }

    xmlFree (str);
  }

  /* Validate */
  if (!username_str) return -1;

  /* Get existing */
  int free_user = 0;
  user = i_user_sql_get (self, username_str);
  if (!user)
  {
    user = i_user_create ();
    user->auth->username = strdup (username_str);
    user->auth->customer_id = strdup (self->hierarchy->cust_name);
    free_user = 1;  // Only free it if we created it
  }
  else
  { 
    perform_delete = 1; 
  }

  /* Update User */
  user->auth->level = auth_level;
  if (password_str && strcmp(password_str, "********") != 0) 
  { if (user->auth->password) free (user->auth->password); user->auth->password = strdup (password_str); }
  if (fullname_str) 
  { if (user->fullname) free (user->fullname); user->fullname = strdup (fullname_str); }

  /* Remove old user */
  if (perform_delete)
  { i_user_sql_delete (self, user->auth->username); }

  /* Add User */
  num = i_user_sql_insert (self, user);
  if (num != 0)
  { i_printf (1, "xml_user_add failed to add new user to db"); }
  
  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "user_update");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Clean up */
  if (user && free_user == 1) i_user_free (user);
  if (username_str) free (username_str);
  if (password_str) free (password_str);
  if (fullname_str) free (fullname_str);

  return 1; 
}

int xml_user_add (i_resource *self, i_xml_request *req)
{
  return xml_user_update (self, req);
}


