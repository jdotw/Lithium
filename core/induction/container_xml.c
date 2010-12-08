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
#include "container.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "xml.h"

/** \addtogroup container Containers
 * @ingroup entity
 * @{
 */

/*
 * Container-related XML functions
 */

/* XML Functions */

void i_container_xml (i_entity *ent, xmlNodePtr ent_node)
{
  /* Add container-specific data to entity node */
  char *str;
  i_container *cnt = (i_container *) ent;

  /* Item info */
  asprintf (&str, "%u", cnt->item_list_state);
  xmlNewChild (ent_node, NULL, BAD_CAST "item_list_state_int", BAD_CAST str);
  free (str);

  /* Module Builder Flag */
  asprintf (&str, "%u", cnt->modb);
  xmlNewChild (ent_node, NULL, BAD_CAST "modb", BAD_CAST str);
  free (str);

  /* Console View */
  if (cnt->cview1_str) xmlNewChild (ent_node, NULL, BAD_CAST "cview1", BAD_CAST cnt->cview1_str);
  if (cnt->cview2_str) xmlNewChild (ent_node, NULL, BAD_CAST "cview2", BAD_CAST cnt->cview2_str);
  if (cnt->cview3_str) xmlNewChild (ent_node, NULL, BAD_CAST "cview3", BAD_CAST cnt->cview3_str);
  if (cnt->cview4_str) xmlNewChild (ent_node, NULL, BAD_CAST "cview4", BAD_CAST cnt->cview4_str);

  return;
}

/* @} */
