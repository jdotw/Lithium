#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>

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

#include "navtree.h"
#include "lic.h"

/* 
 * License Related Functions
 */

/* Take/Rescind -- License allocation 
 *
 * Returns 1 if it's licensed
 * Returns 0 if there's no license available
 *
 */

int l_lic_take (i_resource *self, i_entity *ent)
{
  /* Take a license for the specified entity */
  l_lic_entitlement *ement = l_lic_static_entitlement();
  
  if (ent->ent_type == ENT_CUSTOMER)
  {
    /* 
     * Customer license 
     */
    if (ement->customer_licensed)
    {
      /* Grant license */
      return 1;
    }
    else
    {
      /* Deny */
      return 0;
    }
  }
  else if (ent->ent_type == ENT_SITE)
  {
    /* Grant license */
    return 1;
  }
  else if (ent->ent_type == ENT_DEVICE)
  {
    /* 
     * Device license 
     */

    /* Check site is licensed */
    i_site *site = (i_site *) ent->parent;
    if (site->licensed != 1) return 0;

    /* Check volume */
    if (ement->devices_used < ement->devices_max || ement->devices_max == 0)
    {
      /* Grant license */
      ement->devices_used++;
      return 1;
    }
    else
    {
      /* Deny */
      ement->devices_excess++;
      return 0; 
    }
  }

  return 0;
}

void l_lic_rescind (i_resource *self, i_entity *ent)
{
  /* Rescind a license for the specified entity */
  l_lic_entitlement *ement = l_lic_static_entitlement();

  if (ent->ent_type == ENT_DEVICE)
  {
    /* 
     * Device license 
     */
    i_device *dev = (i_device *) ent;

    /* Check for entitlement */
    if (dev->licensed)
    {
      /* Rescind license */
      ement->devices_used--;
    }
    else
    {
      /* Device was overflow with no match device license */
      ement->devices_excess--;
    }
  }
}
