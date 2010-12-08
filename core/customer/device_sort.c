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
#include <induction/device.h>
#include <induction/hierarchy.h>

#include "device.h"

/* 
 * Device Sorting Functions
 */

int l_device_sortfunc_desc (void *curptr, void *nextptr)
{
  int num;
  i_device *curdev = curptr;
  i_device *nextdev = nextptr;

  if (!curdev || !curdev->desc_str || !nextdev || !nextdev->desc_str)
  { return 1; }

  num = strcmp(curdev->desc_str, nextdev->desc_str);
  if (num > 0)            /* Swap */
  { return 1; }
  else if (num == 0)      /* Exact match, sort by suburb */ 
  { return l_device_sortfunc_name (curptr, nextptr); }

  return 0;               /* Dont swap */
}

int l_device_sortfunc_name (void *curptr, void *nextptr)
{  
  int num;  
  i_device *curdev = curptr;  
  i_device *nextdev = nextptr;

  if (!curdev || !curdev->name_str || !nextdev || !nextdev->name_str)
  { return 1; }

  num = strcmp(curdev->name_str, nextdev->name_str);
  if (num > 0)            /* Swap */    
  { return 1; }
  else if (num == 0)      /* Exact match, sort by address */
  { return l_device_sortfunc_ip (curptr, nextptr); }

  return 0;               /* Dont swap */
}

int l_device_sortfunc_ip (void *curptr, void *nextptr)
{  
  int num;  
  i_device *curdev = curptr;  
  i_device *nextdev = nextptr;

  if (!curdev || !curdev->ip_str || !nextdev || !nextdev->ip_str)
  { return 1; }

  num = strcmp(curdev->ip_str, nextdev->ip_str);
  if (num > 0)            /* Swap */
  { return 1; }

  return 0;               /* Dont swap */
}

