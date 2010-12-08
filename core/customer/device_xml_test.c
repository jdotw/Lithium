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
#include <induction/hierarchy.h>
#include <induction/timer.h>
#include <induction/xml.h>
#include <induction/entity_xml.h>
#include <induction/auth.h>

#include "navtree.h"
#include "lic.h"
#include "device.h"
#include "devtest.h"

int l_device_xml_test_callback (i_resource *self, int result, void *passdata);

int xml_device_test (i_resource *self, i_xml_request *req)
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
  int lom = 0;
  i_site *site;
  i_device *device;
  
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
      else if (!strcmp((char *)node->name, "site_name") && str) sitename_str = strdup(str);
      else if (!strcmp((char *)node->name, "protocol") && str) protocol = atoi (str);
      else if (!strcmp((char *)node->name, "lom") && str) lom = atoi (str);

      xmlFree (str);
    }
  }
  else
  {
    i_printf (1, "xml_device_test, no xml data received");
    return -1; 
  }

  /* Try and locate the site */
  site = (i_site *) i_entity_child_get (ENTITY(self->hierarchy->cust), sitename_str);
  if (!site)
  {
    i_printf (1, "xml_device_test failed to find site"); 
    return -1;
  }

  /* Create dev struct */
  int refresh_interval;
  if (refreshint_str) refresh_interval = atoi (refreshint_str);
  else refresh_interval = 300;
  device = i_device_create (name_str, desc_str, ip_str, lom_ip_str, snmpcomm_str, username_str, password_str, lom_username_str, lom_password_str, vendor_str, profile_str, refresh_interval);
  device->snmpversion = snmpversion;
  device->snmpauthmethod = snmpauthmethod;
  device->snmpprivenc = snmpprivenc;
  device->protocol = protocol;
  device->lom = lom;
  if (snmpauthpass_str) device->snmpauthpass_str = strdup (snmpauthpass_str);
  if (snmpprivpass_str) device->snmpprivpass_str = strdup (snmpprivpass_str);
  if (!device)
  { i_printf (1, "xml_device_test failed to call l_device_add to add new device"); return -1; }
  
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

  /* Perform test */
  int num;
  if (strstr(device->vendor_str, "xserve_") || strstr(device->vendor_str, "osx_"))
  {
    /* OSX-Style tests */
    if (device->protocol == 1)
    {
      num = l_devtest_servermgr (self, device, l_device_xml_test_callback, req); 
    }
    else
    {
      num = l_devtest_snmp (self, device, l_device_xml_test_callback, req);
    }
  }
//  else if (strcmp(device->vendor_str, "xserve_intel") == 0)
//  {
    /* Peform Xseve Intel Tests (LOM) */
//    num = l_devtest_xintel (self, device, l_device_xml_test_callback, req);
//  }
  else if (strcmp(device->vendor_str, "xraid") == 0)
  {
    /* Perform Xserve RAID tests */
    num = l_devtest_xraid (self, device, l_device_xml_test_callback, req); 
  }
  else
  { 
    /* Default: Perform SNMP Test */
    num = l_devtest_snmp (self, device, l_device_xml_test_callback, req);
  }

  /* Check result */
  if (num != 0)
  {
    /* Immediate failure */
    req->xml_out = i_xml_create ();
    req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "device_test");
    xmlDocSetRootElement(req->xml_out->doc, root_node);
    xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST "0");

    return 1;
  }
      
  return 0;
}

int l_device_xml_test_callback (i_resource *self, int result, void *passdata)
{
  i_xml_request *req = passdata;
  
  /* Create return XML */
  req->xml_out = i_xml_create ();
  req->xml_out->doc = xmlNewDoc (BAD_CAST "1.0");
  xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "device_test");
  xmlDocSetRootElement(req->xml_out->doc, root_node);
  char *str;
  asprintf (&str, "%i", result);
  xmlNewChild (root_node, NULL, BAD_CAST "result", BAD_CAST str);
  free (str);

  i_xml_deliver (self, req);

  return 0;
}
