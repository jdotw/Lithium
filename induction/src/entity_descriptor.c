#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "timer.h"
#include "callback.h"
#include "data.h"
#include "cement.h"
#include "site.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "hierarchy.h"
#include "customer.h"
#include "entity.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * Cement Entity Descriptor
 *
 * An entity descriptor is a struct that
 * contains the full hierarchy of name and desc
 * strings for the entity itself and each 
 * parent recursively. 
 * 
 */

extern i_resource *global_self;

i_entity_descriptor* i_entity_descriptor_create ()
{
  i_entity_descriptor *desc;

  desc = (i_entity_descriptor *) malloc (sizeof(i_entity_descriptor));
  if (!desc)
  { i_printf (1, "i_entity_descriptor_create failed to malloc i_entity_descriptor struct"); return NULL; }
  memset (desc, 0, sizeof(i_entity_descriptor));

  return desc;
}

void i_entity_descriptor_free (void *descptr)
{
  i_entity_descriptor *desc = descptr;

  if (!desc) return;

  if (desc->cust_name) free (desc->cust_name);
  if (desc->site_name) free (desc->site_name);
  if (desc->dev_name) free (desc->dev_name);
  if (desc->cnt_name) free (desc->cnt_name);
  if (desc->obj_name) free (desc->obj_name);
  if (desc->met_name) free (desc->met_name);
  if (desc->trg_name) free (desc->trg_name);

  if (desc->cust_desc) free (desc->cust_desc);
  if (desc->site_desc) free (desc->site_desc);
  if (desc->dev_desc) free (desc->dev_desc);
  if (desc->cnt_desc) free (desc->cnt_desc);
  if (desc->obj_desc) free (desc->obj_desc);
  if (desc->met_desc) free (desc->met_desc);
  if (desc->trg_desc) free (desc->trg_desc);

  if (desc->cust_resaddr) i_resource_address_free (desc->cust_resaddr);
  if (desc->dev_resaddr) i_resource_address_free (desc->dev_resaddr);
  
  if (desc->name_str) free (desc->name_str);
  if (desc->desc_str) free (desc->desc_str);
  if (desc->resaddr) i_resource_address_free (desc->resaddr);

  if (desc->site_suburb) free (desc->site_suburb);
  
  free (desc);
}

i_entity_descriptor* i_entity_descriptor_duplicate (i_entity_descriptor *entdesc)
{
  i_entity_descriptor *dup;

  dup = i_entity_descriptor_create ();
  if (!dup)
  { i_printf (1, "i_entity_descriptor_duplicate failed to create dup struct"); return NULL; }

  dup->type = entdesc->type;
  
  if (entdesc->cust_name) dup->cust_name = strdup (entdesc->cust_name);
  if (entdesc->site_name) dup->site_name = strdup (entdesc->site_name);
  if (entdesc->dev_name) dup->dev_name = strdup (entdesc->dev_name);
  if (entdesc->cnt_name) dup->cnt_name = strdup (entdesc->cnt_name);
  if (entdesc->obj_name) dup->obj_name = strdup (entdesc->obj_name);
  if (entdesc->met_name) dup->met_name = strdup (entdesc->met_name);
  if (entdesc->trg_name) dup->trg_name = strdup (entdesc->trg_name);
  
  if (entdesc->cust_desc) dup->cust_desc = strdup (entdesc->cust_desc);
  if (entdesc->site_desc) dup->site_desc = strdup (entdesc->site_desc);
  if (entdesc->dev_desc) dup->dev_desc = strdup (entdesc->dev_desc);
  if (entdesc->cnt_desc) dup->cnt_desc = strdup (entdesc->cnt_desc);
  if (entdesc->obj_desc) dup->obj_desc = strdup (entdesc->obj_desc);
  if (entdesc->met_desc) dup->met_desc = strdup (entdesc->met_desc);
  if (entdesc->trg_desc) dup->trg_desc = strdup (entdesc->trg_desc);

  if (entdesc->cust_resaddr) dup->cust_resaddr = i_resource_address_duplicate (entdesc->cust_resaddr);
  if (entdesc->dev_resaddr) dup->dev_resaddr = i_resource_address_duplicate (entdesc->dev_resaddr);

  if (entdesc->name_str) dup->name_str = strdup (entdesc->name_str);
  if (entdesc->desc_str) dup->desc_str = strdup (entdesc->desc_str);
  if (entdesc->resaddr) dup->resaddr = i_resource_address_duplicate (entdesc->resaddr);

  if (entdesc->site_suburb) dup->site_suburb = strdup (entdesc->site_suburb);

  dup->adminstate = entdesc->adminstate;
  dup->opstate = entdesc->opstate;

  dup->cust_adminstate = entdesc->cust_adminstate;
  dup->cust_opstate = entdesc->cust_opstate;
  dup->site_adminstate = entdesc->site_adminstate;
  dup->site_opstate = entdesc->site_opstate;
  dup->dev_adminstate = entdesc->dev_adminstate;
  dup->dev_opstate = entdesc->dev_opstate;
  dup->cnt_adminstate = entdesc->cnt_adminstate;
  dup->cnt_opstate = entdesc->cnt_opstate;
  dup->obj_adminstate = entdesc->obj_adminstate;
  dup->obj_opstate = entdesc->obj_opstate;
  dup->met_adminstate = entdesc->met_adminstate;
  dup->met_opstate = entdesc->met_opstate;
  dup->trg_adminstate = entdesc->trg_adminstate;
  dup->trg_opstate = entdesc->trg_opstate;

  return dup;
}

