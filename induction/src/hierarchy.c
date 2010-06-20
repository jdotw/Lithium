#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "entity.h"
#include "hashtable.h"
#include "hierarchy.h"
#include "form.h"
#include "data.h"
#include "customer.h"
#include "site.h"
#include "device.h"
#include "auth.h"
#include "callback.h"

extern i_resource *global_self;

/* Struct Manipulation */

i_hierarchy* i_hierarchy_create ()
{
  i_hierarchy *hier;

  hier = (i_hierarchy *) malloc (sizeof(i_hierarchy));
  if (!hier)
  { i_printf (1, "i_hierarchy_create failed to malloc hier"); return NULL; }
  memset (hier, 0, sizeof(i_hierarchy));

  return hier;
}

void i_hierarchy_free (void *hierptr)
{
  i_hierarchy *hier = hierptr;

  if (!hier) return;

  if (hier->dev_name) free (hier->dev_name);
  if (hier->dev_desc) free (hier->dev_desc);
  if (hier->dev_addr) i_resource_free_address (hier->dev_addr);
  if (hier->device_id) free (hier->device_id);
  if (hier->device_desc) free (hier->device_desc);
  if (hier->device_addr) i_resource_free_address (hier->device_addr);
  if (hier->dev) 
  { i_entity_deregister (global_self, ENTITY(hier->dev)); i_entity_free (ENTITY(hier->dev)); }
  
  if (hier->site_name) free (hier->site_name);
  if (hier->site_desc) free (hier->site_desc);
  if (hier->site_id) free (hier->site_id);
  if (hier->site) 
  { i_entity_deregister (global_self, ENTITY(hier->site)); i_entity_free (ENTITY(hier->site)); }

  if (hier->cust_name) free (hier->cust_name);
  if (hier->cust_desc) free (hier->cust_desc);
  if (hier->cust_addr) i_resource_free_address (hier->cust_addr);
  if (hier->customer_id) free (hier->customer_id);
  if (hier->customer_desc) free (hier->customer_desc);
  if (hier->customer_addr) i_resource_free_address (hier->customer_addr);
  if (hier->cust) i_entity_free (ENTITY(hier->cust));
  
  if (hier->entity_str) free (hier->entity_str);

  free (hier);
}

/* Hierarchy Building */

i_hierarchy* i_hierarchy_build (i_resource *self, i_resource *parent)
{
  i_hierarchy *hier;

  hier = i_hierarchy_create ();
  if (!hier)
  { i_printf (1, "i_hierarchy_build failed to create hierarchy"); return NULL; }
  hier->entity_str = i_resource_address_entity_string (RES_ADDR(self));

  /* Insert our hierarchical value */
  switch (self->type)
  {
    case RES_CUSTOMER: 
      if (self->ent)
      {
        if (self->ent->name_str) hier->cust_name = strdup (self->ent->name_str); 
        if (self->ent->desc_str) hier->cust_desc = strdup (self->ent->desc_str); 
        if (self->ent->desc_str) hier->customer_desc = strdup (self->ent->desc_str); 
        if (self->ent->name_str) hier->customer_id = strdup (self->ent->name_str); 
        hier->cust = (i_customer *) self->ent;
        hier->cust->fsroot_str = strdup (self->root);
        hier->cust->resaddr = i_resource_address_duplicate (RES_ADDR(self));
      }
      hier->cust_addr = i_resource_address_duplicate (RES_ADDR(self));
      hier->customer_addr = i_resource_address_duplicate (RES_ADDR(self));
      break;
    case RES_DEVICE: 
      if (self->ent)
      {
        i_device *dev = (i_device *) self->ent;
        
        /* Customer hierarchy */
        if (dev->site && dev->site->cust)
        {
          if (dev->site->cust->name_str) hier->cust_name = strdup (dev->site->cust->name_str);
          if (dev->site->cust->desc_str) hier->cust_desc = strdup (dev->site->cust->desc_str);
          if (dev->site->cust->name_str) hier->customer_id= strdup (dev->site->cust->name_str);
          if (dev->site->cust->desc_str) hier->customer_desc = strdup (dev->site->cust->desc_str);
          hier->cust = dev->site->cust;
          hier->cust->fsroot_str = strdup (parent->root);
          hier->cust->resaddr = i_resource_address_duplicate (RES_ADDR(parent));
        }
        hier->cust_addr = i_resource_address_duplicate (RES_ADDR(parent));
        hier->customer_addr = i_resource_address_duplicate (RES_ADDR(parent));

        /* Site hierarchy */
        if (dev->site)
        {
          if (dev->site->name_str) hier->site_name = strdup (dev->site->name_str);
          if (dev->site->desc_str) hier->site_desc = strdup (dev->site->desc_str);
          if (dev->site->name_str) hier->site_id = strdup (dev->site->name_str);
          hier->site = dev->site;
          if (hier->site && !hier->site->resaddr)
          { hier->site->resaddr = i_resource_address_duplicate (RES_ADDR(parent)); }
        }
        
        /* Device hierarchy */
        if (dev->name_str) hier->dev_name = strdup (dev->name_str); 
        if (dev->name_str) hier->dev_desc = strdup (dev->desc_str);
        if (dev->name_str) hier->device_id = strdup (dev->name_str);
        if (dev->name_str) hier->device_desc = strdup (dev->desc_str);
        hier->dev = dev;
        hier->dev->resaddr = i_resource_address_duplicate (RES_ADDR(self));
      }
      hier->dev_addr = i_resource_address_duplicate (RES_ADDR(self));
      hier->device_addr = i_resource_address_duplicate (RES_ADDR(self));
      break;
  }

  /* Finished */

  return hier;
}

