#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "contact.h"
#include "data.h"

void i_contact_info_free (void *contactptr)
{
  i_contact_info *contact = contactptr;

  if (!contact) return;

  if (contact->phone) free (contact->phone);
  if (contact->address_1) free (contact->address_1);
  if (contact->address_2) free (contact->address_2);
  if (contact->address_3) free (contact->address_3);
  if (contact->notes) free (contact->notes);

  free (contact);
}

i_contact_info* i_contact_info_create ()
{
  i_contact_info *contact;

  contact = (i_contact_info *) malloc (sizeof(i_contact_info));
  if (!contact)
  { i_printf (1, "i_contact_info_create failed to malloc contact"); return NULL; }
  memset (contact, 0, sizeof(i_contact_info));

  return contact;
}

char* i_contact_info_struct_to_data (i_contact_info *info, int *datasizeptr)
{
  /* Data format is :
   *
   * int phone_size;
   * char *phone;
   * int address_1_size;
   * char *address_1;
   * int address_2_size;
   * char *address_2;
   * int address_3_size;
   * char *address_3;
   * int notes_size;
   * char *notes;
   */

  int datasize;
  char *data;
  char *dataptr;

  if (!datasizeptr) return NULL;
  memset (datasizeptr, 0, sizeof(int));
  if (!info) return NULL;
  
  datasize = 5*(sizeof(int));
  if (info->phone) datasize += strlen(info->phone)+1;
  if (info->address_1) datasize += strlen(info->address_1)+1;
  if (info->address_2) datasize += strlen(info->address_2)+1;
  if (info->address_3) datasize += strlen(info->address_3)+1;
  if (info->notes) datasize += strlen(info->notes)+1;

  data = (char *) malloc (datasize);
  if (!data)
  { i_printf (1, "i_contact_info_struct_to_data failed to malloc data"); return NULL; }
  memset (data, 0, datasize);
  dataptr = data;

  dataptr = i_data_add_string (data, dataptr, datasize, info->phone);
  if (!dataptr) { i_printf (1, "i_info_info_struct_to_data failed to add info->phone to data"); free (data); return NULL; }
  
  dataptr = i_data_add_string (data, dataptr, datasize, info->address_1);
  if (!dataptr) { i_printf (1, "i_info_info_struct_to_data failed to add info->address_1 to data"); free (data); return NULL; }
  
  dataptr = i_data_add_string (data, dataptr, datasize, info->address_2);
  if (!dataptr) { i_printf (1, "i_info_info_struct_to_data failed to add info->address_2 to data"); free (data); return NULL; }
  
  dataptr = i_data_add_string (data, dataptr, datasize, info->address_3);
  if (!dataptr) { i_printf (1, "i_info_info_struct_to_data failed to add info->address_3 to data"); free (data); return NULL; }
  
  dataptr = i_data_add_string (data, dataptr, datasize, info->notes);
  if (!dataptr) { i_printf (1, "i_info_info_struct_to_data failed to add info->notes to data"); free (data); return NULL; }

  memcpy (datasizeptr, &datasize, sizeof(int));
  
  return data;
}

i_contact_info* i_contact_info_data_to_struct (char *data, int datasize)
{
  int offset;
  char *dataptr = data;
  i_contact_info *info;

  info = i_contact_info_create ();
  if (!info)
  { i_printf (1, "i_contact_info_data_to_struct failed to create info struct"); return NULL; }

  info->phone = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_info_data_to_struct failed to get contact->phone from data"); i_contact_info_free (info); return NULL; }
  dataptr += offset;

  info->address_1 = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_info_data_to_struct failed to get contact->address_1 from data"); i_contact_info_free (info); return NULL; }
  dataptr += offset;

  info->address_2 = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_info_data_to_struct failed to get contact->address_2 from data"); i_contact_info_free (info); return NULL; }
  dataptr += offset;

  info->address_3 = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_info_data_to_struct failed to get contact->address_3 from data"); i_contact_info_free (info); return NULL; }
  dataptr += offset;

  info->notes = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_info_data_to_struct failed to get contact->notes from data"); i_contact_info_free (info); return NULL; }
  dataptr += offset;

  return info;
}
  
