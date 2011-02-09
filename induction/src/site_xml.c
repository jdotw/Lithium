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
#include "site.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "xml.h"

/** \addtogroup site Sites
 * @ingroup entity
 * @{
 */

/*
 * Site-related XML functions
 */

/* XML Functions */

void i_site_xml (i_entity *ent, xmlNodePtr ent_node, unsigned short flags)
{
  char *str;
  
  /* Add site-specific data to entity node */
  i_site *site = (i_site *) ent;

  /* Address info */
  if (!(flags & ENTXML_MOBILE))
  {
    xmlNewChild (ent_node, NULL, BAD_CAST "addr1", BAD_CAST site->addr1_str);
    xmlNewChild (ent_node, NULL, BAD_CAST "addr2", BAD_CAST site->addr2_str);
    xmlNewChild (ent_node, NULL, BAD_CAST "addr3", BAD_CAST site->addr3_str);
    xmlNewChild (ent_node, NULL, BAD_CAST "suburb", BAD_CAST site->suburb_str);
    xmlNewChild (ent_node, NULL, BAD_CAST "state", BAD_CAST site->state_str);
    xmlNewChild (ent_node, NULL, BAD_CAST "postcode", BAD_CAST site->postcode_str);
    xmlNewChild (ent_node, NULL, BAD_CAST "country", BAD_CAST site->country_str);
    asprintf (&str, "%f", site->longitude);
    xmlNewChild (ent_node, NULL, BAD_CAST "longitude", BAD_CAST str);
    free (str);
    asprintf (&str, "%f", site->latitude);
    xmlNewChild (ent_node, NULL, BAD_CAST "latitude", BAD_CAST str);
    free (str);
    asprintf (&str, "%i", site->licensed);
    xmlNewChild (ent_node, NULL, BAD_CAST "licensed", BAD_CAST str);
    free (str);
  }

  return;
}

/* @} */
