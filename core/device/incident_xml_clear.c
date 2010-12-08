#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/opstate.h>
#include <induction/entity_xml.h>
#include <induction/hierarchy.h>
#include <induction/customer.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/message.h>
#include <induction/incident.h>
#include <induction/xml.h>

int xml_incident_clear (i_resource *self, i_xml_request *xmlreq)
{
  /* Check permission */
  if (xmlreq->auth->level < AUTH_LEVEL_USER)
  { xmlreq->xml_out = i_xml_denied (); return 1; }

  /* Find entity */
  i_trigger *trg;
  trg = (i_trigger *) i_entity_local_get (self, xmlreq->entaddr);
  if (!trg || trg->ent_type != ENT_TRIGGER) return -1;
  i_metric *met = (i_metric *) trg->met;
  
  /* Create return XML */
  xmlNodePtr root_node = NULL;
  xmlreq->xml_out = i_xml_create ();
  xmlreq->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  root_node = xmlNewNode(NULL, BAD_CAST "cleared");
  xmlDocSetRootElement (xmlreq->xml_out->doc, root_node);

  /* Change opstate of trigger to normal value */
  i_opstate_change (self, ENTITY(trg), ENTSTATE_NORMAL);

  /* Clear Incident 
   *
   * NOTE: The fate of the incident struct is now handed over
   *       to i_incident_clear. The incident struct should not
   *       be freed here
   */

  if (met->op_inc)
  { 
    i_incident_clear (self, ENTITY(met), met->op_inc); 
    met->op_inc = NULL; 
  }
  
  return 1;
}

