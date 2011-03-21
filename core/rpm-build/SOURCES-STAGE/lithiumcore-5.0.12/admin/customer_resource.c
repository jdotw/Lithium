#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/module.h>
#include <induction/auth.h>
#include <induction/hashtable.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/construct.h>
#include <induction/list.h>
#include <induction/timer.h>

#include "customer.h"
#include "autoconf.h"

/* 
 * Customer Resource Functions 
 */

/* 
 * Resource (Re)start 
 */

int l_customer_res_restart (i_resource *self, i_customer *cust)
{
  char *fsroot;

  /* Destroy existing resource */
  l_customer_res_destroy (self, cust);

  /* Create fsroot */
  fsroot = i_entity_path (self, ENTITY(cust), 0, ENTPATH_ROOT|ENTPATH_CREATE);
  if (!fsroot)
  { i_printf (1, "l_customer_res_restart failed to create fsroot for %s", cust->name_str); return -1; }

  /* Create resource */
  cust->spawn_op = i_resource_spawn (self, RES_CUSTOMER, -1, cust->name_str, CUSTOMER_MODULE, fsroot, cust->name_str, (i_entity *) cust, l_customer_res_restart_spawncb, strdup(cust->name_str));
  free (fsroot);
  if (!cust->spawn_op)
  { i_printf (1, "l_customer_res_restart failed to create resource for customer %s", cust->name_str); return -1; }

  return 0;
}

int l_customer_res_restart_spawncb (i_resource *self, i_resource_address *addr, void *passdata)
{
  char *name_str = passdata;
  i_customer *cust;

  /* Retrieve customer */
  cust = l_customer_get (self, name_str);
//  free (name_str);

  /* Process message */
  if (cust)
  {
    /* NULLify spawnop */
    cust->spawn_op = NULL;

    /* Set address */
    cust->resaddr = i_resource_address_duplicate (addr);

    /* Update autoconfs */
    l_autoconf_generate_customer (self, cust);
    l_autoconf_generate_deployment (self, l_customer_list());
  
    /* Log */
    i_printf (0, "l_customer_res_restart_spawncb customer resource %s online at %s:%s:%i:%i:%s",
      cust->name_str, addr->plexus, addr->node, addr->type, addr->ident_int, addr->ident_str);
  }
  else
  {
    /* No customer exists, destroy resource immediately */
    i_resource_destroy (self, addr, NULL, NULL);
    i_printf (0, "l_customer_res_restart_spawncb non-existant customer %s attempted to joint from %s:%s:%i:%i:%s",
      name_str, addr->plexus, addr->node, addr->type, addr->ident_int, addr->ident_str);
  }

  return 0;
}

/* 
 * Resource Destroy 
 */

int l_customer_res_destroy (i_resource *self, i_customer *cust)
{
  /* Destroy existing resource */
  if (cust->resaddr)
  {
    i_resource_destroy_cbdata *data;
    
    data = i_resource_destroy (self, cust->resaddr, NULL, NULL);
    if (!data)
    { 
      i_printf (1, "l_customer_res_destroy failed to destroy resource %s:%s:%i:%i:%s", 
        cust->resaddr->plexus, cust->resaddr->node, cust->resaddr->type, cust->resaddr->ident_int, cust->resaddr->ident_str);
    }

    i_resource_address_free (cust->resaddr);
    cust->resaddr = NULL;
  }

  return 0;
}


