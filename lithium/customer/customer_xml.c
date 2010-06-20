#include <stdlib.h> 
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
#include <induction/userdb.h>
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

  char *str;
  asprintf (&str, "%i", licent->limited);
  xmlNewChild (cust_node, NULL, BAD_CAST "license_limited", BAD_CAST str);
  free (str);

  return 0;
}

