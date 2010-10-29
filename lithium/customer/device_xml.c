#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/opstate.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>
#include <induction/auth.h>

#include "navtree.h"
#include "lic.h"
#include "device.h"

int xml_device_update (i_resource *self, i_xml_request *req)
{
  char *sitename_str = NULL;
  char *name_str = NULL;
  char *desc_str = NULL;
  char *ip_str = NULL;
  char *lom_ip_str = NULL;
  char *snmpcomm_str = NULL;
  char *snmpauthpass_str = NULL;
  char *snmpprivpass_str = NULL;
  char *username_str = NULL;
  char *password_str = NULL;
  char *lom_username_str = NULL;
  char *lom_password_str = NULL;
  char *vendor_str = NULL;
  char *profile_str = NULL;
  char *refreshint_str = NULL;
  int snmpversion = 1;
  int snmpauthmethod = 1;
  int snmpprivenc = 1;
  int protocol = 0;
  int icmp = 0;
  int lithiumsnmp = 0;
  int swrun = 0;
  int nagios = 0;
  int lom = 0;
  int xsan = 0;
  int minimum_action_severity = 1;
  i_site *site;
  i_device *device;
  i_device *dup_device = NULL;
  xmlNodePtr ent_node;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "name") && str) name_str = strdup(str);
      else if (!strcmp((char *)node->name, "desc") && str) desc_str = strdup(str);
      else if (!strcmp((char *)node->name, "ip") && str) ip_str = strdup(str);
      else if (!strcmp((char *)node->name, "lom_ip") && str) lom_ip_str = strdup(str);
      else if (!strcmp((char *)node->name, "snmpversion") && str) snmpversion = atoi (str);
      else if (!strcmp((char *)node->name, "snmpauthmethod") && str) snmpauthmethod = atoi (str);
      else if (!strcmp((char *)node->name, "snmpprivenc") && str) snmpprivenc = atoi (str);
      else if (!strcmp((char *)node->name, "snmpcomm") && str) snmpcomm_str = strdup(str);
      else if (!strcmp((char *)node->name, "snmpauthpassword") && str) snmpauthpass_str = strdup(str);
      else if (!strcmp((char *)node->name, "snmpprivpassword") && str) snmpprivpass_str = strdup(str);
      else if (!strcmp((char *)node->name, "username") && str) username_str = strdup(str);
      else if (!strcmp((char *)node->name, "password") && str) password_str = strdup(str);
      else if (!strcmp((char *)node->name, "lom_username") && str) lom_username_str = strdup(str);
      else if (!strcmp((char *)node->name, "lom_password") && str) lom_password_str = strdup(str);
      else if (!strcmp((char *)node->name, "vendor") && str) vendor_str = strdup(str);
      else if (!strcmp((char *)node->name, "profile") && str) profile_str = strdup(str);
      else if (!strcmp((char *)node->name, "refresh_interval") && str) refreshint_str = strdup(str);
      else if (!strcmp((char *)node->name, "protocol") && str) protocol = atoi (str);
      else if (!strcmp((char *)node->name, "icmp") && str) icmp = atoi (str);
      else if (!strcmp((char *)node->name, "lithiumsnmp") && str) lithiumsnmp = atoi (str);
      else if (!strcmp((char *)node->name, "swrun") && str) swrun = atoi (str);
      else if (!strcmp((char *)node->name, "nagios") && str) nagios = atoi (str);
      else if (!strcmp((char *)node->name, "lom") && str) lom = atoi (str);
      else if (!strcmp((char *)node->name, "xsan") && str) xsan = atoi (str);
      else if (!strcmp((char *)node->name, "minimum_action_severity") && str) minimum_action_severity = atoi (str);
      else if (!strcmp((char *)node->name, "site_name") && str) sitename_str = strdup(str);
      else if (!strcmp((char *)node->name, "duplicated_from") && str)
      {
        i_entity_address *dup_addr = i_entity_address_struct (str);
        if (!dup_addr) 
        { i_printf (1, "xml_device_update received bad duplicate_from address"); return -1; }

        dup_device = (i_device *) i_entity_local_get (self, dup_addr);
        i_entity_address_free (dup_addr);
        if (!dup_device)
        { i_printf (1, "xml_device_update specified duplicate_from device not found"); return -1; }
      }

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_device_update, no xml data received");
    return -1; 
  }

  /* Try and locate the site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), sitename_str);
  if (!site)
  {
    i_printf (1, "xml_device_update failed to find site"); 
    return -1;
  }

  /* Try and retrieve the device */
  device = (i_device *) i_entity_child_get (ENTITY(site), name_str);
  if (device)
  {
    /* Rescind license */
    if (device->licensed)
    { l_lic_rescind (self, ENTITY(device)); }
    
    /* Update existing */
    if (desc_str) { free (device->desc_str); device->desc_str = strdup (desc_str); }
    if (ip_str) { free (device->ip_str); device->ip_str = strdup (ip_str); }
    if (lom_ip_str) { free (device->lom_ip_str); device->lom_ip_str = strdup (lom_ip_str); }
    if (snmpcomm_str) { free (device->snmpcomm_str); device->snmpcomm_str = strdup (snmpcomm_str); }
    if (snmpauthpass_str) { free (device->snmpauthpass_str); device->snmpauthpass_str = strdup (snmpauthpass_str); }
    if (snmpprivpass_str) { free (device->snmpprivpass_str); device->snmpprivpass_str = strdup (snmpprivpass_str); }
    device->snmpversion = snmpversion;
    device->snmpauthmethod = snmpauthmethod;
    device->snmpprivenc = snmpprivenc;
    if (username_str) { free (device->username_str); device->username_str = strdup (username_str); }
    if (password_str && strlen(password_str) > 0) { free (device->password_str); device->password_str = strdup (password_str); }
    if (lom_username_str) { free (device->lom_username_str); device->lom_username_str = strdup (lom_username_str); }
    if (lom_password_str && strlen(lom_password_str) > 0) { free (device->lom_password_str); device->lom_password_str = strdup (lom_password_str); }
    if (vendor_str) { free (device->vendor_str); device->vendor_str = strdup (vendor_str); }
    if (profile_str) { free (device->profile_str); device->profile_str = strdup (profile_str); }
    if (refreshint_str) { device->refresh_interval = atoi (refreshint_str); }
    device->protocol = protocol;
    device->icmp = icmp;
    device->lithiumsnmp = lithiumsnmp;
    device->swrun = swrun;
    device->nagios = nagios;
    device->lom = lom;
    device->xsan = xsan;
    device->minimum_action_severity = minimum_action_severity;
    l_device_update (self, device);
  }
  else
  {
    /* 
     * Add new 
     */

    /* Set defaults */
    int refresh_interval;
    if (refreshint_str) refresh_interval = atoi (refreshint_str);
    else refresh_interval = 300;

    /* Check if we're being duplicated */
    if (dup_device && !password_str && dup_device->password_str)
    { password_str = strdup (dup_device->password_str); }

    /* Add Device */   
    device = l_device_add (self, site, name_str, desc_str, ip_str, lom_ip_str, snmpversion, snmpcomm_str, snmpauthpass_str, snmpprivpass_str, snmpauthmethod, snmpprivenc, username_str, password_str, lom_username_str, lom_password_str, vendor_str, profile_str, refresh_interval, protocol, icmp, lithiumsnmp, swrun, nagios, lom, xsan, minimum_action_severity);

    /* Check result */
    if (!device)
    { i_printf (1, "xml_device_update failed to call l_device_add to add new device"); return -1; }
  }

  /* Clean up */
  if (name_str) free (name_str);
  if (desc_str) free (desc_str);
  if (ip_str) free (ip_str);
  if (lom_ip_str) free (lom_ip_str);
  if (snmpcomm_str) free (snmpcomm_str);
  if (snmpauthpass_str) free (snmpauthpass_str);
  if (snmpprivpass_str) free (snmpprivpass_str);
  if (username_str) free (username_str);
  if (password_str) free (password_str);
  if (lom_username_str) free (lom_username_str);
  if (lom_password_str) free (lom_password_str);
  if (vendor_str) free (vendor_str);
  if (profile_str) free (profile_str);
  if (refreshint_str) free (refreshint_str);
  if (sitename_str) free (sitename_str);

  /* Create return case XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  ent_node = i_entity_xml (ENTITY(device), 0, 0);
  xmlDocSetRootElement (req->xml_out->doc, ent_node);
  
  return 1;
}

int xml_device_remove (i_resource *self, i_xml_request *req)
{
  char *sitename_str = NULL;
  char *name_str = NULL;
  i_site *site;
  i_device *device;
  xmlNodePtr ent_node;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "name") && str) name_str = strdup(str);
      else if (!strcmp((char *)node->name, "site_name") && str) sitename_str = strdup(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_device_delete failed, no xml data received");
    return -1; 
  }

  /* Try and locate the site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), sitename_str);
  if (!site)
  {
    i_printf (1, "xml_device_delete failed to find site (%s)", sitename_str); 
    free (sitename_str);
    return -1;
  }
  free (sitename_str);

  /* Try and retrieve the device */
  device = (i_device *) i_entity_child_get (ENTITY(site), name_str);
  if (device)
  {
    /* Remove device */
    l_device_remove (self, device);
  }
  else
  {
    i_printf (1, "xml_device_delete failed to find device (%s at %s)", name_str, site->name_str); 
    free (name_str);
    return -1;
  }
  free (name_str);

  /* Create return site XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  ent_node = i_entity_xml (ENTITY(site->cust), 0, 0);
  xmlDocSetRootElement (req->xml_out->doc, ent_node);
          
  return 1;
}

int xml_device_mark (i_resource *self, i_xml_request *req)
{
  char *sitename_str = NULL;
  char *name_str = NULL;
  int mark = 0;
  i_site *site;
  i_device *device;
  xmlNodePtr ent_node;
  
  /* Check permission */
  if (req->auth->level < AUTH_LEVEL_ADMIN)
  { req->xml_out = i_xml_denied (); return 1; }

  /* Interpret XML */
  if (req->xml_in)
  {
    /* Set variables  */
    xmlNodePtr node;
    xmlNodePtr root_node = NULL;
    root_node = xmlDocGetRootElement (req->xml_in->doc);
    for (node = root_node->children; node; node = node->next)
    {
      char *str;

      str = (char *) xmlNodeListGetString (req->xml_in->doc, node->xmlChildrenNode, 1);
 
      if (!strcmp((char *)node->name, "name") && str) name_str = strdup(str);
      else if (!strcmp((char *)node->name, "site_name") && str) sitename_str = strdup(str);
      else if (!strcmp((char *)node->name, "mark") && str) mark = atoi(str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_device_mark failed, no xml data received");
    return -1; 
  }

  /* Try and locate the site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), sitename_str);
  if (!site)
  {
    i_printf (1, "xml_device_mark failed to find site (%s)", sitename_str); 
    free (sitename_str);
    return -1;
  }
  free (sitename_str);

  /* Try and retrieve the device */
  device = (i_device *) i_entity_child_get (ENTITY(site), name_str);
  if (device)
  {
    /* Mark device */
    device->mark = mark;
    i_opstate_change (self, ENTITY(device), mark);

    /* Update SQL */  
    i_pg_async_conn *conn = i_pg_async_conn_open_customer (self);
    if (conn)
    {
      /* Create query */
      char *query;
      asprintf (&query, "UPDATE devices SET mark='%i' WHERE site='%s' AND name='%s'", mark, site->name_str, device->name_str);

      /* Execute query */
      int num = i_pg_async_query_exec (self, conn, query, 0, NULL, NULL);
      free (query);
      if (num != 0)
      { i_printf (1, "xml_device_mark warning, failed to execute UPDATE query"); i_pg_async_conn_close (conn); }
    }
    else
    { i_printf (1, "xml_device_mark failed to open SQL connection"); }

    /* Restart device */
    l_device_res_restart (self, device);
  }
  else
  {
    i_printf (1, "xml_device_mark failed to find device (%s at %s)", name_str, site->name_str); 
    free (name_str);
    return -1;
  }
  free (name_str);

  /* Create return site XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  ent_node = i_entity_xml (ENTITY(site->cust), 0, 0);
  xmlDocSetRootElement (req->xml_out->doc, ent_node);
          
  return 1;
}

