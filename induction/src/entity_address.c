#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "callback.h"
#include "cement.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "entity.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * Cement Entity Addressing
 *
 * Entity addresses are expressed 
 * as a string of colon-separated unique
 * named. The string is arranged from left
 * to right in top-down hierarchical order
 * with the first entity definine the plexus
 * to which the entity begins. I.e the full 
 * length of an address would be:
 *
 * type:customer:site:device:container:object:metric:trigger
 * 
 */

i_entity_address* i_entity_address_create ()
{
  i_entity_address *addr;

  addr = (i_entity_address *) malloc (sizeof(i_entity_address));
  if (!addr)
  { i_printf (1, "i_entity_address_create failed to malloc i_entity_address struct"); return NULL; }
  memset (addr, 0, sizeof(i_entity_address));

  return addr;
}

void i_entity_address_free (void *addrptr)
{
  i_entity_address *addr = addrptr;

  if (!addr) return;

  if (addr->cust_name) free (addr->cust_name);
  if (addr->site_name) free (addr->site_name);
  if (addr->dev_name) free (addr->dev_name);
  if (addr->cnt_name) free (addr->cnt_name);
  if (addr->obj_name) free (addr->obj_name);
  if (addr->met_name) free (addr->met_name);
  if (addr->trg_name) free (addr->trg_name);

  free (addr);
}

i_entity_address* i_entity_address_duplicate (i_entity_address *entaddr)
{
  i_entity_address *dup;

  dup = i_entity_address_create ();
  if (!dup)
  { i_printf (1, "i_entity_address_duplicate failed to create dup struct"); return NULL; }

  dup->type = entaddr->type;
  if (entaddr->cust_name) dup->cust_name = strdup (entaddr->cust_name);
  if (entaddr->site_name) dup->site_name = strdup (entaddr->site_name);
  if (entaddr->dev_name) dup->dev_name = strdup (entaddr->dev_name);
  if (entaddr->cnt_name) dup->cnt_name = strdup (entaddr->cnt_name);
  if (entaddr->obj_name) dup->obj_name = strdup (entaddr->obj_name);
  if (entaddr->met_name) dup->met_name = strdup (entaddr->met_name);
  if (entaddr->trg_name) dup->trg_name = strdup (entaddr->trg_name);

  return dup;
}

/* Address string/struct manipulation */

char* i_entity_address_string (i_entity *ent, i_entity_address *entaddr)
{
  /* This function creates a string representation
   * of an entity's address. If the ent is specified, 
   * then the ent struct will be used. If the ent is 
   * NULL and an entaddr is specified, then the entaddr
   * will be used. 
   */
  char *addrstr = NULL;

  if (ent)
  {
    i_entity *curent;
    char *tempstr;

    /* Create the far-right portion of the addr
     * string from the specified ent's name_str
     */
    addrstr = strdup (ent->name_str);

    /* Iterate through hierarchy and prefix parent's
     * name_str at each level
     */
    curent = ent->parent;
    while (curent)
    {
      asprintf (&tempstr, "%s:%s", curent->name_str, addrstr);
      free (addrstr);
      addrstr = tempstr;
      curent = curent->parent;
    }

    /* Prefix the entity type */
    asprintf (&tempstr, "%i:%s", ent->ent_type, addrstr);
    free (addrstr);
    addrstr = tempstr;
  }
  else if (entaddr)
  {
    int level = 0;
    char *namestr;

    asprintf (&addrstr, "%i", entaddr->type);
    level++;

    /* Compile string */
    namestr = entaddr->cust_name;
    while (namestr)
    {
      char *tempstr;

      /* Suffix the current namestr */
      asprintf (&tempstr, "%s:%s", addrstr, namestr);
      free (addrstr);
      addrstr = tempstr;

      /* Progress to next namestr */
      level++;
      switch (level)
      {
        case 2:
          namestr = entaddr->site_name;
          break;
        case 3:
          namestr = entaddr->dev_name;
          break;
        case 4: 
          namestr = entaddr->cnt_name;
          break;
        case 5:
          namestr = entaddr->obj_name;
          break;
        case 6: 
          namestr = entaddr->met_name;
          break;
        case 7:
          namestr = entaddr->trg_name;
          break;
        default:
          namestr = NULL;
      }
    }
  }

  return addrstr;
}

