#include <stdlib.h> 
#include <string.h>
#include <libxml/parser.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/entity_xml.h>
#include <induction/postgresql.h>
#include <induction/xml.h>
#include <induction/customer.h>
#include <induction/vendor.h>
#include <induction/contact.h>
#include <induction/user.h>
#include <induction/version.h>

#include "customer_xml.h"
#include "case.h"
#include "incident.h"
#include "infstat.h"
#include "config.h"
#include "lic.h"

/*
 * Customer XML functions
 */

extern i_resource *global_self;

int l_customer_xmlfunc (i_customer *cust, xmlNodePtr cust_node)
{

  /* Infstat Info */
//  asprintf (&str, "%lu", l_infstat_issue_count_offline());
//  xmlNewChild (cust_node, NULL, BAD_CAST "infstat_offline_count", BAD_CAST str);
//  free (str);
//  asprintf (&str, "%lu", l_infstat_issue_count_impaired());
//  xmlNewChild (cust_node, NULL, BAD_CAST "infstat_impaired_count", BAD_CAST str);
//  free (str);
//  asprintf (&str, "%lu", l_infstat_issue_count_trivial());
//  xmlNewChild (cust_node, NULL, BAD_CAST "infstat_trivial_count", BAD_CAST str);
//  free (str);
//  asprintf (&str, "%lu", l_infstat_issue_count_normal());
//  xmlNewChild (cust_node, NULL, BAD_CAST "infstat_normal_count", BAD_CAST str);
//  free (str);

  /* Entity States */
//  asprintf (&str, "%lu", l_incident_count_failed());
//  xmlNewChild (cust_node, NULL, BAD_CAST "entity_failed_count", BAD_CAST str);
//  free (str);
//  asprintf (&str, "%lu", l_incident_count_impaired());
//  xmlNewChild (cust_node, NULL, BAD_CAST "entity_impaired_count", BAD_CAST str);
//  free (str);
//  asprintf (&str, "%lu", l_incident_count_atrisk());
//  xmlNewChild (cust_node, NULL, BAD_CAST "entity_atrisk_count", BAD_CAST str);
//  free (str);

  /* Versions */
  xmlNewChild (cust_node, NULL, BAD_CAST "core_version", BAD_CAST LITHIUM_VERSION);
  xmlNewChild (cust_node, NULL, BAD_CAST "induction_version", BAD_CAST INDUCTION_VERSION);

  /* License info */
  l_lic_entitlement *licent = l_lic_static_entitlement ();
  if (licent->paid == 1) 
  { xmlNewChild (cust_node, NULL, BAD_CAST "license_type", BAD_CAST "1"); }
  else if (licent->demo == 1)
  { xmlNewChild (cust_node, NULL, BAD_CAST "license_type", BAD_CAST "2"); }
  else if (licent->demo == 2)
  { xmlNewChild (cust_node, NULL, BAD_CAST "license_type", BAD_CAST "3"); }
  else if (licent->nfr == 1)
  { xmlNewChild (cust_node, NULL, BAD_CAST "license_type", BAD_CAST "4"); }
  else if (licent->nfr == 2)
  { xmlNewChild (cust_node, NULL, BAD_CAST "license_type", BAD_CAST "5"); }
  else if (licent->free == 1)
  { xmlNewChild (cust_node, NULL, BAD_CAST "license_type", BAD_CAST "6"); }

  /* Limited License Flag */
  char *str;
  asprintf (&str, "%i", licent->limited);
  xmlNewChild (cust_node, NULL, BAD_CAST "license_limited", BAD_CAST str);
  free (str);

  /* Configured flag */
  asprintf (&str, "%i", cust->configured);
  xmlNewChild (cust_node, NULL, BAD_CAST "configured", BAD_CAST str);
  free (str);

  /* Vendor module list */
  xmlNodePtr vendor_root_node = xmlNewNode(NULL, BAD_CAST "custom_module_list");
  i_list *vendor_list = i_vendor_list (global_self);
  i_vendor *vendor = NULL;
  for (i_list_move_head(vendor_list); (vendor=i_list_restore(vendor_list))!=NULL; i_list_move_next(vendor_list))
  {
    if (!strstr(".xml", vendor->name_str)) continue; 
    xmlNodePtr vendor_node = xmlNewNode (NULL, BAD_CAST "custom_module");
    xmlNewChild (vendor_node, NULL, BAD_CAST "name", BAD_CAST vendor->name_str);
    xmlNewChild (vendor_node, NULL, BAD_CAST "desc", BAD_CAST vendor->desc_str);
    xmlAddChild (vendor_root_node, vendor_node);
  }
  xmlAddChild (cust_node, vendor_root_node);
  if (vendor_list) i_list_free (vendor_list);

  return 0;
}

