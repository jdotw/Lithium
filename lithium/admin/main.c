#include <stdio.h>
#include <stdlib.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/customer.h>
#include <induction/construct.h>
#include <induction/log.h>
#include <induction/postgresql.h>

#include "snmpagent.h"
#include "config.h"
#include "customer.h"
#include "userauth.h"
#include "lic_admin.h"

#ifdef HAVE_DNS_SD
  #include <dns_sd.h>
#endif

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Administration Module");
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
  i_msgproc_handler *hdlr;
  
  i_printf (0, "module_init entered.");
  
  /* Enable logging */
  num = i_log_enable (self, NULL);
  if (num != 0)
  { i_printf (1, "module_entry warning, failed to enable i_log"); }
  
  /* MSG_AUTH_VERIFY handler */
  hdlr = i_msgproc_handler_add (self, self->core_socket, MSG_AUTH_VERIFY, l_userauth_handler, NULL);
  if (!hdlr)
  { i_printf (1, "module_init failed to add handler for MSG_AUTH_VERIFY"); }

  /* Enable async postgres */
  num = i_pg_async_enable (self);
  if (num != 0)
  { i_printf (1, "module_init failed to enable async postgres sub-system"); }
  
  /* Enable SNMP Agent */
//  num = l_snmpagent_enable (self);
//  i_printf (1, "module_init enabled l_snmpagent");
//  if (num != 0)
//  { i_printf (1, "module_init failed to enable snmp agent sub-system"); return -1; }
//

  /* Initialise license SQL */
  l_lic_initsql (self);
  l_lic_initadmin (self);

  /* Initialise customer SQL */
  num = l_customer_initsql (self);
  if (num != 0)
  { 
    i_printf (1, "module_init failed to initialise customer SQL table"); 
//    i_timer_add (self, 5, 0, l_customer_initsql_retry, NULL);
  }

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

  int num;

  i_printf (0, "module_entry entered.");

  /* Load all customers */
  num = l_customer_loadall (self);
  if (num != 0)
  { 
    i_printf (1, "module_entry failed to load customers"); 
    i_timer_add (self, 5, 0, l_customer_loadall_retry, NULL);
  }      
  
  /* Annouce via BonJour */
#ifdef HAVE_DNS_SD
  i_list *customer_list = l_customer_list();
  i_list_move_head(customer_list);
  i_customer *first_customer = i_list_restore(customer_list);
  if (first_customer)
  {
    DNSServiceRef service;
    DNSServiceRegister (&service, 0, 0, NULL, "_lithium._tcp", NULL, NULL, htons(51180), strlen(first_customer, NULL, NULL, NULL);
  }
#endif
  
  return 0;
}

int module_shutdown (i_resource *self)
{
  i_printf (0, "module_shutdown entered.");
  
  return 0;
}
