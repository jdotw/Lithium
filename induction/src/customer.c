#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "customer.h"
#include "entity.h"
#include "data.h"
#include "form.h"

/** \addtogroup customer Customers
 * @ingroup entity
 * @{
 */

/* Struct Manipulation */

i_customer* i_customer_create (char *name_str, char *desc_str, char *baseurl_str)
{
  i_customer *cust;

  cust = (i_customer *) i_entity_create (name_str, desc_str, ENT_CUSTOMER, sizeof(i_customer));
  if (!cust)
  { i_printf (1, "i_customer_create failed to create customer entity"); return NULL; }
  cust->registered = 1;
  if (baseurl_str) cust->baseurl_str = strdup (baseurl_str);
  cust->authorative = 1;

  return cust;
}

void i_customer_free (void *custptr)
{
  /* This func should only be called
   * by i_entity_free.
   * Just free the customer-specific
   * portions of the struct. The struct
   * and everything else will be freed
   * by i_entity_free
   */
  i_customer *cust = custptr;

  if (!cust) return;

  if (cust->baseurl_str) free (cust->baseurl_str);

}

i_customer* i_customer_duplicate (i_customer *cust)
{
  i_customer *dup;

  /* Create duplicate entity struct */
  dup = i_customer_create (cust->name_str, cust->desc_str, cust->baseurl_str);
  if (!dup)
  { i_printf (1, "i_customer_duplicate failed to create dup struct"); return NULL; }

  /* Duplicate Variables */
  if (cust->fsroot_str) dup->fsroot_str = strdup (cust->fsroot_str);
  if (cust->resaddr) dup->resaddr = i_resource_address_duplicate (cust->resaddr);

  return dup;
}

/* Struct/Data Conversion */

char* i_customer_data (i_customer *cust, int *datasizeptr)
{
  /* Data format: 
   *
   * unsigned short type
   * int name_len
   * char *name_str
   * int desc_len
   * char *desc_len
   * int baseurl_len
   * char *baseurl_len
   * int uuid_len
   * char *uuid_str
   */
  int datasize;
  char *data;
  char *dataptr;

  /* Parse UUID */
  char uuid_str[37];
  uuid_unparse_lower (cust->uuid, uuid_str);

  /* Calc data size */
  datasize = (4 * sizeof(int)) + sizeof(unsigned short);
  if (cust->name_str) datasize += strlen (cust->name_str)+1;
  if (cust->desc_str) datasize += strlen (cust->desc_str)+1;
  if (cust->baseurl_str) datasize += strlen (cust->baseurl_str)+1;
  datasize += strlen(uuid_str)+1;

  /* Malloc */
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_customer_data failed to malloc data (%i bytes)", datasize); return NULL; }
  dataptr = data;

  /* Add type */
  dataptr = i_data_add_ushort (data, dataptr, datasize, &cust->ent_type);
  if (!dataptr)
  { i_printf (1, "i_customer_data failed to add ent_type to data"); free (data); return NULL; }
  
  /* Name */
  dataptr = i_data_add_string (data, dataptr, datasize, cust->name_str);
  if (!dataptr)
  { i_printf (1, "i_customer_data failed to add name_str to data"); free (data); return NULL; }

  /* Desc */
  dataptr = i_data_add_string (data, dataptr, datasize, cust->desc_str);
  if (!dataptr)
  { i_printf (1, "i_customer_data failed to add desc_str to data"); free (data); return NULL; }

  /* Base URL */
  dataptr = i_data_add_string (data, dataptr, datasize, cust->baseurl_str);
  if (!dataptr)
  { i_printf (1, "i_customer_data failed to add baseurl_str to data"); free (data); return NULL; }

  /* UUID */
  dataptr = i_data_add_string (data, dataptr, datasize, uuid_str);
  if (!dataptr)
  { i_printf (1, "i_customer_data failed to add uuid_str to data"); free (data); return NULL; }

  /* Set datasizeptr */
  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_customer* i_customer_struct (char *data, int datasize)
{
  int offset;
  unsigned short type;
  char *name_str;
  char *desc_str;
  char *baseurl_str;
  char *uuid_str;
  char *dataptr = data;
  i_customer *cust;

  /* Type */
  type = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_customer_struct failed to get type from data"); return NULL; }
  dataptr += offset;

  /* Name */
  name_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_customer_struct failed to get name_str from data"); return NULL; }
  dataptr += offset;

  /* Desc */
  desc_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_customer_struct failed to get desc_str from data"); if (name_str) free (name_str); return NULL; }
  dataptr += offset;
  
  /* Base URL */
  baseurl_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_customer_struct failed to get baseurl_str from data"); if (name_str) free (name_str); if (desc_str) free (desc_str); return NULL; }
  dataptr += offset;

  /* UUID */
  uuid_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_customer_struct failed to get uuid_str from data"); if (name_str) free (name_str); if (desc_str) free (desc_str); return NULL; }
  dataptr += offset;

  /* Create customer struct */
  cust = i_customer_create (name_str, desc_str, baseurl_str);
  uuid_parse (uuid_str, cust->uuid);
  if (name_str) free (name_str);
  if (desc_str) free (desc_str); 
  if (baseurl_str) free (baseurl_str); 
  if (uuid_str) free (uuid_str);
  if (!cust)
  { i_printf (1, "i_customer_struct failed to create customer struct"); return NULL; }

  return cust;
}

/* Misc utility */

i_resource_address* i_customer_get_resaddr_by_id (i_resource *self, char *id_str)
{
  char *str;
  i_resource_address *addr;

  if (!id_str) return NULL;

  /* Admin handling */
  if (!strcmp(id_str, "admin"))
  { asprintf (&str, "::%i:0:", RES_ADMIN); }
  else
  {
    /* Normal customer handling */
    asprintf (&str, "::%i:0:%s", RES_CUSTOMER, id_str);
  }

  addr = i_resource_address_string_to_struct (str);
  free (str);

  return addr;
}

/* Customer Specific Registration / Deregistration 
 * (Should only be called by i_entity_register and i_entity_deregister)
 */

int i_customer_register (i_resource *self, i_entity *parent, i_customer *cust)
{
  return 0;
}

int i_customer_deregister (i_resource *self, i_customer *cust)
{
  return 0;
}

