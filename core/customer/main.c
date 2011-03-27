#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <induction.h>
#include <induction/timer.h>
#include <induction/site.h>
#include <induction/hashtable.h>
#include <induction/hierarchy.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/inventory.h>
#include <induction/ip.h>
#include <induction/ipregistry.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/log.h>
#include <induction/message.h>
#include <induction/socket.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/construct.h>
#include <induction/customer.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/navtree.h>
#include <induction/xml.h>
#include <induction/contact.h>
#include <induction/path.h>
#include <induction/user.h>

#include "config.h"
#include "lic.h"
#include "site.h"
#include "device.h"
#include "triggerset.h"
#include "record.h"
#include "imagecache.h"
#include "resready_handler.h"
#include "authentication_handler.h"
#include "notification_handler.h"
#include "ipregistry.h"
#include "svcregistry.h"
#include "procregistry.h"
#include "xsanregistry.h"
#include "lunregistry.h"
#include "inventory.h"
#include "action.h"
#include "pgsql_maint.h"
#include "infstat.h"
#include "navtree.h"
#include "case.h"
#include "incident.h"
#include "dhcp.h"
#include "customer_xml.h"
#include "devnormal.h"
#include "devtest.h"
#include "group.h"
#include "document.h"
#include "scene.h"
#include "vrack.h"
#include "searchcache.h"

#ifdef HAVE_DNS_SD
#include <dns_sd.h>
#include "bonjour.h"
#endif

#define SITE_TABLE_SIZE 5000

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Customer Module");
}

int module_sanity_check (i_construct *construct)
{
  /* 
   * This is where any pre-spawning sanity checking is done.
   * Most important is to verify that the configuration passed
   * to the function will result in a module that will spawn
   * and actually work
   *
   * return 0 on 'all ok'
   * return -1 on 'failure'
   *
   */
  
  return 0;
}

