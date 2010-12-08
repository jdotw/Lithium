#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/auth.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/xml.h>
#include <induction/triggerset_xml.h>

#include "triggerset.h"

int xml_triggerset_apprule_update (i_resource *self, i_xml_request *xmlreq)
{
  char *tset_name = NULL;
  char *met_name = NULL;
  int exclusive = 0;
  i_object *obj;
  i_triggerset *tset;
  i_triggerset_apprule *rule;
  xmlNodePtr root_node = NULL;
  xmlNodePtr rule_node = NULL;
  xmlNodePtr node = NULL;
  i_hashtable_key *key;

  /* Check permission */
  if (xmlreq->auth->level < AUTH_LEVEL_ADMIN)
  { xmlreq->xml_out = i_xml_denied (); return 1; }

  /* Find entity */
  obj = (i_object *) i_entity_local_get (self, xmlreq->entaddr);
  if (!obj || obj->ent_type != ENT_OBJECT) return -1;

  /* Create rule */
  rule = i_triggerset_apprule_create ();

  /* Get ref_sec, period and list of entity descriptor from XML */
  if (xmlreq->xml_in)
  {
    /* Iterate through XML */
    root_node = xmlDocGetRootElement (xmlreq->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, node->xmlChildrenNode, 1);
      if (str && !strcmp((char *)node->name, "tset_name")) tset_name = strdup (str);
      if (str && !strcmp((char *)node->name, "met_name")) met_name = strdup (str);
      if (str && !strcmp((char *)node->name, "id")) rule->id = atol(str);
      if (str && !strcmp((char *)node->name, "site_name")) rule->site_name = strdup (str);
      if (str && !strcmp((char *)node->name, "site_desc")) rule->site_desc = strdup (str);
      if (str && !strcmp((char *)node->name, "dev_name")) rule->dev_name = strdup (str);
      if (str && !strcmp((char *)node->name, "dev_desc")) rule->dev_desc = strdup (str);
      if (str && !strcmp((char *)node->name, "obj_name")) rule->obj_name = strdup (str);
      if (str && !strcmp((char *)node->name, "obj_desc")) rule->obj_desc = strdup (str);
      if (str && !strcmp((char *)node->name, "applyflag")) rule->applyflag = atoi (str);
      if (str && !strcmp((char *)node->name, "exclusive")) exclusive = atoi (str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_triggerset_apprule_update error, no xml data received");
    return -1;
  }
  if (!tset_name || !met_name)
  { i_printf (1, "xml_triggerset_apprule_update no tset_name and/or met_name specified"); return -1; }

  /* Find Tset */
  i_hashtable *tset_ht;
  if (obj->tset_ht) tset_ht = obj->tset_ht;
  else tset_ht = obj->cnt->tset_ht;
  key = i_hashtable_create_key_string (met_name, tset_ht->size);
  free (tset_name);
  free (met_name);
  tset = i_hashtable_get (tset_ht, key);
  i_hashtable_key_free (key);
  if (!tset)
  { i_printf (1, "xml_triggerset_apprule_update specified tset not found"); return -1; }

  /* Check for an ID */
  if (rule->id != 0)
  {
    /* Update existing rule */
    i_triggerset_apprule_update (self, obj, tset, rule);
  }
  else
  {
    /* Add new rule */
    if (exclusive == 0)
    { i_triggerset_apprule_add (self, obj, tset, rule); }
    else
    { i_triggerset_apprule_add_exclusive (self, obj, tset, rule); }
  } 

  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "triggerset_apprule_update");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);
  rule_node = i_triggerset_apprule_xml (rule);
  i_triggerset_apprule_free (rule);
  xmlAddChild (root_node, rule_node);
  
  return 1;
}

