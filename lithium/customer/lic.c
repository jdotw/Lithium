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
        { asprintf (&volume_str, "%i", key->volume); }
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

/* Load */

i_list* l_lic_loadkeys (i_resource *self)
{
  int num;
  int row;
  int row_count;
  PGresult *res;
  PGconn *pgconn;
  i_list *list;
  l_lic_key *key;
  l_lic_key *demo_key = NULL;

  /* Create list */
  list = i_list_create ();
  i_list_set_destructor (list, l_lic_key_free);

  /* DEBUG FIX BETA */
//  key = l_lic_key_create ();
//  key->status = KEY_VALID;
//  key->id = 0;
//  key->serial = 0;
//  key->type_str = strdup ("CSTD");
//  key->product_str = strdup ("beta");
//  key->custhash_str = strdup ("");
//  key->version = 0x00000005;
//  key->expiry_sec = 0;
//  key->volume = 0x00000000;
//  i_list_enqueue (list, key);
//  return list;
  /* END DEBUG FIX BETA */

  /* Connect to SQL */
  pgconn = i_pg_connect (self, "lithium");
  if (!pgconn)
  { i_printf (1, "l_lic_loadkeys failed to connect to SQL database"); return NULL; }

  /* Load all sites */
  res = PQexec (pgconn, "SELECT id, key FROM license_keys");
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK)
  { 
    i_printf (1, "l_site_loadkeys failed to execute SELECT query for the sites table");
    if (res) PQclear (res);
    i_pg_close (pgconn);
    return NULL;
  }

  /* Loop through each record */
  row_count = PQntuples (res);
  for (row=0; row < row_count; row++)
  {
    char *id_str;
    char *enckey_str;
    char *pub_key = "-----BEGIN PUBLIC KEY-----\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDVRyTSjUU8r3+bQHIrx5p3AZBj\r\nUpiYEmRyOToD/C0U2ML9Fw/oFAYA4M5X+XmVJ92DitEk3KXetH9zkEMqCvVmWKwP\r\nqFQnakMU4f7n8/ynDwNLexMUhPvZnhP1ubSFeHFF6TEd5Hlnh3+3e5Of092SEYSf\r\n5b4puVQqmUuisHjchwIDAQAB\r\n-----END PUBLIC KEY-----";
    BIO *bio;
    char *temp;
    char *tempptr;
    char *endptr;

    /* Fields */
    id_str = PQgetvalue (res, row, 0);
    enckey_str = PQgetvalue (res, row, 1);

    /* Create key */
    key = l_lic_key_create ();
    key->id = atol (id_str);
    key->enc_str = strdup (enckey_str);
    i_list_enqueue (list, key);

    /* Decode key */
    if((bio = BIO_new_mem_buf(pub_key, strlen(pub_key)+1)))
    {
      RSA* rsa_key = 0;
      if(PEM_read_bio_RSA_PUBKEY(bio, &rsa_key, NULL, NULL))
      {
        unsigned char dst[strlen(key->enc_str)+1];
        unsigned char *src = (unsigned char *) strdup (key->enc_str);
        int src_size = b64decode(src);
        num = RSA_public_decrypt(src_size, src, dst, rsa_key, RSA_PKCS1_PADDING);
        if (num != -1)
        {
          dst[num] = '\0';
          key->raw_str = strdup ((char *)dst);
          key->status = KEY_DECODED;    /* Key decoded */
        }
        else
        {
          char buf[1024];
          ERR_error_string(ERR_get_error(), buf);
          i_printf (1, "l_lic_loadkeys error decoding key (%i) %s", num, buf); 
          key->status = KEY_INVALID;    /* Invalid key */
          continue;
        }
        RSA_free(rsa_key);
      }
      else
      { 
          i_printf (1, "l_lic_loadkeys error loading pub key"); 
          key->status = KEY_INVALID;    /* Invalid key */
          continue;
      }
      BIO_free(bio);
    }
    else
    {
      i_printf (1, "l_lic_loadkeys error allocating decoder memory"); 
      key->status = KEY_INVALID; 
      continue; 
    }

    /* 
     * Iterpret the key
     */
    temp = strdup (key->raw_str);
    tempptr = temp;

    /* License serial num */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { i_printf (0, "l_lic_loadkeys key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; continue; }
    *endptr = '\0';
    key->serial = atol (tempptr);
    tempptr = endptr+1;
    
    /* License type */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { i_printf (0, "l_lic_loadkeys key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; continue; }
    *endptr = '\0';
    key->type_str = strdup (tempptr);
    tempptr = endptr+1;

    /* Customer hash */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { i_printf (0, "l_lic_loadkeys key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; continue; }
    *endptr = '\0';
    key->custhash_str = strdup (tempptr);
    tempptr = endptr+1;

    /* Product ID */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { i_printf (0, "l_lic_loadkeys key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; continue; }
    *endptr = '\0';
    key->product_str = strdup (tempptr);
    tempptr = endptr+1;
    if (strstr(key->product_str, "demo"))
    {
      if (!demo_key)
      { demo_key = key; }
      else
      { key->status = KEY_INVALID_DUPLICATE; continue; }
    }

    /* Version */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { i_printf (0, "l_lic_loadkeys key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; continue; }
    *endptr = '\0';
    key->version = strtol (tempptr, NULL, 16);
    tempptr = endptr+1;

    /* Volume */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { i_printf (0, "l_lic_loadkeys key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; continue; }
    *endptr = '\0';
    key->volume = strtoul (tempptr, NULL, 16);
    tempptr = endptr+1;

    /* Flags */
    endptr = strchr(tempptr, ':');
    if (!endptr)
    { i_printf (0, "l_lic_loadkeys key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; continue; }
    *endptr = '\0';
    key->flags = (unsigned int) strtoul (tempptr, NULL, 16);
    tempptr = endptr+1;

    /* Expiry */
    key->expiry_sec = (unsigned int) strtoul (tempptr, NULL, 16);;

    free (temp);

    /*
     * Validate the license
     */

    /* Check customer hash */
    if (self->hierarchy->cust && strcmp(key->type_str, "RACN") != 0 && strcmp(key->type_str, "RMSP") != 0)
    {
      /* License is customer-specific, check the hash */
      unsigned char custhash_raw[SHA_DIGEST_LENGTH];
      char *custhash;
      SHA1 ((const unsigned char *)self->hierarchy->cust->name_str, strlen(self->hierarchy->cust->name_str), custhash_raw);
      asprintf (&custhash, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", custhash_raw[0], custhash_raw[1], custhash_raw[2], custhash_raw[3], 
        custhash_raw[4], custhash_raw[5], custhash_raw[6], custhash_raw[7], custhash_raw[8], custhash_raw[9], custhash_raw[10], 
        custhash_raw[11], custhash_raw[12], custhash_raw[13], custhash_raw[14], custhash_raw[15], custhash_raw[16], custhash_raw[17], 
        custhash_raw[18], custhash_raw[19]);
      if (strcmp(custhash, key->custhash_str)!=0)
      {
        /* Customer mismatch */
        key->status = KEY_INVALID_CUSTOMER;
        continue;
      }
    }

    /* Check for duplicates and exclusivity */
    l_lic_key *existing_key;
    for (i_list_move_head(list); (existing_key=i_list_restore(list))!=NULL; i_list_move_next(list))
    {
      if (existing_key->status != KEY_VALID) continue;

      if (existing_key->serial == key->serial && existing_key != key)
      { 
        /* Dup found */
        key->status = KEY_INVALID_DUPLICATE;
        break;
      }
    }
    if (key->status == KEY_INVALID_DUPLICATE || key->status == KEY_INVALID_DUPLICATE_EXCLUSIVE)
    { continue; }

    /* Check for expiry */
    if (key->expiry_sec > 0 && key->expiry_sec < time(NULL))
    {
      /* Key has expired */
      key->status = KEY_INVALID_EXPIRED;
      continue;
    }

    /* If control reaches here, the key is valid */
    key->status = KEY_VALID;
  }

  /* Check to see if demo license should be removed */
  if (list && list->size > 1 && demo_key)
  {
    if (i_list_search(list, demo_key) == 0)
    { i_list_delete (list); }
    demo_key = NULL;
  }

  /* Check to see if demo license is active */
  if (demo_key && demo_key->status == KEY_VALID)
  {
    /*
     * We're in demo mode, verify it 
     */
    struct timeval now;
    gettimeofday (&now, NULL);

    /* Check age of 'install' dir */
    char *path = i_path_glue (self->root, "install");
    struct stat sb;
    num = stat (path, &sb);
    free (path);
    if (num != 0)
    { demo_key->status = KEY_INVALID_EXPIRED; }
    else if ((sb.st_mtime + (30*86400)) <= now.tv_sec)
    { demo_key->status = KEY_INVALID_EXPIRED; }

    /* Check age of the 'client_handler' dir */
    path = i_path_glue (self->root, "../client_handler");
    num = stat (path, &sb);
    free (path);
    if (num != 0)
    { demo_key->status = KEY_INVALID_EXPIRED; }
    else if ((sb.st_mtime + (30*86400)) <= now.tv_sec)
    { demo_key->status = KEY_INVALID_EXPIRED; }

    /* If key is still valid, set expiry */
    demo_key->expiry_sec = sb.st_mtime + (30*86400);
  }

  if ((list && list->size == 0) || (demo_key && demo_key->status == KEY_INVALID_EXPIRED))
  {
    /* If there's no license keys and/or the demo
     * license key has expired, install a 'FREE' key
     * for monitoring 3 devices
     */

    int past_30days = 0;
    struct timeval now;
    gettimeofday (&now, NULL);

    key = l_lic_key_create ();
    key->status = KEY_VALID;
    key->id = 0;
    key->serial = 0;
    key->type_str = strdup ("CSTD");
    key->product_str = strdup ("free");
    key->custhash_str = strdup ("");
    key->version = 0x00000005;
    key->expiry_sec = 0;
    key->volume = 0x00000003;
    i_list_enqueue (list, key);

    /* Check age of 'install' dir */
    char *path = i_path_glue (self->root, "install");
    struct stat sb;
    num = stat (path, &sb);
    free (path);
    if (num != 0)
    { past_30days = 1; }
    else if ((sb.st_mtime + (30*86400)) <= now.tv_sec)
    { past_30days = 1; }

    /* Check age of the 'client_handler' dir */
    path = i_path_glue (self->root, "../client_handler");
    num = stat (path, &sb);
    free (path);
    if (num != 0)
    { past_30days = 1; }
    else if ((sb.st_mtime + (30*86400)) <= now.tv_sec)
    { past_30days = 1; }

    /* Check status */
    if (past_30days == 1 || (demo_key && demo_key->status == KEY_INVALID_EXPIRED))
    {
      key->enc_str = strdup ("Post-Trial Free License");
      key->raw_str = strdup ("Post-Trial Free License");
      key->flags = KEY_FLAG_FREE_EXPIRED;
    }
    else
    {
      key->enc_str = strdup ("Unregistered Trial License");
      key->raw_str = strdup ("Unregistered Trial License");
      key->flags = KEY_FLAG_FREE_UNREG;
      key->expiry_sec = sb.st_mtime + (30*86400);
    }
  }

  /* Close DB */
  if (res) PQclear (res);
  i_pg_close (pgconn);

  return list;
}

l_lic_key* l_lic_validate_key (i_resource *self, char *enckey_str)
{
  int num;
  char *pub_key = "-----BEGIN PUBLIC KEY-----\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDVRyTSjUU8r3+bQHIrx5p3AZBj\r\nUpiYEmRyOToD/C0U2ML9Fw/oFAYA4M5X+XmVJ92DitEk3KXetH9zkEMqCvVmWKwP\r\nqFQnakMU4f7n8/ynDwNLexMUhPvZnhP1ubSFeHFF6TEd5Hlnh3+3e5Of092SEYSf\r\n5b4puVQqmUuisHjchwIDAQAB\r\n-----END PUBLIC KEY-----";
  BIO *bio;
  char *temp;
  char *tempptr;
  char *endptr;
  l_lic_key *key;

  /* Create key */
  key = l_lic_key_create ();
  key->enc_str = strdup (enckey_str);

  /* Decode key */
  if((bio = BIO_new_mem_buf(pub_key, strlen(pub_key)+1)))
  {
    RSA* rsa_key = 0;
    if(PEM_read_bio_RSA_PUBKEY(bio, &rsa_key, NULL, NULL))
    {
      unsigned char dst[strlen(key->enc_str)+1];
      unsigned char *src = (unsigned char *) strdup (key->enc_str);
      int src_size = b64decode(src);
      num = RSA_public_decrypt(src_size, src, dst, rsa_key, RSA_PKCS1_PADDING);
      if (num != -1)
      {
        dst[num] = '\0';
        key->raw_str = strdup ((char *)dst);
        key->status = KEY_DECODED;    /* Key decoded */
      }
      else
      {
        char buf[1024];
        ERR_error_string(ERR_get_error(), buf);
        i_printf (1, "l_lic_validate_key error decoding key (%i) %s", num, buf); 
        key->status = KEY_INVALID;    /* Invalid key */
        return key;
      }
      RSA_free(rsa_key);
    }
    else
    { 
      i_printf (1, "l_lic_validate_key error loading pub key"); 
      key->status = KEY_INVALID;    /* Invalid key */
      return key;
    }
    BIO_free(bio);
  }
  else
  {
    i_printf (1, "l_lic_validate_key error allocating decoder memory"); 
    key->status = KEY_INVALID; 
    return key; 
  }

  /* 
   * Iterpret the key
   */
  temp = strdup (key->raw_str);
  tempptr = temp;

  /* License serial num */
  endptr = strchr(tempptr, ':');
  if (!endptr)
  { i_printf (0, "l_lic_validate_key key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; return key; }
  *endptr = '\0';
  key->serial = strtol (tempptr, NULL, 16);
  tempptr = endptr+1;
    
  /* License type */
  endptr = strchr(tempptr, ':');
  if (!endptr)
  { i_printf (0, "l_lic_validate_key key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; return key; }
  *endptr = '\0';
  key->type_str = strdup (tempptr);
  tempptr = endptr+1;

  /* Customer hash */
  endptr = strchr(tempptr, ':');
  if (!endptr)
  { i_printf (0, "l_lic_validate_key key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; return key; }
  *endptr = '\0';
  key->custhash_str = strdup (tempptr);
  tempptr = endptr+1;

  /* Product ID */
  endptr = strchr(tempptr, ':');
  if (!endptr)
  { i_printf (0, "l_lic_validate_key key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; return key; }
  *endptr = '\0';
  key->product_str = strdup (tempptr);
  tempptr = endptr+1;

  /* Version */
  endptr = strchr(tempptr, ':');
  if (!endptr)
  { i_printf (0, "l_lic_validate_key key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; return key; }
  *endptr = '\0';
  key->version = strtol (tempptr, NULL, 16);
  tempptr = endptr+1;

  /* Volume */
  endptr = strchr(tempptr, ':');
  if (!endptr)
  { i_printf (0, "l_lic_validate_key key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; return key; }
  *endptr = '\0';
  key->volume = strtoul (tempptr, NULL, 16);
  tempptr = endptr+1;

  /* Flags */
  endptr = strchr(tempptr, ':');
  if (!endptr)
  { i_printf (0, "l_lic_validate_key key format invalid (%s)", key->raw_str); key->status = KEY_INVALID; return key; }
  *endptr = '\0';
  key->flags = (unsigned int) strtoul (tempptr, NULL, 16);
  tempptr = endptr+1;

  /* Expiry */
  key->expiry_sec = (unsigned int) strtoul (tempptr, NULL, 16);

  free (temp);

  /*
   * Validate the license
   */

  /* Check license type */
  if (key->type_str && strcmp(key->type_str, "RACN")!=0 && strcmp(key->type_str, "RMSP")!=0)
  {
    /* Check customer hash */
    unsigned char custhash_raw[SHA_DIGEST_LENGTH];
    char *custhash;
    SHA1 ((const unsigned char *)self->hierarchy->cust->name_str, strlen(self->hierarchy->cust->name_str), custhash_raw);
    asprintf (&custhash, "%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x", custhash_raw[0], custhash_raw[1], custhash_raw[2], custhash_raw[3], 
      custhash_raw[4], custhash_raw[5], custhash_raw[6], custhash_raw[7], custhash_raw[8], custhash_raw[9], custhash_raw[10], 
      custhash_raw[11], custhash_raw[12], custhash_raw[13], custhash_raw[14], custhash_raw[15], custhash_raw[16], custhash_raw[17], 
      custhash_raw[18], custhash_raw[19]);
    if (strcmp(custhash, key->custhash_str)!=0)
    {
      /* Customer mismatch */
      key->status = KEY_INVALID_CUSTOMER;
      return key;
    }
  }

  /* Check for expiry */
  if (key->expiry_sec > 0 && key->expiry_sec < time(NULL))
  {
    /* Key has expired */
    key->status = KEY_INVALID_EXPIRED;
    return key;
  }

  /* Check version */
  if (key->version != 5) 
  {
    key->status = KEY_INVALID_EXPIRED;
    return key;
  }

  /* If control reaches here, the key is valid */
  key->status = KEY_VALID;

  return key;
}

int b64decode(unsigned char* str)
{
    unsigned char *cur, *start;
    int d, dlast, phase;
    unsigned char c;
    static int table[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
    };

    d = dlast = phase = 0;
    start = str;
    for (cur = str; *cur != '\0'; ++cur )
    {
        if(*cur == '\n' || *cur == '\r'){phase = dlast = 0; continue;}
        d = table[(int)*cur];
        if(d != -1)
        {
            switch(phase)
            {
            case 0:
                ++phase;
                break;
            case 1:
                c = ((dlast << 2) | ((d & 0x30) >> 4));
                *str++ = c;
                ++phase;
                break;
            case 2:
                c = (((dlast & 0xf) << 4) | ((d & 0x3c) >> 2));
                *str++ = c;
                ++phase;
                break;
            case 3:
                c = (((dlast & 0x03 ) << 6) | d);
                *str++ = c;
                phase = 0;
                break;
            }
            dlast = d;
        }
    }
    *str = '\0';
    return str - start;
}