int module_init (i_resource *self)
{
  /*
   * This is where initialisation of the module occurs.
   * A crucial step here is for the module to register itself
   * with the core. Otherwise the core will attempt to respawn 
   * the module.
   *
   * return 0 on 'all ok'
   * return -1 on 'failure'
   *
   */

  int num;
  char *str;
  i_msgproc_handler *hdlr;

  i_printf (0, "customer module_init entered (%s).", self->ident_str);

  /* Create action scripts directory */
  char *dir = i_path_glue (self->root, "action_scripts");
  mkdir (dir, 0755);
  free (dir);
  dir = i_path_glue (self->root, "service_scripts");
  mkdir (dir, 0755);
  free (dir);

  /* Set XML Func */
  self->hierarchy->cust->xml_func = l_customer_xmlfunc;

  /* Create database */
  if (self->hierarchy->cust->use_lithium_db == 0)
  {
    asprintf (&str, "customer_%s", self->hierarchy->cust->name_str);
    num = i_pg_checkcreate_db (self, str);
    free (str);
    if (num != 0)
    { i_printf (1, "module_init failed to create SQL database for customer %s", self->hierarchy->cust->name_str); return -1; }
  }

  /* Configure customer entity */
  self->hierarchy->cust->navtree_func = l_navtree_func_cust;

  /* Enabled Async postgres */
  num = i_pg_async_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable async postgres sub-system"); return -1; }

  /* Initialize userdb */
  i_user_sql_init(self);

  /* Initialise search cache */
  l_searchcache_init(self);

  /* Enable PostgreSQL Maintenance */
//  num = l_pgsql_maint_enable (self);
//  if (num != 0)
//  { i_printf (1, "module_init failed to enable postgres maintenance sub-system"); return -1; }

  /* Enable cases */
  num = l_case_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable case sub-system"); return -1; }

  /* Initialise devices SQL table */
  num = l_device_initsql (self);
  if (num != 0)
  { i_printf (1, "module_init failed to initialise device SQL table"); return -1; }

  /* Reset license entitlements */
  num = l_lic_reset_entitlement (self);
  if (num != 0)
  { i_printf (0, "module_init failed to reset license entitlements"); return -1; }

  /* Set license refresh */
  i_timer_add (self, (60 * 60 * 12), 0, l_lic_entitlement_reset_timercb, NULL);

  /* License the customer */
  self->hierarchy->cust->licensed = l_lic_take (self, ENTITY(self->hierarchy->cust));

  /* Initialise sites SQL table */
  num = l_site_initsql (self);
  if (num != 0)
  { i_printf (1, "module_init failed to initialise site SQL table"); return -1; }

  /* Initialise triggerset SQL tables */
  num = l_triggerset_initsql (self);
  if (num != 0)
  { i_printf (1, "module_init failed to initialise triggerset SQL tables"); return -1; }

  /* Initialise record SQL tables */
  num = l_record_initsql (self);
  if (num != 0)
  { i_printf (1, "module_init failed to initialise recrules SQL tables"); return -1; }

  /* Enable Log Sub-system */
  num = i_log_enable (self, NULL);
  if (num != 0)
  { i_printf (1, "module_init failed to enable log sub-system"); return -1; }

  /* Enable Incident Sub-System */
  num = l_incident_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable incident sub-system"); return -1; }

  /* Enable Groups */
  num = l_group_enable (self);
  if (num != 0)
  { i_printf (1, "module_init warning, failed to enable groups"); }

  /* Enable Documents */
  num = l_document_enable (self);
  if (num != 0)
  { i_printf (1, "module_init warning, failed to enable documents"); }

  /* Enable Scene Documents */
  num = l_scene_enable (self);
  if (num != 0)
  { i_printf (1, "module_init warning, failed to enable scene documents"); }

  /* Enable VRack Documents */
  num = l_vrack_enable (self);
  if (num != 0)
  { i_printf (1, "module_init warning, failed to enable vrack documents"); }

  /* Install required message handlers */
  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_AUTH_REQUIRED, l_authentication_required_handler, NULL);
  if (!hdlr)
  { i_printf (1, "module_init failed to add handler for MSG_AUTH_REQUIRED"); return -1; }

  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_AUTH_VERIFY, l_authentication_check_handler, NULL);
  if (!hdlr)
  { i_printf (1, "module_init failed to add handler for MSG_AUTH_VERIFY"); return -1; }

  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_RES_READY, l_resready_handler, NULL);
  if (!hdlr)
  { i_printf (1, "module_init failed to add handler for MSG_AUTH_VERIFY"); return -1; }

  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_NOTIFICATION, l_notification_handler, NULL);
  if (!hdlr)
  { i_printf (1, "module_init failed to add message handler for MSG_NOFICIATION"); return -1; } 

  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_DEVNORMAL_REPORT, l_devnormal_handler_report, NULL);
  if (!hdlr)
  { i_printf (1, "module_init failed to add message handler for MSG_DEVNORMAL_REPORT"); return -1; } 
  
  /* Enable IP Registry */
  num = l_ipregistry_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable l_ipregistry"); return -1; }
  
  /* Enable Service Registry */
  num = l_svcregistry_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable l_svcregistry"); return -1; }
  
  /* Enable Process Registry */
  num = l_procregistry_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable l_procregistry"); return -1; }
  
  /* Enable Xsan Registry */
  num = l_xsanregistry_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable l_svcregistry"); return -1; }
  
  /* Enable LUN Registry */
  num = l_lunregistry_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable l_svcregistry"); return -1; }
  
  /* Enable Inventory */
  num = l_inventory_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable l_inventory"); return -1; }

  /* Enable bonjour */
#ifdef HAVE_DNS_SD
  l_bonjour_enable (self);
#endif
  
  /* Enable actions */
  num = l_action_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable l_action"); return -1; }

  /* Enable device testing */
  num = l_devtest_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable devtest"); return -1; }

  /* Configure site list sorting */
  self->hierarchy->cust->site_list_sortfunc = l_site_sortfunc_suburb;

  /* Load sites */
  num = l_site_loadall (self);
  if (num != 0)
  { i_printf (1, "module_init failed to load sites"); return -1; }

  /* Enable imagecache cleanup */
  num = l_imagecache_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable imagecache clean sub-system"); return -1; }

  return 0;
}

int module_entry (i_resource *self)
{
  /*
   * This is the main() equivalent for a lithium module.
   * Run the module from this function.
   *
   * return 0 on 'successful run'
   * return -1 on 'failed run'
   *
   */

  i_printf (0, "module_entry entered.");

  return 0;
}

int module_shutdown (i_resource *self)
{
  i_printf (0, "module_shutdown entered.");
  
  return 0;
}
