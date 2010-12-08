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

int xml_triggerset_apprule_list (i_resource *self, i_xml_request *xmlreq)
{
  int num;
  i_object *obj;
  i_triggerset *tset;
  char *tset_name = NULL;
  char *met_name = NULL;
  xmlNodePtr root_node = NULL;
  xmlNodePtr node = NULL;
  i_hashtable_key *key;

  /* Find entity */
  obj = (i_object *) i_entity_local_get (self, xmlreq->entaddr);
  if (!obj || obj->ent_type != ENT_OBJECT) return -1;

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

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_triggerset_apprule_list error, no xml data received");
    return -1;
  }
  if (!tset_name || !met_name)
  { i_printf (1, "xml_triggerset_apprule_list no tset_name and/or met_name specified"); return -1; }

  /* Find Tset */
  i_hashtable *tset_ht;
  if (obj->tset_ht) tset_ht = obj->tset_ht;
  else tset_ht = obj->cnt->tset_ht;
  key = i_hashtable_create_key_string (met_name, tset_ht->size);
  free (met_name);
  free (tset_name);
  tset = i_hashtable_get (tset_ht, key);
  i_hashtable_key_free (key);
  if (!tset)
  { i_printf (1, "xml_triggerset_apprule_list specified tset not found"); return -1; }

  /* Load rules */
  num = i_triggerset_apprule_sql_load (self, tset, obj, l_triggerset_xml_apprule_list_rulecb, xmlreq);
  if (num != 0)
  { i_printf (1, "xml_triggerset_apprule_list failed to query sql"); return -1; }

  return 0;
}

int l_triggerset_xml_apprule_list_rulecb (i_resource *self, i_list *list, void *passdata)
{
  i_triggerset_apprule *rule;
  i_xml_request *xmlreq = passdata;
  xmlNodePtr root_node = NULL;

  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "triggerset_apprule_list");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);

  /* Loop through each rule */
  for (i_list_move_head(list); (rule=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr rule_node;

    /* Create rule node */
    rule_node = i_triggerset_apprule_xml (rule);

    /* Rule node */
    xmlAddChild (root_node, rule_node);
  }

  i_xml_deliver (self, xmlreq);
  
  return -1;    /* Dont keep list */
}

