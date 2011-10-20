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
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/hierarchy.h>
#include <induction/callback.h>
#include <induction/user.h>
#include <induction/auth.h>
#include <induction/xml.h>

#include "lic.h"

/* Initial setup of Lithium Core */

int xml_coresetup (i_resource *self, i_xml_request *req)
{
  xmlNodePtr node;
  xmlNodePtr root_node = NULL;

  if (!req->xml_in) return -1;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  char *key_str = NULL;
  char *username_str = NULL;
  char *password_str = NULL;
  root_node = xmlDocGetRootElement (req->xml_in->doc);
  for (node = root_node->children; node; node = node->next)
  {
    char *str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);

    if (!strcmp((char *)node->name, "key") && str) key_str = strdup (str);
    else if (!strcmp((char *)node->name, "auth_username") && str) username_str = strdup (str);
    else if (!strcmp((char *)node->name, "auth_password") && str) password_str = strdup (str);

    xmlFree (str);
  }

  /* Create return xml */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode (NULL, BAD_CAST "result");
  xmlDocSetRootElement (req->xml_out->doc, root_node);

  /* Add the license key */
  l_lic_key *key = l_lic_validate_key (self, key_str);
  if (key && key->status == KEY_VALID)
  {
    /* Valid key */
    xmlNewChild (root_node, NULL, BAD_CAST "license_result", BAD_CAST "1");
    xmlNewChild (root_node, NULL, BAD_CAST "license_message", BAD_CAST "License key validated and added.");
    l_lic_key_add (self, key_str);
  }
  else
  {
    /* Invalid key */
    i_printf (1, "xml_coresetup license key is invalid");
    xmlNewChild (root_node, NULL, BAD_CAST "license_result", BAD_CAST "0");
    xmlNewChild (root_node, NULL, BAD_CAST "license_message", BAD_CAST "Invalid License Key.");
  }

  /* Add the user if specified */
  if (username_str && password_str)
  {
    i_user *user = i_user_create();
    user->auth->username = strdup (username_str);
    user->auth->password = strdup (password_str);
    user->auth->level = AUTH_LEVEL_ADMIN;
    int num = i_user_sql_insert(self, user);
    if (num == 0)
    {
      xmlNewChild (root_node, NULL, BAD_CAST "auth_result", BAD_CAST "1");
      xmlNewChild (root_node, NULL, BAD_CAST "auth_message", BAD_CAST "License key validated and added.");
    }
    else
    { 
      i_printf(1, "xml_coresetup failed to add user"); 
      xmlNewChild (root_node, NULL, BAD_CAST "auth_result", BAD_CAST "0");
      xmlNewChild (root_node, NULL, BAD_CAST "auth_message", BAD_CAST "User account ");
    }
  }
  
  /* Mark the customer as configured */
  PGconn *pgconn = i_pg_connect (self, "lithium");
  char *query;
  asprintf(&query, "UPDATE customers SET configured = true WHERE name = '%s'", self->hierarchy->cust->name_str);
  PGresult *result = PQexec (pgconn, query);
  if (!result || PQresultStatus(result) != PGRES_TUPLES_OK || (PQntuples(result)) < 1)
  { i_printf (1, "xml_coresetup failed to set customer status to configured=true"); }
  PQclear(result);
  i_pg_close(pgconn);

  /* Update customer */
  self->hierarchy->cust->configured = 1;
  self->hierarchy->cust->version = time(NULL);
  
  return 1; 
}


