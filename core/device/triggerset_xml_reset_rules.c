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
#include <induction/timer.h>
#include <induction/xml.h>
#include <induction/triggerset_xml.h>

#include "restart.h"
#include "triggerset.h"

int xml_triggerset_reset_rules (i_resource *self, i_xml_request *xmlreq)
{
  /*
   * Takes a list of entities and resets all
   * triggerset value and application rules 
   * that are specific to that entity and below. 
   *
   * For example, if a device is specified then all
   * rules that have site=site_name device=dev_name
   * will be removed. 
   */
  
  xmlNodePtr root_node = NULL;
  xmlNodePtr node = NULL;

  /* Check permission */
  if (xmlreq->auth->level < AUTH_LEVEL_ADMIN)
  { xmlreq->xml_out = i_xml_denied (); return 1; }

  /* list of entity descriptor from XML */
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
          i_entity *ent = (i_entity *) i_entity_local_get (self, (i_entity_address *)entdesc);
          if (ent)
          { 
            l_triggerset_xml_reset_rules_process (self, ent); 
          }
        }
      }
    }
  }

  /* Create return XML */
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "xml_triggerset_reset_rules");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);

  i_timer_add (self, 5, 0, l_restart_timercb, NULL);
  
  return 1;
}

int l_triggerset_xml_reset_rules_process (i_resource *self, i_entity *ent)
{
  /* Remove App Rules */
  i_triggerset_apprule_remove_exclusive (self, ent);

  /* Remove Value Rules */
  i_triggerset_valrule_remove_exclusive (self, ent);

  return 0;
}

