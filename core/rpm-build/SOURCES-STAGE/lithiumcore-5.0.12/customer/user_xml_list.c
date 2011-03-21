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
#include <induction/contact.h>
#include <induction/user.h>
#include <induction/userdb.h>
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
  char *admin_username;

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_incident_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "user_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Admin User */
  user_node = xmlNewNode (NULL, BAD_CAST "user");
  admin_username = i_configfile_get (self, NODECONF_FILE, "master_user", "username", 0);
  if (admin_username)
  {
    xmlNewChild (user_node, NULL, BAD_CAST "username", BAD_CAST admin_username);
    free (admin_username);
  }
  else
  { xmlNewChild (user_node, NULL, BAD_CAST "username", BAD_CAST "admin"); }
  xmlNewChild (user_node, NULL, BAD_CAST "level", BAD_CAST "Administrator");
  xmlNewChild (user_node, NULL, BAD_CAST "level_num", BAD_CAST "1024");
  xmlNewChild (user_node, NULL, BAD_CAST "fullname", BAD_CAST "Administrator");
  xmlNewChild (user_node, NULL, BAD_CAST "title", BAD_CAST "Global Master User");
  xmlNewChild (user_node, NULL, BAD_CAST "global_admin", BAD_CAST "1");
  xmlAddChild (root_node, user_node);

  /* User list */
  user_list = i_userdb_get_all (self);
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
    xmlNewChild (user_node, NULL, BAD_CAST "title", BAD_CAST user->title);
    if (user->contact)
    {
      i_contact_info *contact;

      xmlNewChild (user_node, NULL, BAD_CAST "email", BAD_CAST user->contact->email);
      asprintf (&str, "%i", user->contact->hours);
      xmlNewChild (user_node, NULL, BAD_CAST "hours", BAD_CAST str);
      free (str);

      if (user->contact->office)
      { l_user_xml_contact (user_node, user->contact->office, "office"); }
      if (user->contact->mobile)
      { l_user_xml_contact (user_node, user->contact->mobile, "mobile"); }
      if (user->contact->after_hours)
      { l_user_xml_contact (user_node, user->contact->after_hours, "after_hours"); }
      for (i_list_move_head(user->contact->others); (contact=i_list_restore(user->contact->others))!=NULL; i_list_move_next(user->contact->others))
      { l_user_xml_contact (user_node, contact, "other"); }
    }

    xmlAddChild (root_node, user_node);
  }
  i_list_free (user_list);
    
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

void l_user_xml_contact (xmlNodePtr user_node, i_contact_info *contact, char *name_str)
{
  xmlNodePtr contact_node;

  contact_node = xmlNewNode (NULL, BAD_CAST "contact");
  xmlNewChild (contact_node, NULL, BAD_CAST "name", BAD_CAST name_str);
  xmlNewChild (contact_node, NULL, BAD_CAST "phone", BAD_CAST contact->phone);
  xmlNewChild (contact_node, NULL, BAD_CAST "address_1", BAD_CAST contact->address_1);
  xmlNewChild (contact_node, NULL, BAD_CAST "address_2", BAD_CAST contact->address_2);
  xmlNewChild (contact_node, NULL, BAD_CAST "address_3", BAD_CAST contact->address_3);
  xmlNewChild (contact_node, NULL, BAD_CAST "notes", BAD_CAST contact->notes);

  xmlAddChild (user_node, contact_node);
}

