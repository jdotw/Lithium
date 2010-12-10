#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "list.h"
#include "entity.h"
#include "customer.h"
#include "site.h"
#include "form.h"
#include "data.h"

/** \addtogroup site Sites
 * @ingroup entity
 * @{
 */

extern i_resource *global_self;

/* Struct Manipulation */

i_site* i_site_create (char *name_str, char *desc_str, char *addr1_str, char *addr2_str, char *addr3_str, char *suburb_str, char *state_str, char *postcode_str, char *country_str, double longitude, double latitude)
{
  i_site *site;

  site = (i_site *) i_entity_create (name_str, desc_str, ENT_SITE, sizeof(i_site));
  if (!site)
  { i_printf (1, "i_site_create failed to create site entity"); return NULL; }
  if (addr1_str) site->addr1_str = strdup (addr1_str);
  if (addr2_str) site->addr2_str = strdup (addr2_str);
  if (addr3_str) site->addr3_str = strdup (addr3_str);
  if (suburb_str) site->suburb_str = strdup (suburb_str);
  if (state_str) site->state_str = strdup (state_str);
  if (postcode_str) site->postcode_str = strdup (postcode_str);
  if (country_str) site->country_str = strdup (country_str);
  site->longitude = longitude;
  site->latitude = latitude;
  site->authorative = 1;

  return site;
}

void i_site_free (void *siteptr)
{
  /* This func should only be called 
   * by i_entity_free.
   * Just free the site-specific
   * portions of the struct. The struct
   * and everything else will be freed
   * by i_entity_free
   */
  i_site *site = siteptr;

  if (!site) return;

  if (site->addr1_str) free (site->addr1_str);
  if (site->addr2_str) free (site->addr2_str);
  if (site->addr3_str) free (site->addr3_str);
  if (site->suburb_str) free (site->suburb_str);
  if (site->state_str) free (site->state_str);
  if (site->postcode_str) free (site->postcode_str);
  if (site->country_str) free (site->country_str);
  if (site->subnet_list) i_list_free (site->subnet_list);
}

i_site* i_site_duplicate (i_site *site)
{
  i_site *dup;

  dup = i_site_create (site->name_str, site->desc_str, site->addr1_str, site->addr2_str, site->addr3_str, site->suburb_str, site->state_str, site->postcode_str, site->country_str, site->longitude, site->latitude);
  if (!dup)
  { i_printf (1, "i_site_duplicate failed to create dup site"); return NULL; }

  /* Duplicate Variables */
  if (site->fsroot_str) dup->fsroot_str = strdup (site->fsroot_str);
  if (site->resaddr) dup->resaddr = i_resource_address_duplicate (site->resaddr);

  return dup;
}

/* Struct/Data Conversion */

