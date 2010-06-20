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
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>
#include <induction/auth.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>

#include "device.h"
#include "navtree.h"
#include "site.h"

int xml_site_list (i_resource *self, i_xml_request *req)
{
  i_xml *xml;
  i_site *site;
  xmlNodePtr root_node = NULL;

  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Create XML */
  xml = i_xml_create ();
  if (!xml) 
  { i_printf (1, "xml_site_list failed to create xml struct"); return -1; }

  /* Create/setup doc */
  xml->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "site_list");
  xmlDocSetRootElement(xml->doc, root_node);

  /* Loops through site list */
  for (i_list_move_head(self->hierarchy->cust->site_list); (site=i_list_restore(self->hierarchy->cust->site_list)) != NULL; i_list_move_next(self->hierarchy->cust->site_list))
  {
    char *str;
    xmlNodePtr site_node = NULL;

    site_node = xmlNewNode (NULL, BAD_CAST "site");
    xmlAddChild (root_node, site_node);

    xmlNewChild (site_node, NULL, BAD_CAST "name", BAD_CAST site->name_str);
    xmlNewChild (site_node, NULL, BAD_CAST "desc", BAD_CAST site->desc_str);
    xmlNewChild (site_node, NULL, BAD_CAST "opstate", BAD_CAST i_entity_opstatestr (site->opstate));
    asprintf (&str, "%u", site->opstate);
    xmlNewChild (site_node, NULL, BAD_CAST "opstate_num", BAD_CAST str);
    free (str);
  }
  
  /* Finished */
  req->xml_out = xml;
  
  return 1;
}

int xml_site_update (i_resource *self, i_xml_request *req)
{
  char *name_str = NULL;
  char *desc_str = NULL;
  char *addr1_str = NULL;
  char *addr2_str = NULL;
  char *addr3_str = NULL;
  char *suburb_str = NULL;
  char *state_str = NULL;
  char *postcode_str = NULL;
  char *country_str = NULL;
  char *longitude_str = NULL;
  char *latitude_str = NULL;
  i_site *site;
  xmlNodePtr ent_node;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "name") && str) name_str = strdup(str);
      else if (!strcmp((char *)node->name, "desc") && str) desc_str = strdup(str);
      else if (!strcmp((char *)node->name, "addr1") && str) addr1_str = strdup(str);
      else if (!strcmp((char *)node->name, "addr2") && str) addr2_str = strdup(str);
      else if (!strcmp((char *)node->name, "addr3") && str) addr3_str = strdup(str);
      else if (!strcmp((char *)node->name, "suburb") && str) suburb_str = strdup(str);
      else if (!strcmp((char *)node->name, "state") && str) state_str = strdup(str);
      else if (!strcmp((char *)node->name, "postcode") && str) postcode_str = strdup(str);
      else if (!strcmp((char *)node->name, "country") && str) country_str = strdup(str);
      else if (!strcmp((char *)node->name, "longitude") && str) longitude_str = strdup(str);
      else if (!strcmp((char *)node->name, "latitude") && str) latitude_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_site_update, no xml data received");
    return -1; 
  }

  /* Try and retrieve the site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), name_str);
  if (site)
  {
    /* Update existing */
    if (desc_str) { free (site->desc_str); site->desc_str = strdup (desc_str); }
    if (addr1_str) { free (site->addr1_str); site->addr1_str = strdup (addr1_str); }
    if (addr2_str) { free (site->addr2_str); site->addr2_str = strdup (addr2_str); }
    if (addr3_str) { free (site->addr3_str); site->addr3_str = strdup (addr3_str); }
    if (suburb_str) { free (site->suburb_str); site->suburb_str = strdup (suburb_str); }
    if (state_str) { free (site->state_str); site->state_str = strdup (state_str); }
    if (postcode_str) { free (site->postcode_str); site->postcode_str = strdup (postcode_str); }
    if (country_str) { free (site->country_str); site->country_str = strdup (country_str); }
    if (longitude_str) { site->longitude = atof (longitude_str); }
    if (latitude_str) { site->latitude = atof (latitude_str); }
    l_site_update (self, site);
  }
  else
  {
    /* Add new */
    double longitude;
    double latitude;
    if (longitude_str) longitude = atof (longitude_str);
    else longitude = 0.0;
    if (latitude_str) latitude = atof (latitude_str);
    else latitude = 0.0;
    site = l_site_add (self, name_str, desc_str, addr1_str, addr2_str, addr3_str, suburb_str, state_str, postcode_str, country_str, longitude, latitude);
    if (!site)
    { i_printf (1, "xml_site_update failed to call l_site_add to add new site"); return -1; }
  }

  /* Clean up */
  if (name_str) free (name_str);
  if (desc_str) free (desc_str);
  if (addr1_str) free (addr1_str);
  if (addr2_str) free (addr2_str);
  if (addr3_str) free (addr3_str);
  if (suburb_str) free (suburb_str);
  if (state_str) free (state_str);
  if (postcode_str) free (postcode_str);
  if (country_str) free (country_str);
  if (longitude_str) free (longitude_str);
  if (latitude_str) free (latitude_str);

  /* Create return site XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  ent_node = i_entity_xml (ENTITY(site), 0, 0);
  xmlDocSetRootElement (req->xml_out->doc, ent_node);
  
  return 1;
}

int xml_site_remove (i_resource *self, i_xml_request *req)
{
  char *name_str = NULL;
  i_site *site;
  xmlNodePtr ent_node;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "name") && str) name_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_site_remove, no xml data received");
    return -1; 
  }
  if (!name_str)
  { i_printf (1, "xml_site_remove failed, no site specified"); return -1; }

  /* Try and retrieve the site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), name_str);
  free (name_str);
  if (site)
  {
    /* Remove */
    l_site_remove (self, site);
  }
  else
  {
    i_printf (1, "xml_site_remove failed, site not found"); 
    return -1;
  }

  /* Create return cust XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  ent_node = i_entity_xml (ENTITY(self->hierarchy->cust), 0, 0);
  xmlDocSetRootElement (req->xml_out->doc, ent_node);

  return 1;
}

