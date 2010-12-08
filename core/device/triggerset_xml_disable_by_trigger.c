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

int xml_triggerset_disable_by_trigger (i_resource *self, i_xml_request *xmlreq)
{
  /*
   * Takes a list of trigger entity descriptors and 
   * disables the trigger set that has resulted in 
   * that trigger becoming active
   */
  xmlNodePtr root_node = NULL;
  xmlNodePtr node = NULL;

  /* Get ref_sec, period and list of entity descriptor from XML */
  if (xmlreq->xml_in)
  {
    /* Iterate through XML */
    root_node = xmlDocGetRootElement (xmlreq->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      if (strcmp((char *)node->name, "entity_descriptor") == 0)
      {
        /* Process entity */
        i_entity_descriptor *entdesc = i_entity_descriptor_fromxml (xmlreq->xml_in, node);
        if (entdesc)
        {
          i_trigger *trg = (i_trigger *) i_entity_local_get (self, (i_entity_address *)entdesc);
          if (trg)
          { 
            l_triggerset_xml_disable_by_trigger_process (self, trg, entdesc); 
            i_entity_deregister (self, ENTITY(trg)); 
            i_entity_free (trg);
          }
        }
      }
    }
  }

  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "xml_triggerset_disable_by_trigger");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);

  return 1;
}

int l_triggerset_xml_disable_by_trigger_process (i_resource *self, i_trigger *trg, i_entity_descriptor *entdesc)
{
  /* Create rule */
  i_triggerset_apprule *rule = i_triggerset_apprule_create ();
  rule->site_name = strdup (entdesc->site_name);
  rule->site_desc = strdup (entdesc->site_desc);
  rule->dev_name = strdup (entdesc->dev_name);
  rule->dev_desc = strdup (entdesc->dev_desc);
  rule->obj_name = strdup (entdesc->obj_name);
  rule->obj_desc = strdup (entdesc->obj_desc);
  rule->applyflag = 0;

  /* Find Tset */
  i_hashtable *tset_ht;
  if (trg->met->obj->tset_ht) tset_ht = trg->met->obj->tset_ht;
  else tset_ht = trg->met->obj->cnt->tset_ht;
  i_hashtable_key *key = i_hashtable_create_key_string (entdesc->met_name, tset_ht->size);
  i_triggerset *tset = i_hashtable_get (tset_ht, key);
  i_hashtable_key_free (key);
  if (!tset)
  { i_printf (1, "l_triggerset_xml_disable_by_trigger_process specified tset not found"); return -1; }

  /* Add an exclusive rule */
  i_triggerset_apprule_add_exclusive (self, trg->met->obj, tset, rule);

  return 0;
}