char* i_site_data (i_site *site, int *datasizeptr)
{
  /* Data format: 
   *
   * unsigned short type;
   * int name_len
   * char *name_str
   * int desc_len
   * char *desc_len
   * int addr1_len;
   * char *addr1_str;
   * int addr2_len;
   * char *addr2_str;
   * int addr3_len;
   * char *addr3_str;
   * int suburb_len;
   * char *suburb_str;
   * int state_len;
   * char *state_str;
   * int postcode;
   * char *postcode_str;
   * int country_len;
   * char *country_str;
   * double longitude;
   * double latitude;
   * int uuid_len;
   * char *uuid_str;
   * int custdata_size;
   * char *custdata;
   *
   */
  int datasize;
  int custdata_size;
  char *data;
  char *dataptr;
  char *custdata;
  char uuid_str[37];

  /* Parse UUID */
  uuid_unparse_lower (site->uuid, uuid_str);

  /* Create customer data */
  if (site->cust)
  {
    custdata = i_customer_data (site->cust, &custdata_size);
    if (!custdata)
    { i_printf (1, "i_site_data failed to convert site->cust to data"); return NULL; }
  }
  else
  { custdata = NULL; custdata_size = 0; }

  /* Calc data size */
  datasize = (11 * sizeof(int)) + sizeof(unsigned short) + (2*sizeof(double)) + custdata_size;
  if (site->name_str) datasize += strlen (site->name_str)+1;
  if (site->desc_str) datasize += strlen (site->desc_str)+1;
  if (site->addr1_str) datasize += strlen (site->addr1_str)+1;
  if (site->addr2_str) datasize += strlen (site->addr2_str)+1;
  if (site->addr3_str) datasize += strlen (site->addr3_str)+1;
  if (site->suburb_str) datasize += strlen (site->suburb_str)+1;
  if (site->state_str) datasize += strlen (site->state_str)+1;
  if (site->postcode_str) datasize += strlen (site->postcode_str)+1;
  if (site->country_str) datasize += strlen (site->country_str)+1;
  datasize += strlen (uuid_str)+1;

  /* Malloc */
  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_site_data failed to malloc data (%i bytes)", datasize); if (custdata) free (custdata); return NULL; }
  dataptr = data;

  /* Add type */
  dataptr = i_data_add_ushort (data, dataptr, datasize, &site->ent_type);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add ent_type to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Name */
  dataptr = i_data_add_string (data, dataptr, datasize, site->name_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add name_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Desc */
  dataptr = i_data_add_string (data, dataptr, datasize, site->desc_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add desc_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Addr1 */
  dataptr = i_data_add_string (data, dataptr, datasize, site->addr1_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add addr1_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Addr2 */
  dataptr = i_data_add_string (data, dataptr, datasize, site->addr2_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add addr2_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Addr3 */
  dataptr = i_data_add_string (data, dataptr, datasize, site->addr3_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add addr3_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Suburb */
  dataptr = i_data_add_string (data, dataptr, datasize, site->suburb_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add suburb_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* State */
  dataptr = i_data_add_string (data, dataptr, datasize, site->state_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add state_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Postcode */
  dataptr = i_data_add_string (data, dataptr, datasize, site->postcode_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add postcode_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Country */
  dataptr = i_data_add_string (data, dataptr, datasize, site->country_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add country_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Longitude */
  dataptr = i_data_add_double (data, dataptr, datasize, &site->longitude);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add longitude to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Latitude */
  dataptr = i_data_add_double (data, dataptr, datasize, &site->latitude);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add latitude to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Country */
  dataptr = i_data_add_string (data, dataptr, datasize, uuid_str);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add uuid_str to data"); free (data); if (custdata) free (custdata); return NULL; }

  /* Customer */
  dataptr = i_data_add_chunk (data, dataptr, datasize, custdata, custdata_size);
  if (custdata) free (custdata);
  if (!dataptr)
  { i_printf (1, "i_site_data failed to add custdata to data"); free (data); return NULL; }

  /* Set datasizeptr */
  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_site* i_site_struct (char *data, int datasize)
{
  int offset;
  int custdata_size;
  unsigned short type;
  char *name_str;
  char *desc_str;
  char *custdata;
  char *dataptr = data;
  i_site *site;

  /* Type */
  type = i_data_get_ushort (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get type from data"); return NULL; }
  dataptr += offset;

  /* Name */
  name_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get name_str from data"); return NULL; }
  dataptr += offset;

  /* Desc */
  desc_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct warning, failed to get desc_str from data"); }
  dataptr += offset;

  /* Create site struct */
  site = i_site_create (name_str, desc_str, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0);
  free (name_str);
  free (desc_str);
  if (!site)
  { i_printf (1, "i_site_struct failed to create site struct"); return NULL; }
    
  /* addr1 */
  site->addr1_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get addr1_str from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* addr2 */
  site->addr2_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get addr2_str from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* addr3 */
  site->addr3_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get addr3_str from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* suburb */
  site->suburb_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get suburb_str from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* state */
  site->state_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get state_str from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* postcode */
  site->postcode_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get postcode_str from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* country */
  site->country_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get country_str from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* longitude */
  site->longitude = i_data_get_double (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get longitude from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* latitude */
  site->latitude = i_data_get_double (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get latitude from data"); i_site_free (site); return NULL; }
  dataptr += offset;

  /* uuid */
  char *uuid_str = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get uuid_str from data"); i_site_free (site); return NULL; }
  uuid_parse (uuid_str, site->uuid);
  if (uuid_str) free (uuid_str);
  dataptr += offset;

  /* customer */
  custdata = i_data_get_chunk (data, dataptr, datasize, &custdata_size, &offset);
  if (offset < 1)
  { i_printf (1, "i_site_struct failed to get customer data from data"); i_site_free (site); return NULL; }
  if (custdata)
  {
    i_customer *cust;
    cust = i_customer_struct (custdata, custdata_size);
    free (custdata);
    if (cust)
    {
      /* Register site to customer */
      i_entity_register (global_self, ENTITY(cust), ENTITY(site));
    }
  }

  return site;
}

/* Site Specific Registration / Deregistration 
 * (Should only be called by i_entity_register and i_entity_deregister)
 */

int i_site_register (i_resource *self, i_customer *cust, i_site *site)
{
  return 0;
}

int i_site_deregister (i_resource *self, i_site *site)
{
  return 0;
}

