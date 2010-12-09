#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#include <induction/path.h>
#include <induction/navtree.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/hierarchy.h>
#include <induction/timer.h>

#include "device.h"
#include "navtree.h"
#include "lic.h"

/* 
 * License Related Functions
 */

/* Entitlement */

static l_lic_entitlement* static_entitlement = NULL;

l_lic_entitlement* l_lic_static_entitlement ()
{ return static_entitlement; }

/* Reset */

int l_lic_reset_entitlement (i_resource *self)
{
  /* Reset license entitlement */
  int dev_delta = 0;
  l_lic_entitlement *current_entitlement = l_lic_load_entitlement (self);
  if (static_entitlement)
  {
    if (static_entitlement->devices_max != current_entitlement->devices_max)
    {
      if (current_entitlement->devices_max == 0)
      {
        dev_delta = static_entitlement->devices_excess;
      }
      else if (static_entitlement->devices_max == 0)
      {
        dev_delta = current_entitlement->devices_max - static_entitlement->devices_used;
      }
      else
      { 
        dev_delta = current_entitlement->devices_max - static_entitlement->devices_max;
      }
    }
    current_entitlement->devices_used = static_entitlement->devices_used;
    current_entitlement->devices_excess = static_entitlement->devices_excess;
    l_lic_entitlement_free (static_entitlement);
    static_entitlement = current_entitlement;
    current_entitlement = NULL;
  }
  else
  { 
    static_entitlement = current_entitlement;
    return 0;
  }

  i_printf (1, "devdelta is %i", dev_delta);

  /* 
   * Compare, take and rescind 
   */

  i_customer *cust = self->hierarchy->cust;
  i_site *site;
  if (dev_delta < 0 || (dev_delta > 0 && static_entitlement->devices_excess > 0))
  {
    /* There is either too many licenses currently in use, 
     * or there are now free licenses to be taken
     */
    for (i_list_move_head(cust->site_list); (site=i_list_restore(cust->site_list))!=NULL; i_list_move_next(cust->site_list))
    {
      i_device *dev;
      for (i_list_move_head(site->dev_list); (dev=i_list_restore(site->dev_list))!=NULL; i_list_move_next(site->dev_list))
      {
        if (dev_delta > 0 && !dev->licensed)
        {
          /* Take license */
          static_entitlement->devices_excess--;
          dev->licensed = l_lic_take (self, ENTITY(dev));
          l_device_res_restart (self, dev);
          dev_delta--;
        }
        else if (dev_delta < 0 && dev->licensed)
        {
          /* Rescind license */
          l_lic_rescind (self, ENTITY(dev));
          dev->licensed = 0;
          static_entitlement->devices_excess++;
          l_device_res_destroy (self, dev);
          dev_delta++;
        }
        if (dev_delta == 0) break;
      }
    }
  }

  return 0;
}

int l_lic_entitlement_reset_timercb (i_resource *self, i_timer *timer, void *passdata)
{
  /* Periodic license entitlement checking */
  l_lic_reset_entitlement (self);
  return 0;
}

