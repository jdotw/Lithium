#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "form.h"
#include "timer.h"
#include "name.h"
#include "desc.h"
#include "hashtable.h"
#include "callback.h"
#include "cement.h"
#include "customer.h"
#include "site.h"
#include "device.h"
#include "container.h"
#include "object.h"
#include "metric.h"
#include "trigger.h"
#include "data.h"
#include "entity.h"
#include "entity_xmlsync.h"

/** \addtogroup entity Monitored Entities
 * @{
 */

/*
 * CEMent Entities
 */

extern i_resource *global_self;

/* Struct Manipulation */

i_entity* i_entity_create (char *name_str, char *desc_str, unsigned short ent_type, size_t datasize)
{
  i_entity *ent;

  ent = (i_entity *) malloc (datasize);
  if (!ent)
  { i_printf (1, "i_entity_create failed to malloc i_entity struct"); return NULL; }
  memset (ent, 0, datasize);
  if (name_str) ent->name_str = strdup (name_str);
  if (desc_str) ent->desc_str = strdup (desc_str);
  ent->ent_type = ent_type;
  ent->prio = 100;
  ent->opstate = -1;
  ent->version = time (NULL);
  uuid_clear (ent->uuid);

  /* Parse the name_str and desc_str to remove illegal chars */
  i_name_parse (ent->name_str); 
  i_desc_parse (ent->desc_str);

  return ent;
}

void i_entity_free (void *entptr)
{
  i_entity *ent = entptr;

  if (!ent) return;

  /* Free child list/ht */
  if (ent->child_list)
  {
    i_entity *child;
    void *listp;
    for (i_list_move_head(ent->child_list); (child=i_list_restore(ent->child_list))!=NULL; i_list_move_next(ent->child_list))
    { 
      listp = ent->child_list->p;
      i_entity_free (child); 
      ent->child_list->p = listp;
    }
  }
  
  /* Terminate/Clean Refresh data */
  if ((ent->refresh_state & REFSTATE_ALL) == REFSTATE_BUSY) 
  { i_entity_refresh_terminate (ent); }
  if (ent->refresh_data) i_entity_refresh_cleandata (ent);

  /* Perform entity-specified free */
  switch (ent->ent_type)
  {
    case ENT_CUSTOMER:
      i_customer_free ((i_customer *) ent);
      break;
    case ENT_SITE:
      i_site_free ((i_site *) ent);
      break;
    case ENT_DEVICE:
      i_device_free ((i_device *) ent);
      break;
    case ENT_CONTAINER:
      i_container_free ((i_container *) ent);
      break;
    case ENT_OBJECT:
      i_object_free ((i_object *) ent);
      break;
    case ENT_METRIC:
      i_metric_free ((i_metric *) ent);
      break;
    case ENT_TRIGGER:
      i_trigger_free ((i_trigger *) ent);
      break;
  }

  /* Disable XML Sync */
  if (ent->xmlsync_timer) i_entity_xmlsync_disable (global_self, ent);

  /* Free filesystem root string */
  if (ent->fsroot_str) free (ent->fsroot_str);

  /* Free refresh data */
  if (ent->refresh_timer) i_timer_remove (ent->refresh_timer);
  if (ent->refreshcb_list) 
  {
    i_callback *cb;
    for (i_list_move_head(ent->refreshcb_list); (cb=i_list_restore(ent->refreshcb_list))!=NULL; i_list_move_next(ent->refreshcb_list))
    {
      i_callback_free (cb);
    }
    i_list_free (ent->refreshcb_list);
  }

  /* Free variables */
  if (ent->name_str) free (ent->name_str);
  if (ent->desc_str) free (ent->desc_str);
  if (ent->desc_esc_str) free (ent->desc_esc_str);
  if (ent->child_list) i_list_free (ent->child_list);
  if (ent->child_ht) i_hashtable_free (ent->child_ht);
  if (ent->resaddr) i_resource_address_free (ent->resaddr);
  if (ent->config_form) i_form_free (ent->config_form);

  free (ent);
}

/* Struct/Data Conversion */

char* i_entity_data (i_entity *ent, int *datasizeptr)
{
  /* Initialise datasizeptr */
  memset (datasizeptr, 0, sizeof(int));
  
  /* Convert entity to data */
  switch (ent->ent_type)
  {
    case ENT_CUSTOMER:
      return i_customer_data ((i_customer *) ent, datasizeptr);
    case ENT_SITE:
      return i_site_data ((i_site *) ent, datasizeptr);
    case ENT_DEVICE:
      return i_device_data ((i_device *) ent, datasizeptr);
  }

  return NULL;
}

i_entity* i_entity_struct (char *data, int datasize)
{
  int offset;
  unsigned short type;

  /* Retrieve type */
  type = i_data_get_ushort (data, data, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_entity_struct failed to get entity type from data"); return NULL; }

  /* Convert to struct */
  switch (type)
  {
    case ENT_CUSTOMER:
      return (i_entity *) i_customer_struct (data, datasize);
    case ENT_SITE:
      return (i_entity *) i_site_struct (data, datasize);
    case ENT_DEVICE:
      return (i_entity *) i_device_struct (data, datasize);
  }

  return NULL;
}

/* Duplicate */

i_entity* i_entity_duplicate (i_entity *ent)
{
  int datasize;
  char *data;
  i_entity *dup;

  /* Convert to data */
  data = i_entity_data (ent, &datasize);
  if (!data)
  { i_printf (1, "i_entity_duplicate failed to convert entity to data"); return NULL; }

  /* Convert to struct */
  dup = i_entity_struct (data, datasize);
  free (data);
  if (!dup)
  { i_printf (1, "i_entity_duplicate failed to convert data to entity"); return NULL; }

  return dup;
}

/* String Utility */

char* i_entity_typestr (unsigned short type)
{
  switch (type)
  {
    case ENT_CUSTOMER: return "Customer";
    case ENT_SITE: return "Site";
    case ENT_DEVICE: return "Device";
    case ENT_CONTAINER: return "Container";
    case ENT_OBJECT: return "Object";
    case ENT_METRIC: return "Metric";
    default: return "Unknown";
  }

  return NULL;
}

char* i_entity_opstatestr (short state)
{
  switch (state)
  {
    case ENTSTATE_FAILED: return "Critical";
    case ENTSTATE_IMPAIRED: return "Impaired";
    case ENTSTATE_ATRISK: return "At Risk";
    case ENTSTATE_NORMAL: return "Normal";
    case ENTSTATE_UNKNOWN: return "Unknown";
    default: return "Unknown";
  }
  
  return NULL;
}

char* i_entity_adminstatestr (short state)
{
  switch (state & ENTADMIN_ALL)
  {
    case ENTADMIN_ENABLED: return "Enabled";
    case ENTADMIN_DISABLED: return "Disabled";
    default: return "Unknown";
  }

  return NULL;
}

/*
 * @}
 */
