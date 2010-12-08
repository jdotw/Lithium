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

/* Entitlements */

l_lic_entitlement* l_lic_entitlement_create ()
{
  l_lic_entitlement *ement = (l_lic_entitlement *) malloc (sizeof(l_lic_entitlement));
  memset (ement, 0, sizeof(l_lic_entitlement));
  return ement;
}

void l_lic_entitlement_free (void *ementptr)
{
  l_lic_entitlement *ement = ementptr;
  free (ement);
}