/* Conversion Functions */

char* i_entity_descriptor_data (i_entity_descriptor *desc, i_entity *ent, int *datasizeptr)
{
  /* This struct creates an entity descriptor 
   * data block which can be converted to an
   * entity descriptor struct using 
   * i_entity_descriptor_struct
   */
  
  int datasize;
  char *data;
  char *dataptr;
  char *resaddrstr = NULL;
  char *cust_resaddrstr = NULL;
  char *dev_resaddrstr = NULL;

  /* Init datasizeptr */
  memset (datasizeptr, 0, sizeof(int));

  if (ent)
  {
    /* Create static descriptor struct */
    desc = i_entity_descriptor_struct_static (ent);
    if (!desc)
    { 
      i_printf (1, "i_entity_descriptor_data failed to retrieve static descriptor struct for %s %s",
        i_entity_typestr(ent->ent_type), ent->name_str); 
      return NULL;
    }
  }

  /* Convert resaddr */
  if (desc->cust_resaddr) cust_resaddrstr = i_resource_address_struct_to_string (desc->cust_resaddr);
  if (desc->dev_resaddr) dev_resaddrstr = i_resource_address_struct_to_string (desc->dev_resaddr);
  if (desc->resaddr) resaddrstr = i_resource_address_struct_to_string (desc->resaddr);

  /* Calculate datasize */
  datasize = (17*sizeof(unsigned short)) + (20*sizeof(int));
  if (desc->cust_name) datasize += strlen (desc->cust_name)+1;
  if (desc->cust_desc) datasize += strlen (desc->cust_desc)+1;
  if (desc->site_name) datasize += strlen (desc->site_name)+1;
  if (desc->site_desc) datasize += strlen (desc->site_desc)+1;
  if (desc->dev_name) datasize += strlen (desc->dev_name)+1;
  if (desc->dev_desc) datasize += strlen (desc->dev_desc)+1;
  if (desc->cnt_name) datasize += strlen (desc->cnt_name)+1;
  if (desc->cnt_desc) datasize += strlen (desc->cnt_desc)+1;
  if (desc->obj_name) datasize += strlen (desc->obj_name)+1;
  if (desc->obj_desc) datasize += strlen (desc->obj_desc)+1;
  if (desc->met_name) datasize += strlen (desc->met_name)+1;
  if (desc->met_desc) datasize += strlen (desc->met_desc)+1;
  if (desc->trg_name) datasize += strlen (desc->trg_name)+1;
  if (desc->trg_desc) datasize += strlen (desc->trg_desc)+1;
  if (desc->name_str) datasize += strlen (desc->name_str)+1;
  if (desc->desc_str) datasize += strlen (desc->desc_str)+1;
  if (desc->site_suburb) datasize += strlen (desc->site_suburb)+1;
  if (cust_resaddrstr) datasize += strlen (cust_resaddrstr)+1;
  if (dev_resaddrstr) datasize += strlen (dev_resaddrstr)+1;
  if (resaddrstr) datasize += strlen (resaddrstr)+1;

  /* Malloc data */
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_entity_descriptor_data failed to malloc data (%i bytes)", datasize);  return NULL; }
  dataptr = data;

  /* Add type */
  dataptr = i_data_add_ushort (data, dataptr, datasize, &desc->type);
  if (!dataptr) 
  { i_printf (1, "i_entity_descriptor_data failed to add ent_type to data"); free (data); return NULL; }

  /* 
   * Add Data 
   */

  /* Customer */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->cust_name);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->cust_desc);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->cust_adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->cust_opstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, cust_resaddrstr);
  if (!dataptr) { free (data); return NULL; }
  if (resaddrstr) free (cust_resaddrstr);

  /* Site */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->site_name);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->site_desc);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->site_adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->site_opstate);
  if (!dataptr) { free (data); return NULL; }

  /* Device */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->dev_name);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->dev_desc);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->dev_adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->dev_opstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, dev_resaddrstr);
  if (!dataptr) { free (data); return NULL; }
  if (dev_resaddrstr) free (dev_resaddrstr);

  /* Container */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->cnt_name);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->cnt_desc);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->cnt_adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->cnt_opstate);
  if (!dataptr) { free (data); return NULL; }

  /* Object */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->obj_name);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->obj_desc);
  if (!dataptr) { free (data); return NULL; } 
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->obj_adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->obj_opstate);
  if (!dataptr) { free (data); return NULL; }

  /* Metric */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->met_name);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->met_desc);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->met_adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->met_opstate);
  if (!dataptr) { free (data); return NULL; }

  /* Trigger */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->trg_name);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->trg_desc);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->trg_adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->trg_opstate);
  if (!dataptr) { free (data); return NULL; }
  
  /* Local */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->name_str);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, desc->desc_str);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->adminstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_short (data, dataptr, datasize, &desc->opstate);
  if (!dataptr) { free (data); return NULL; }
  dataptr = i_data_add_string (data, dataptr, datasize, resaddrstr);
  if (!dataptr) { free (data); return NULL; }
  if (resaddrstr) free (resaddrstr);

  /* Misc */
  dataptr = i_data_add_string (data, dataptr, datasize, desc->site_suburb);
  if (!dataptr) { free (data); return NULL; }

  /* Set datasizeptr */
  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_entity_descriptor* i_entity_descriptor_struct (char *data, int datasize)
{
  /* This function creates an i_entity_descriptor struct
   * from the data block provided tha was created by 
   * the i_entity_descriptor_data func
   */

  int offset=0;
  char *dataptr = data;
  char *cust_resaddrstr;
  char *dev_resaddrstr;
  char *resaddrstr;
  i_entity_descriptor *desc;

  /* Create desc */
  desc = i_entity_descriptor_create ();
  if (!desc)
  { i_printf (1, "i_entity_descriptor_struct failed to creae i_entity_descriptor struct"); return NULL; }

  /* Get type */
  desc->type = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;

  /* 
   * Get Name/Desc Pairs 
   */

  desc->cust_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->cust_desc = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->cust_adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->cust_opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  cust_resaddrstr = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  if (cust_resaddrstr)
  { desc->cust_resaddr = i_resource_address_string_to_struct (cust_resaddrstr); free (cust_resaddrstr); }

  desc->site_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->site_desc = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->site_adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->site_opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;

  desc->dev_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->dev_desc = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->dev_adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->dev_opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  dev_resaddrstr = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  if (dev_resaddrstr)
  { desc->dev_resaddr = i_resource_address_string_to_struct (dev_resaddrstr); free (dev_resaddrstr); }

  desc->cnt_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->cnt_desc = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->cnt_adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->cnt_opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;

  desc->obj_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->obj_desc = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->obj_adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->obj_opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;

  desc->met_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->met_desc = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->met_adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->met_opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;

  desc->trg_name = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->trg_desc = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->trg_adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->trg_opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;

  desc->name_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->desc_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->adminstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  desc->opstate = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  resaddrstr = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;
  if (resaddrstr)
  { desc->resaddr = i_resource_address_string_to_struct (resaddrstr); free (resaddrstr); }

  desc->site_suburb = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) { i_entity_descriptor_free (desc); return NULL; }
  dataptr += offset;

  return desc;
}