l_lic_entitlement* l_lic_load_entitlement (i_resource *self)
{  
  /* Create entitlement */
  l_lic_entitlement *entitlement = l_lic_entitlement_create ();

  /* Load keys */
  i_list *keys = l_lic_loadkeys (self);

  /* Check for root licenses */
  l_lic_key *key;
  int root_acn_present = 0;
  int root_msp_present = 0;
  for (i_list_move_head(keys); (key=i_list_restore(keys))!=NULL; i_list_move_next(keys))
  {
    /* 
     * MSP or ACN Root License
     */

    if (key->status != KEY_VALID) continue;

    if (strcmp(key->type_str, "RMSP") == 0)
    { root_msp_present = 1; }
    if (strcmp(key->type_str, "RACN") == 0)
    { root_acn_present = 1; }
  }
  if (root_acn_present)
  {
    /* A root ACN license is present
     * License the customer for a maximum of 5 devices
     */
    entitlement->customer_licensed = 1;
    entitlement->expiry_sec = 0;
    entitlement->devices_max = 5;
    entitlement->paid = 1;
    entitlement->type_str = strdup("Lithium ACN Customer License");
    i_list_free (keys);
    return entitlement;
  }

  /* Process customer key entitlements */
  for (i_list_move_head(keys); (key=i_list_restore(keys))!=NULL; i_list_move_next(keys))
  {
    /* Process by key type 
     *
     * CSTD = Standard Customer License
     * CMSP = MSP Customer License
     * CACN = ACN Customer License
     * RMSP = MSP Root License
     * RACN = ACN Root License
     *
     */
    if (strcmp(key->type_str, "CSTD") == 0 || strcmp(key->type_str, "CMSP") == 0 || strcmp(key->type_str, "CACN") == 0)
    {
      /*
       * Customer License (CSTD/CMSP/CACN) 
       */
      
      /* Check validity */
      if (key->status != KEY_VALID)
      {
        /* Do not process an invalid key */
        if (key->flags & KEY_FLAG_DEMO && key->status == KEY_INVALID_EXPIRED)
        {
          /* An expired demo key is present, set the demo flag to 2
           * to indicate that a demo was had, but expired
           */
          entitlement->demo = 2;
        }
        continue;
      }

      /* Check for MSP Validity */
      if (strcmp(key->type_str, "CMSP") == 0 && root_msp_present == 0)
      {
        /* Invalid, no root MSP cert is present */
        asprintf (&entitlement->type_str, "Lithium MSP Root License Key Not Installed");
        continue;
      }

      /* Check for MSP Validity */
      if (strcmp(key->type_str, "CACN") == 0 && root_acn_present == 0)
      {
        /* Invalid, no root MSP cert is present */
        asprintf (&entitlement->type_str, "Lithium ACN Root License Key Not Installed");
        continue;
      }

      /* Check for limited (Lithium LE) */
      if (key->product_str && strcmp(key->product_str, "limited") == 0)
      { 
        entitlement->limited = 1; 
      }

      /* Set Entitlement info */
      entitlement->customer_licensed = 1;
      entitlement->expiry_sec = key->expiry_sec;
      if (key->flags & KEY_FLAG_DEMO && entitlement->paid == 0) 
      {
        asprintf (&entitlement->type_str, "Registered 30-Day Trial");
        entitlement->demo = 1;
        entitlement->devices_max = key->volume;
      }
      else if (key->flags & KEY_FLAG_FREE_UNREG && entitlement->paid == 0) 
      {
        asprintf (&entitlement->type_str, "Unregistered 30-Day Trial -- Maximum 3 Monitored Devices");
        entitlement->free = 1;
        entitlement->devices_max = key->volume;
      }
      else 
      { 
        /* Update device count */
        if (entitlement->paid == 0)
        {
          /* This is the first paid license, reset the device count */
          entitlement->devices_max = 0;
          entitlement->paid = 1;
        }
        entitlement->devices_max += key->volume;          /* Allow stacking of customer licenses */
          
        /* Update type */
        char *volume_str;
        if (key->volume == 0)
        { volume_str = strdup (""); }
        else
        { asprintf (&volume_str, "%lu", key->volume); }
        if (strcmp(key->type_str, "CSTD") == 0)
        { 
          if (entitlement->limited == 0)
          {
            if (key->volume == 0)
            { entitlement->type_str = strdup ("Lithium (Unlimited)"); }
            else
            { asprintf (&entitlement->type_str, "Lithium %s", volume_str); }
          }
          else
          { asprintf (&entitlement->type_str, "Lithium %s-LE", volume_str); }
        }
        if (strcmp(key->type_str, "CMSP") == 0)
        { asprintf (&entitlement->type_str, "Lithium MSP %s", volume_str); }
        if (strcmp(key->type_str, "CACN") == 0)
        { asprintf (&entitlement->type_str, "Lithium ACN %s", volume_str); }
        free (volume_str);
      }
    }
  }

  /* Cleanup */
  i_list_free (keys);

  return entitlement;
}

