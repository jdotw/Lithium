#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libxml/parser.h>

#include "induction.h"
#include "entity.h"
#include "entity_xml.h"
#include "message.h"
#include "socket.h"
#include "data.h"
#include "msgproc.h"
#include "auth.h"
#include "respond.h"
#include "incident.h"
#include "xml.h"

/** \addtogroup incident Incident Reporting
 * @ingroup backend
 * @{
 */

/*
 * Incident-related XML functions
 */

xmlNodePtr i_incident_xml (i_incident *inc)
{
  char *str;
  xmlNodePtr node;
  xmlNodePtr inc_node;

  /* Create resource address node */
  inc_node = xmlNewNode (NULL, BAD_CAST "incident");

  /* Basic Info */
  asprintf (&str, "%li", inc->id);
  xmlNewChild (inc_node, NULL, BAD_CAST "id", BAD_CAST str);
  free (str);
  asprintf (&str, "%u", inc->state);
  xmlNewChild (inc_node, NULL, BAD_CAST "state_int", BAD_CAST str);
//  xmlNewChild (inc_node, NULL, BAD_CAST "state", BAD_CAST i_incident_statestr (inc->state));
  free (str);
  asprintf (&str, "%u", inc->type);
  xmlNewChild (inc_node, NULL, BAD_CAST "type_int", BAD_CAST str);
//  xmlNewChild (inc_node, NULL, BAD_CAST "type", BAD_CAST i_incident_typestr (inc->type));
  free (str);

  /* Time stamps */
  asprintf (&str, "%lu", inc->start_tv.tv_sec);
  xmlNewChild (inc_node, NULL, BAD_CAST "start_sec", BAD_CAST str);
  free (str);
//  asprintf (&str, "%lu", inc->start_tv.tv_usec);
//  xmlNewChild (inc_node, NULL, BAD_CAST "start_usec", BAD_CAST str);
//  free (str);
  asprintf (&str, "%lu", inc->end_tv.tv_sec);
  xmlNewChild (inc_node, NULL, BAD_CAST "end_sec", BAD_CAST str);
  free (str);
//  asprintf (&str, "%lu", inc->end_tv.tv_usec);
//  xmlNewChild (inc_node, NULL, BAD_CAST "end_usec", BAD_CAST str);
//  free (str);

  /* Case binding */
  asprintf (&str, "%li", inc->caseid);
  xmlNewChild (inc_node, NULL, BAD_CAST "caseid", BAD_CAST str);
  free (str);

  /* Raised/Cleared Values */
  xmlNewChild (inc_node, NULL, BAD_CAST "raised_valstr", BAD_CAST inc->raised_valstr);
  xmlNewChild (inc_node, NULL, BAD_CAST "cleared_valstr", BAD_CAST inc->cleared_valstr);
  
  /* Trigger info */
  asprintf (&str, "%i", inc->trg_type);
  xmlNewChild (inc_node, NULL, BAD_CAST "trg_type", BAD_CAST str);
  free (str);
  xmlNewChild (inc_node, NULL, BAD_CAST "trg_xval", BAD_CAST inc->trg_xval_str);
  xmlNewChild (inc_node, NULL, BAD_CAST "trg_yval", BAD_CAST inc->trg_yval_str);

  /* Resource Address */
  str = i_resource_address_struct_to_string (inc->ent->resaddr);
  xmlNewChild (inc_node, NULL, BAD_CAST "resaddr", BAD_CAST str);
  free (str);
  
  /* Entity Descriptor */
  node = i_entity_descriptor_xml (inc->ent);
  xmlAddChild (inc_node, node);

  return inc_node;
}

/* @} */
