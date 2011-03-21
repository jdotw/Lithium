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

int xml_triggerset_apprule_delete (i_resource *self, i_xml_request *xmlreq)
{
  long id = -1;
  char *str;
  char *tset_name = NULL;
  char *met_name = NULL;
  i_object *obj;
  i_triggerset *tset;
  xmlNodePtr root_node = NULL;
  xmlNodePtr node = NULL;
  i_hashtable_key *key;

  /* Check permission */
  if (xmlreq->auth->level < AUTH_LEVEL_ADMIN)
  { xmlreq->xml_out = i_xml_denied (); return 1; }

  /* Find entity */
  obj = (i_object *) i_entity_local_get (self, xmlreq->entaddr);
  if (!obj || obj->ent_type != ENT_OBJECT) return -1;

  /* Get value from XML */
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
      if (str && !strcmp((char *)node->name, "id")) id = atol(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_triggerset_apprule_delete error, no xml data received");
    return -1;
  }
  if (!tset_name)
  { i_printf (1, "xml_triggerset_apprule_delete no tset_name specified"); return -1; }
  if (id == -1)
  { i_printf (1, "xml_triggerset_apprule_delete no rule ID specified"); return -1; }

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
  { i_printf (1, "xml_triggerset_apprule_delete specified tset not found"); return -1; }

  /* Delete */
  i_triggerset_apprule_remove (self, obj, tset, id);

  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "triggerset_apprule_delete");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);
  asprintf (&str, "%li", id);
  xmlNewChild (root_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  
  return 1;
}

