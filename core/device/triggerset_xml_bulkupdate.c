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

/* The xml_triggerset_bulkupdate form performs update and delete operations
 * for apprules and valrules under a given triggerset
 *
 * It is primarily/initially used by the iOS client as a one-stop way to 
 * push triggerset changes up to LithiumCore
 */

int xml_triggerset_bulkupdate (i_resource *self, i_xml_request *xmlreq)
{
  /* Check permission */
  if (xmlreq->auth->level < AUTH_LEVEL_ADMIN)
  { xmlreq->xml_out = i_xml_denied (); return 1; }

  /* Find entity */
  i_object *obj = (i_object *) i_entity_local_get (self, xmlreq->entaddr);
  if (!obj || obj->ent_type != ENT_OBJECT) return -1;

  /* Parse the XML */
  i_triggerset *tset = NULL;
  i_metric *met = NULL;
  if (xmlreq->xml_in)
  {
    
    /* Iterate through XML */
    xmlNodePtr root_node = xmlDocGetRootElement (xmlreq->xml_in->doc);
    xmlNodePtr node = NULL;
    for (node = root_node->children; node; node = node->next)
    {
      /* Look for met_name and find triggerset and metric */
      if (strcmp((char *)node->name, "met_name")==0)
      {
        /* Get tset hashtable */
        i_hashtable *tset_ht;
        if (obj->tset_ht) tset_ht = obj->tset_ht;
        else tset_ht = obj->cnt->tset_ht;

        /* Get metric name from XML */
        char *met_name = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, node->xmlChildrenNode, 1);

        /* Get tset from hashtable */
        i_hashtable_key *key = i_hashtable_create_key_string (met_name, tset_ht->size);
        tset = i_hashtable_get (tset_ht, key);
        i_hashtable_key_free (key);

        /* Get metric from the object (entaddr) */
        met = (i_metric *) i_entity_child_get (ENTITY(obj), met_name);
        xmlFree (met_name);
        if (!tset || !met)
        { i_printf (1, "xml_triggerset_bulkupdate failed to find specified trigger set"); return -1; }
      }

      /* Look for updates */
      if (strcmp((char *)node->name, "update")==0)
      {
        /* The list of apprules and valrules to be updated */
        xmlNodePtr update_node = NULL;
        for (update_node=node->children; update_node; update_node=update_node->next)
        {
          if (strcmp((char *)update_node->name, "apprule")==0)
          {
            /* An apprule to be updated */
            i_triggerset_apprule *rule = i_triggerset_apprule_create ();
            xmlNodePtr rule_node;
            for (rule_node=update_node->children; rule_node; rule_node=rule_node->next)
            {
              char *str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, rule_node->xmlChildrenNode, 1);
              if (str && !strcmp((char *)node->name, "id")) rule->id = atol(str);
              if (str && !strcmp((char *)node->name, "site_name")) rule->site_name = strdup (str);
              if (str && !strcmp((char *)node->name, "site_desc")) rule->site_desc = strdup (str);
              if (str && !strcmp((char *)node->name, "dev_name")) rule->dev_name = strdup (str);
              if (str && !strcmp((char *)node->name, "dev_desc")) rule->dev_desc = strdup (str);
              if (str && !strcmp((char *)node->name, "obj_name")) rule->obj_name = strdup (str);
              if (str && !strcmp((char *)node->name, "obj_desc")) rule->obj_desc = strdup (str);
              if (str && !strcmp((char *)node->name, "applyflag")) rule->applyflag = atoi (str);
              xmlFree (str);
            }

            /* Add or update the rule */
            if (rule->id != 0) i_triggerset_apprule_update (self, obj, tset, rule);
            else i_triggerset_apprule_add (self, obj, tset, rule);
          }
          else if (strcmp((char *)update_node->name, "valrule")==0)
          {
            /* A valrule to be updated */
            i_triggerset_valrule *rule = i_triggerset_valrule_create ();
            xmlNodePtr rule_node;
            for (rule_node=update_node->children; rule_node; rule_node=rule_node->next)
            {
              char *str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, rule_node->xmlChildrenNode, 1);

              if (str && !strcmp((char *)node->name, "id")) rule->id = atol (str);
              if (str && !strcmp((char *)node->name, "site_name")) rule->site_name = strdup (str);
              if (str && !strcmp((char *)node->name, "site_desc")) rule->site_desc = strdup (str);
              if (str && !strcmp((char *)node->name, "dev_name")) rule->dev_name = strdup (str);
              if (str && !strcmp((char *)node->name, "dev_desc")) rule->dev_desc = strdup (str);
              if (str && !strcmp((char *)node->name, "obj_name")) rule->obj_name = strdup (str);
              if (str && !strcmp((char *)node->name, "obj_desc")) rule->obj_desc = strdup (str);
              if (str && !strcmp((char *)node->name, "trg_name")) rule->trg_name = strdup (str);
              if (str && !strcmp((char *)node->name, "trg_desc")) rule->trg_desc = strdup (str);
              if (str && !strcmp((char *)node->name, "xval")) rule->xval_str = strdup (str);
              if (str && !strcmp((char *)node->name, "yval")) rule->yval_str = strdup (str);
              if (str && !strcmp((char *)node->name, "duration")) rule->duration_sec = atol (str);
              if (str && !strcmp((char *)node->name, "trg_type_num")) rule->trg_type = atoi (str);
              if (str && !strcmp((char *)node->name, "adminstate_num")) rule->adminstate = atoi (str);

              xmlFree (str);
            }

            /* Add or update the rule */
            if (rule->id != 0) i_triggerset_valrule_update (self, obj, tset, rule);
            else i_triggerset_valrule_add (self, obj, tset, rule); 
          }
        }
      }

      /* Look for deletes */
      else if (strcmp((char *)node->name, "delete")==0)
      {
        /* The list of apprules and valrules to be delete */
        xmlNodePtr delete_node = NULL;
        for (delete_node=node->children; delete_node; delete_node=delete_node->next)
        {
          if (strcmp((char *)delete_node->name, "apprule")==0)
          {
            /* An apprule to be delete */
            unsigned long id = 0;
            xmlNodePtr rule_node;
            for (rule_node=delete_node->children; rule_node; rule_node=rule_node->next)
            {
              char *str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, rule_node->xmlChildrenNode, 1);
              if (str && !strcmp((char *)node->name, "id")) id = atol (str);
              xmlFree (str);
            }
            if (id != 0) i_triggerset_apprule_remove (self, obj, tset, id);
          }
          else if (strcmp((char *)delete_node->name, "valrule")==0)
          {
            /* A valrule to be deleted */
            unsigned long id = 0;
            xmlNodePtr rule_node;
            for (rule_node=delete_node->children; rule_node; rule_node=rule_node->next)
            {
              char *str = (char *) xmlNodeListGetString (xmlreq->xml_in->doc, rule_node->xmlChildrenNode, 1);
              if (str && !strcmp((char *)node->name, "id")) id = atol (str);
              xmlFree (str);
            }
            if (id != 0) i_triggerset_valrule_remove (self, obj, tset, id);
          }
        }
      }
    }
  }

  /* Update the metric */
  if (met) i_trigger_process_all (self, met);

  /* Create return XML (blank) */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "triggerset_bulkupdate");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);
  
  return 1;
}

