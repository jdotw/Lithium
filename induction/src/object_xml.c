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
#include "object.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "xml.h"

/** \addtogroup object Objects
 * @ingroup entity
 * @{
 */

/*
 * Object-related XML functions
 */

/* XML Functions */

void i_object_xml (i_entity *ent, xmlNodePtr ent_node, unsigned short flags)
{
  char *str;
  
  /* Add object-specific data to entity node */
  i_object *obj = (i_object *) ent;

  /* Index */
  asprintf (&str, "%lu", obj->index);
  xmlNewChild (ent_node, NULL, BAD_CAST "index", BAD_CAST str);
  free (str);

  return;
}

/* @} */
