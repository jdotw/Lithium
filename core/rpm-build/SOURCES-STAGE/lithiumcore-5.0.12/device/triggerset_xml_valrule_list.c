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

int xml_triggerset_valrule_list (i_resource *self, i_xml_request *xmlreq)
{
  i_object *obj;
  i_trigger *trg;
  i_triggerset *tset;
  i_list *list;
  char *tset_name = NULL;
  char *met_name = NULL;
  char *trg_name = NULL;
  xmlNodePtr root_node = NULL;
  xmlNodePtr node = NULL;
  i_hashtable_key *key;
  i_triggerset_valrule *rule;

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
      if (str && !strcmp((char *)node->name, "trg_name")) trg_name = strdup (str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_triggerset_valrule_list error, no xml data received");
    return -1;
  }
  if (!tset_name || !met_name)
  { i_printf (1, "xml_triggerset_valrule_list no tset_name and/or met_name specified"); return -1; }
  if (!trg_name)
  { i_printf (1, "triggerset_xml_valrule_list no trg_name specified"); return -1; }

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
  { i_printf (1, "xml_triggerset_valrule_list failed to find specified trigger set"); return -1; }

  /* Find Tset Trigger */
  for (i_list_move_head(tset->trg_list); (trg=i_list_restore(tset->trg_list))!=NULL; i_list_move_next(tset->trg_list)) 
  {
    if (!strcmp(trg->name_str, trg_name)) break;
  }
  free (trg_name);
  if (!trg)
  { i_printf (1, "xml_triggerset_valrule_list specified trigger not found"); return -1; }

  /* Load rules */
  list = i_triggerset_valrule_sql_load_sync (self, tset, obj, trg);
  if (!list)
  { i_printf (1, "xml_triggerset_valrule_list failed to retrieve list of value rules"); return -1; }

  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "triggerset_valrule_list");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);

  /* Loop through each rule */
  for (i_list_move_head(list); (rule=i_list_restore(list))!=NULL; i_list_move_next(list))
  {
    xmlNodePtr rule_node;

    /* Create rule node */
    rule_node = i_triggerset_valrule_xml (rule);

    /* Rule node */
    xmlAddChild (root_node, rule_node);
  }
  
  return 1;
}

