#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/hashtable.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/msgproc.h>
#include <induction/postgresql.h>
#include <induction/list.h>
#include <induction/hierarchy.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/inventory.h>
#include <induction/site.h>
#include <induction/device.h>

#include "inventory.h"

/* Basic Sorts */

int l_inventory_listsort_vendor_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (strcmp(cur->vendor_str, next->vendor_str) > 0) return 1;

  return 0;
}

int l_inventory_listsort_product_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (strcmp(cur->product_str, next->product_str) > 0) return 1;

  return 0;
}

int l_inventory_listsort_version_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (strcmp(cur->version_str, next->version_str) > 0) return 1;

  return 0;
}

int l_inventory_listsort_serial_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (strcmp(cur->serial_str, next->serial_str) > 0) return 1;

  return 0;
}

int l_inventory_listsort_devicedesc_func (void *curptr, void *nextptr)
{
  i_device *cur = curptr;
  i_device *next = nextptr;

  if (strcmp(cur->desc_str, next->desc_str) > 0) return 1;

  return 0;
}

int l_inventory_listsort_sitedesc_func (void *curptr, void *nextptr)
{
  i_site *cur = curptr;
  i_site *next = nextptr;

  if (strcmp(cur->desc_str, next->desc_str) > 0) return 1;

  return 0;
}

int l_inventory_listsort_type_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (cur->type > next->type) return 1;

  return 0;
}

/* Multipart Sorts */

int l_inventory_listsort_typevendor_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (cur->type == next->type)
  { return l_inventory_listsort_vendor_func (curptr, nextptr); }
  else
  { return l_inventory_listsort_type_func (curptr, nextptr); }

  return 0;
}

int l_inventory_listsort_typeproduct_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (cur->type == next->type)
  { return l_inventory_listsort_product_func (curptr, nextptr); }
  else
  { return l_inventory_listsort_type_func (curptr, nextptr); }

  return 0;
}

int l_inventory_listsort_typeverser_func (void *curptr, void *nextptr)
{
  i_inventory_item *cur = curptr;
  i_inventory_item *next = nextptr;

  if (cur->type == next->type)
  {
    /* types match, sort on either version or serial */
    if (cur->type & INV_HARDWARE)
    { return l_inventory_listsort_serial_func (curptr, nextptr); }
    else if (cur->type & INV_SOFTWARE)
    { return l_inventory_listsort_version_func (curptr, nextptr); }
    else
    { return 0; }
  }
  else
  {
    /* Diff types, sort by type */
    if (cur->type > next->type) return 1;
  }

  return 0;
}

