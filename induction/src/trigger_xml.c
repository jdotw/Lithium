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
#include "list.h"
#include "respond.h"
#include "incident.h"
#include "metric.h"
#include "value.h"
#include "trigger.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "xml.h"
#include "trigger_xml.h"

/** \addtogroup trigger Triggers
 * @ingroup entity
 * @{
 */

/*
 * Trigger-related XML functions
 */

/* XML Functions */

void i_trigger_xml (i_entity *ent, xmlNodePtr ent_node, unsigned short flags)
{
  /* Add trigger-specific data to entity node */
  char *str;
  i_trigger *trg = (i_trigger *) ent;

  /* Value type */
  asprintf (&str, "%u", trg->val_type);
  xmlNewChild (ent_node, NULL, BAD_CAST "val_type_num", BAD_CAST str);
  free (str);
//  xmlNewChild (ent_node, NULL, BAD_CAST "val_type", BAD_CAST i_value_typestr (trg->val_type));

  /* Values */
  str = i_value_valstr_raw (trg->val_type, trg->val);
  xmlNewChild (ent_node, NULL, BAD_CAST "xval", BAD_CAST str);
  free (str);
  if (trg->yval)
  { 
    str = i_value_valstr_raw (trg->val_type, trg->yval);
    xmlNewChild (ent_node, NULL, BAD_CAST "yval", BAD_CAST str);
    free (str);
  }  

  /* Trigger type */
  asprintf (&str, "%u", trg->trg_type);
  xmlNewChild (ent_node, NULL, BAD_CAST "trg_type_num", BAD_CAST str);
  free (str);
 // xmlNewChild (ent_node, NULL, BAD_CAST "trg_type", BAD_CAST i_trigger_typestr (trg->trg_type));

  /* Effect */
  asprintf (&str, "%i", trg->effect);
  xmlNewChild (ent_node, NULL, BAD_CAST "effect_num", BAD_CAST str);
  free (str);
//  xmlNewChild (ent_node, NULL, BAD_CAST "effect", BAD_CAST i_entity_opstatestr(trg->effect));
  
  /* Duration */
  asprintf (&str, "%li", trg->duration_sec);
  xmlNewChild (ent_node, NULL, BAD_CAST "duration", BAD_CAST str);
  free (str);
  
  /* Active Time */
  asprintf (&str, "%li", trg->active_tstamp.tv_sec);
  xmlNewChild (ent_node, NULL, BAD_CAST "active_tstamp", BAD_CAST str);
  free (str);

  return;
}


/* @} */
