#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
#include "device.h"
#include "resource_xml.h"
#include "incident_xml.h"
#include "xml.h"

/** \addtogroup device Devices
 * @ingroup entity
 * @{
 */

/*
 * Device-related XML functions
 */

/* XML Functions */

extern i_resource *global_self;

void i_device_xml (i_entity *ent, xmlNodePtr ent_node)
{
  char *str;
  
  /* Add device-specific data to entity node */
  i_device *dev = (i_device *) ent;

  /* Device info */
  xmlNewChild (ent_node, NULL, BAD_CAST "ip", BAD_CAST dev->ip_str);
  xmlNewChild (ent_node, NULL, BAD_CAST "lom_ip", BAD_CAST dev->lom_ip_str);
  xmlNewChild (ent_node, NULL, BAD_CAST "snmpcomm", BAD_CAST dev->snmpcomm_str);
  asprintf (&str, "%i", dev->snmpversion);
  xmlNewChild (ent_node, NULL, BAD_CAST "snmpversion", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->snmpauthmethod);
  xmlNewChild (ent_node, NULL, BAD_CAST "snmpauthmethod", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->snmpprivenc);
  xmlNewChild (ent_node, NULL, BAD_CAST "snmpprivenc", BAD_CAST str);
  free (str);
  xmlNewChild (ent_node, NULL, BAD_CAST "snmpauthpassword", BAD_CAST dev->snmpauthpass_str);
  xmlNewChild (ent_node, NULL, BAD_CAST "snmpprivpassword", BAD_CAST dev->snmpprivpass_str);
  xmlNewChild (ent_node, NULL, BAD_CAST "username", BAD_CAST dev->username_str);
  if (dev->password_str && strlen(dev->password_str) > 0) 
  { xmlNewChild (ent_node, NULL, BAD_CAST "password", BAD_CAST "********"); }
  xmlNewChild (ent_node, NULL, BAD_CAST "lom_username", BAD_CAST dev->lom_username_str);
  if (dev->lom_password_str && strlen(dev->lom_password_str) > 0) 
  { xmlNewChild (ent_node, NULL, BAD_CAST "lom_password", BAD_CAST "********"); }
  if (dev->modb_xml_str)
  { 
    xmlNewChild (ent_node, NULL, BAD_CAST "vendor", BAD_CAST dev->modb_xml_str); 
  }
  else
  { 
    xmlNewChild (ent_node, NULL, BAD_CAST "vendor", BAD_CAST dev->vendor_str); 
  }
  xmlNewChild (ent_node, NULL, BAD_CAST "profile", BAD_CAST dev->profile_str);
  asprintf (&str, "%li", dev->refresh_interval);
  xmlNewChild (ent_node, NULL, BAD_CAST "refresh_interval", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->protocol);
  xmlNewChild (ent_node, NULL, BAD_CAST "protocol", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->icmp);
  xmlNewChild (ent_node, NULL, BAD_CAST "icmp", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->lom);
  xmlNewChild (ent_node, NULL, BAD_CAST "lom", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->nagios);
  xmlNewChild (ent_node, NULL, BAD_CAST "nagios", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->xsan);
  xmlNewChild (ent_node, NULL, BAD_CAST "xsan", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->lithiumsnmp);
  xmlNewChild (ent_node, NULL, BAD_CAST "lithiumsnmp", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->swrun);
  xmlNewChild (ent_node, NULL, BAD_CAST "swrun", BAD_CAST str);
  free (str);
  asprintf (&str, "%i", dev->mark);
  xmlNewChild (ent_node, NULL, BAD_CAST "mark", BAD_CAST str);
  free (str);

  /* Refresh info */
  if (global_self->type == RES_DEVICE)
  {
    asprintf (&str, "%lu", ent->refresh_tstamp.tv_sec);
    xmlNewChild (ent_node, NULL, BAD_CAST "refresh_tstamp_sec", BAD_CAST str);
    free (str);
    
    asprintf (&str, "%lu", dev->refresh_count);
    xmlNewChild (ent_node, NULL, BAD_CAST "refresh_count", BAD_CAST str);
    free (str);
    
    xmlNewChild (ent_node, NULL, BAD_CAST "resource_started", BAD_CAST "1");
  }
  else if (global_self->type == RES_CUSTOMER)
  {
    if (dev->resaddr)
    { xmlNewChild (ent_node, NULL, BAD_CAST "resource_started", BAD_CAST "1"); }
    else
    { xmlNewChild (ent_node, NULL, BAD_CAST "resource_started", BAD_CAST "0"); }

    asprintf (&str, "%i", dev->licensed);
    xmlNewChild (ent_node, NULL, BAD_CAST "licensed", BAD_CAST str);
    free (str);
  }
  
  return;
}

/* @} */