i_entity_descriptor* i_entity_descriptor_struct_static (i_entity *ent)
{
  /* This function creates an i_entity_descriptor struct 
   * for the specified entity. The return i_entity_descriptor
   * struct is a static struct which must not be freed 
   */
  i_entity *curent = ent;
  static i_entity_descriptor desc;

  /* Zero the static struct */
  memset (&desc, 0, sizeof(i_entity_descriptor));

  /* Set local values*/
  desc.type = ent->ent_type;
  desc.name_str = ent->name_str;
  desc.desc_str = ent->desc_str;
  desc.opstate = ent->opstate;
  desc.adminstate = ent->adminstate;
  if (ent->resaddr)
  { desc.resaddr = ent->resaddr; }
  else
  { desc.resaddr = RES_ADDR(global_self); }

  /* Set entity values */
  while (curent)
  {
    switch (curent->ent_type)
    {
      case ENT_CUSTOMER:
        desc.cust_name = curent->name_str;
        desc.cust_desc = curent->desc_str;
        desc.cust_adminstate = curent->adminstate;
        desc.cust_opstate = curent->opstate;
        if (curent->resaddr)
        { desc.cust_resaddr = ent->resaddr; }
        else
        { desc.cust_resaddr = RES_ADDR(global_self); }
        break;
      case ENT_SITE:
        desc.site_name = curent->name_str;
        desc.site_desc = curent->desc_str;
        desc.site_adminstate = curent->adminstate;
        desc.site_opstate = curent->opstate;
        desc.site_suburb = ((i_site *) curent)->suburb_str;
        break;
      case ENT_DEVICE:
        desc.dev_name = curent->name_str;
        desc.dev_desc = curent->desc_str;
        desc.dev_adminstate = curent->adminstate;
        desc.dev_opstate = curent->opstate;
        if (curent->resaddr)
        { desc.dev_resaddr = ent->resaddr; }
        else
        { desc.dev_resaddr = RES_ADDR(global_self); }
        break;
      case ENT_CONTAINER:
        desc.cnt_name = curent->name_str;
        desc.cnt_desc = curent->desc_str;
        desc.cnt_adminstate = curent->adminstate;
        desc.cnt_opstate = curent->opstate;
        break;
      case ENT_OBJECT:
        desc.obj_name = curent->name_str;
        desc.obj_desc = curent->desc_str;
        desc.obj_adminstate = curent->adminstate;
        desc.obj_opstate = curent->opstate;
        break;
      case ENT_METRIC:
        desc.met_name = curent->name_str;
        desc.met_desc = curent->desc_str;
        desc.met_adminstate = curent->adminstate;
        desc.met_opstate = curent->opstate;
        break;
      case ENT_TRIGGER:
        desc.trg_name = curent->name_str;
        desc.trg_desc = curent->desc_str;
        desc.trg_adminstate = curent->adminstate;
        desc.trg_opstate = curent->opstate;
        break;
    }

    curent = curent->parent;
  }

  return &desc;
}

/* @} */
