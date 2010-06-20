#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/customer.h>
#include <induction/site.h>
#include <induction/hierarchy.h>

#include "site.h"

/* 
 * Site Sorting Functions
 */

int l_site_sortfunc_suburb (void *curptr, void *nextptr)
{
  int num;
  i_site *cursite = curptr;
  i_site *nextsite = nextptr;

  if (!cursite || !cursite->suburb_str || !nextsite || !nextsite->suburb_str)
  { return 1; }

  num = strcmp(cursite->suburb_str, nextsite->suburb_str);
  if (num > 0)            /* Swap */
  { return 1; }
  else if (num == 0)      /* Exact match, sort by suburb */ 
  { return l_site_sortfunc_name (curptr, nextptr); }

  return 0;               /* Dont swap */
}

int l_site_sortfunc_name (void *curptr, void *nextptr)
{  
  int num;  
  i_site *cursite = curptr;  
  i_site *nextsite = nextptr;

  if (!cursite || !cursite->name_str || !nextsite || !nextsite->name_str)
  { return 1; }

  num = strcmp(cursite->name_str, nextsite->name_str);
  if (num > 0)            /* Swap */    
  { return 1; }
  else if (num == 0)      /* Exact match, sort by address */
  { return l_site_sortfunc_addr1 (curptr, nextptr); }

  return 0;               /* Dont swap */
}

int l_site_sortfunc_addr1 (void *curptr, void *nextptr)
{  
  int num;  
  i_site *cursite = curptr;  
  i_site *nextsite = nextptr;

  if (!cursite || !cursite->addr1_str || !nextsite || !nextsite->addr1_str)
  { return 1; }

  num = strcmp(cursite->addr1_str, nextsite->addr1_str);
  if (num > 0)            /* Swap */
  { return 1; }

  return 0;               /* Dont swap */
}