i_entity_address* i_entity_address_struct (char *addrstr) 
{
  /* This function creates an i_entity_address struct 
   * from a string representation of an entity address
   */
  int level = 0;
  char *divptr;
  i_entity_address *addr;

  /* Create struct */
  addr = i_entity_address_create ();
  if (!addr)
  { i_printf (1, "i_entity_address_struct failed to create addr struct"); return NULL; }
  
  /* Create a working copy */
  addrstr = strdup (addrstr);
  if (!addrstr)
  { i_printf (1, "i_entity_address_struct failed to create working copy of addrstr"); return NULL; }

  /* Compile struct from the string */
  divptr = addrstr;
  while (divptr)
  {
    char *namestr;

    namestr = divptr;
    divptr = strchr (namestr, ':');
    if (divptr)
    { *divptr = '\0'; divptr++; }
    switch (level)
    {
      case 0: addr->type = atoi (namestr);
              break;
      case 1: addr->cust_name = strdup (namestr);
              break;
      case 2: addr->site_name = strdup (namestr);
              break;
      case 3: addr->dev_name = strdup (namestr);
              break;
      case 4: addr->cnt_name = strdup (namestr);
              break;
      case 5: addr->obj_name = strdup (namestr);
              break;
      case 6: addr->met_name = strdup (namestr);
              break;
      case 7: addr->trg_name = strdup (namestr);
              break;
    }
    level++;
  }

  /* Cleanup */
  free (addrstr);             /* Free the working copy */

  return addr;
}

i_entity_address* i_entity_address_struct_static (i_entity *ent)
{
  /* This function creates an i_entity_address struct 
   * for the specified entity. The return i_entity_address
   * struct is a static struct which must not be freed 
   */
  i_entity *curent = ent;
  static i_entity_address addr;

  /* Zero the static struct */
  memset (&addr, 0, sizeof(i_entity_address));

  /* Set type */
  addr.type = ent->ent_type;

  /* Set names */
  while (curent)
  {
    switch (curent->ent_type)
    {
      case ENT_CUSTOMER: 
        addr.cust_name = curent->name_str;
        break;
      case ENT_SITE:
        addr.site_name = curent->name_str;
        break;
      case ENT_DEVICE:
        addr.dev_name = curent->name_str;
        break;
      case ENT_CONTAINER:
        addr.cnt_name = curent->name_str;
        break;
      case ENT_OBJECT:
        addr.obj_name = curent->name_str;
        break;
      case ENT_METRIC:
        addr.met_name = curent->name_str;
        break;
      case ENT_TRIGGER:
        addr.trg_name = curent->name_str;
        break;
    }

    curent = curent->parent;
  }

  return &addr;
}

/* URL Address */

char* i_entity_address_url (i_entity *ent, i_entity_address *entaddr)
{
  /* Returns a lithium://node/cust/site/dev/cnt/obj/met/trg style URL */
  char hostname[256];
  gethostname (hostname, 255);
  char *url_str;
  asprintf (&url_str, "lithium://%s/entity", hostname);

  /* Check if an entity was specified */
  if (ent)
  {
    /* Create an entity address */
    entaddr = i_entity_address_struct_static (ent);
  }

  /* Create URL off entaddr */  
  if (entaddr)
  {
    char *tmp;
    if (entaddr->cust_name) { asprintf (&tmp, "%s/%s", url_str, entaddr->cust_name); free (url_str); url_str = tmp; }
    if (entaddr->site_name) { asprintf (&tmp, "%s/%s", url_str, entaddr->site_name); free (url_str); url_str = tmp; }
    if (entaddr->dev_name) { asprintf (&tmp, "%s/%s", url_str, entaddr->dev_name); free (url_str); url_str = tmp; }
    if (entaddr->cnt_name) { asprintf (&tmp, "%s/%s", url_str, entaddr->cnt_name); free (url_str); url_str = tmp; }
    if (entaddr->obj_name) { asprintf (&tmp, "%s/%s", url_str, entaddr->obj_name); free (url_str); url_str = tmp; }
    if (entaddr->met_name) { asprintf (&tmp, "%s/%s", url_str, entaddr->met_name); free (url_str); url_str = tmp; }
    if (entaddr->trg_name) { asprintf (&tmp, "%s/%s", url_str, entaddr->trg_name); free (url_str); url_str = tmp; }
  }

  return url_str;
}

/* @} */
