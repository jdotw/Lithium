#include <stdio.h>
#include <stdlib.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/hashtable.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/hierarchy.h"
#include "induction/postgresql.h"
#include "induction/interface.h"
#include "induction/construct.h"
#include "device/snmp.h"
#include "device/icmp.h"

char* vendor_name ()
{ return "icmp"; }
char *vendor_desc ()
{ return "ICMP Ping"; }

void module_info ()
{
  /*
   * This is a utility function which should only i_printf()
   * a banner describing the module / version
   */
  
  i_printf (0, "Lithium Device ICMP Vendor Module");
}

int module_sanity_check (i_construct *construct, i_form *config_form)
{
  return 0;
}

int module_init (i_resource *self, i_form *config_form)
{
  i_printf (0, "icmp vendor module_init entered.");
  return 0;
}

int module_entry (i_resource *self, i_form *config_form)
{
  int num;
  
  i_printf (0, "icmp vendor module_entry entered.");

//  printf ("==%i== pausing for 5 seconds for gdb attach\n", getpid()); 
//  sleep (5);

  /* Disable SNMP */
  num = l_snmp_disable (self);
  
  /* Enable ICMP */
  num = l_icmp_enable (self, 0);
  if (num != 0)
  { i_printf (1, "icmp vendor module_entry failed to enable l_icmp"); }
        
  return 0;
}

int module_shutdown (void *res_ptr, void *data, int datasize)
{
  i_printf (0, "icmp vendor module_shutdown entered.");
  
  return 0;
}
