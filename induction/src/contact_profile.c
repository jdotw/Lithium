#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "contact.h"
#include "data.h"
#include "list.h"

void i_contact_profile_free (void *profileptr)
{
  i_contact_profile *profile = profileptr;

  if (!profile) return;

  if (profile->email) free (profile->email);
  if (profile->office) i_contact_info_free (profile->office);
  if (profile->mobile) i_contact_info_free (profile->mobile);
  if (profile->after_hours) i_contact_info_free (profile->after_hours);
  if (profile->others) i_list_free (profile->others);

  free (profile);

  return;
}

i_contact_profile* i_contact_profile_create ()
{
  i_contact_profile *profile;

  profile = (i_contact_profile *) malloc (sizeof(i_contact_profile));
  if (!profile)
  { i_printf (1, "i_contact_profile_create failed to malloc profile"); return NULL; }
  memset (profile, 0, sizeof(i_contact_profile));

  profile->others = i_list_create ();
  if (!profile->others)
  {
    i_printf (1, "i_contact_profile_create failed to create profile->others list");
    i_contact_profile_free (profile);
    return NULL;
  }
  i_list_set_destructor (profile->others, i_contact_info_free);

  return profile;
}

char* i_contact_profile_struct_to_data (i_contact_profile *profile, int *datasizeptr)
{
  /* Data Format :
   *
   * int hours
   * int email_size
   * char *email
   * 
   * int office_datasize;
   * void *office_data;
   * int mobile_datasize;
   * void *mobile_data;
   * int ah_datasize;
   * void *ah_data;
   *
   * unsigned long others_count;
   * (for each i_contact_info struct in profile->others)
   * int info_datasize
   * void *info_data
   * (repeated)
   */

  int office_datasize = 0;
  int mobile_datasize = 0;
  int ah_datasize = 0;
  int datasize;
  char *office_data = NULL;
  char *mobile_data = NULL;
  char *ah_data = NULL;
  char *data;
  char *dataptr;
  
  if (!datasizeptr) { i_printf (1, "i_contact_profile_struct_to_data passed NULL datasizeptr"); return NULL; }
  memset (datasizeptr, 0, sizeof(int));
  if (!profile) { i_printf (1, "i_contact_profile_struct_to_data passed NULL profile"); return NULL; }

  /* Convert the office/mobile/ah contact_info structs to data */
  
  if (profile->office) 
  {
    office_data = i_contact_info_struct_to_data (profile->office, &office_datasize);
    if (!office_data) 
    { i_printf (1, "i_contact_profile_struct_to_data failed to convert profile->office to data"); return NULL; }
  }
  
  if (profile->mobile) 
  {
    mobile_data = i_contact_info_struct_to_data (profile->mobile, &mobile_datasize);
    if (!mobile_data) 
    { 
      i_printf (1, "i_contact_profile_struct_to_data failed to convert profile->mobile to data"); 
      if (office_data) free (office_data); 
      return NULL; 
    }
  }
  
  if (profile->after_hours) 
  {
    ah_data = i_contact_info_struct_to_data (profile->after_hours, &ah_datasize);
    if (!ah_data) 
    { 
      i_printf (1, "i_contact_profile_struct_to_data failed to convert profile->after_hours to data"); 
      if (office_data) free (office_data);
      if (mobile_data) free (mobile_data); 
      return NULL; 
    }
  }

  /* Create the initial data block */

  datasize = (5*sizeof(int)) + sizeof(long) + office_datasize + mobile_datasize + ah_datasize;
  if (profile->email) datasize += strlen (profile->email) + 1;
    
  data = (char *) malloc (datasize);
  if (!data)
  {
    i_printf (1, "i_contact_profile_struct_to_data failed to malloc data (%i bytes)", datasize);
    if (office_data) free (office_data);
    if (mobile_data) free (mobile_data); 
    if (ah_data) free (ah_data);
    return NULL;
  }
  memset (data, 0, datasize);
  dataptr = data;

  /* Add the hours int */

  dataptr = i_data_add_int (data, dataptr, datasize, &profile->hours);
  if (!dataptr)
  { 
    i_printf (1, "i_contact_profile_struct_to_data failed to add hours to data");
    if (office_data) free (office_data);
    if (mobile_data) free (mobile_data);
    if (ah_data) free (ah_data);
    return NULL;
  }

  /* Add the email string */

  dataptr = i_data_add_string (data, dataptr, datasize, profile->email);
  if (!dataptr)
  { 
    i_printf (1, "i_contact_profile_struct_to_data failed to add email to data");
    if (office_data) free (office_data);
    if (mobile_data) free (mobile_data);
    if (ah_data) free (ah_data);
    return NULL;
  }
  
  /* Add the office/mobile/ah datablocks to the initial datablock */

  dataptr = i_data_add_chunk (data, dataptr, datasize, office_data, office_datasize);
  if (office_data) free (office_data);
  if (!dataptr)
  { 
    i_printf (1, "i_contact_profile_struct_to_data failed to add office_data to data");
    if (mobile_data) free (mobile_data);
    if (ah_data) free (ah_data);
    return NULL;
  }

  dataptr = i_data_add_chunk (data, dataptr, datasize, mobile_data, mobile_datasize);
  if (mobile_data) free (mobile_data);
  if (!dataptr)
  { 
    i_printf (1, "i_contact_profile_struct_to_data failed to add mobile_data to data");
    if (ah_data) free (ah_data);
    return NULL;
  }

  dataptr = i_data_add_chunk (data, dataptr, datasize, ah_data, ah_datasize);
  if (ah_data) free (ah_data);
  if (!dataptr)
  { 
    i_printf (1, "i_contact_profile_struct_to_data failed to add ah_data to data");
    return NULL;
  }

  /* Handle the list of 'other' contact_info structs */

  if (profile->others)
  {
    i_contact_info *otherinfo;

    dataptr = i_data_add_long (data, dataptr, datasize, (long *) &profile->others->size);
    if (!dataptr) 
    { i_printf (1, "i_contact_profile_struct_to_data failed to add profile->others->size to data"); free (data); return NULL; }

    for (i_list_move_head(profile->others); (otherinfo=i_list_restore(profile->others))!=NULL; i_list_move_next(profile->others))
    {
      /* Loop through each 'other' contact */

      int olddatasize;
      int other_datasize;
      char *other_data;
      char *x;

      other_data = i_contact_info_struct_to_data (otherinfo, &other_datasize);
      if (!other_data)
      { i_printf (1, "i_contact_profile_struct_to_data failed to add a 'other' contact to data. continuing"); continue; }

      olddatasize = datasize;
      datasize += other_datasize;
      x = (char *) realloc (data, datasize);
      if (!x)
      { 
        i_printf (1, "i_contact_profile_struct_to_data failed to realloc data to handle a 'other' contact. continuing");
        datasize = olddatasize;
        continue;
      }

      data = x;
      dataptr = data + olddatasize;

      dataptr = i_data_add_chunk (data, dataptr, datasize, other_data, other_datasize);
      free (other_data);
      if (!dataptr)
      {
        i_printf (1, "i_contact_profile_struct_to_data failed to add other_data to data");
        free (data);
        return NULL;
      }
    }
  }
  else
  {
    /* No profile->others */
    unsigned long zero_count = 0;

    dataptr = i_data_add_long (data, dataptr, datasize, (long *) &zero_count);
    if (!dataptr)
    { i_printf (1, "i_contact_profile_struct_to_data failed to add zero_count to data"); free (data); return NULL; }
  }

  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_contact_profile* i_contact_profile_data_to_struct (char *data, int datasize)
{
  /* Data Format :
   *
   * int office_datasize;
   * void *office_data;
   * int mobile_datasize;
   * void *mobile_data;
   * int ah_datasize;
   * void *ah_data;
   *
   * unsigned long others_count;
   * (for each i_contact_info struct in profile->others)
   * int info_datasize
   * void *info_data
   * (repeated)
   */

  unsigned long i;
  int offset;
  unsigned long others_count = 0;
  char *office_data;
  int office_datasize;
  char *mobile_data;
  int mobile_datasize;
  char *ah_data;
  int ah_datasize;
  char *dataptr = data;
  i_contact_profile *profile;

  profile = i_contact_profile_create ();
  if (!profile)
  { i_printf (1, "i_contact_profile_data_to_struct failed to create profile"); i_contact_profile_free (profile); return NULL; }

  profile->hours = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_profile_data_to_struct failed to get hours int from data"); i_contact_profile_free (profile); return NULL; }
  dataptr += offset;

  profile->email = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_profile_data_to_struct failed to get hours string from data"); i_contact_profile_free (profile); return NULL; }
  dataptr += offset;

  office_data = i_data_get_chunk (data, dataptr, datasize, &office_datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_profile_data_to_struct failed to get office_data from data"); i_contact_profile_free (profile); return NULL; }
  dataptr += offset;
  if (office_data) 
  { profile->office = i_contact_info_data_to_struct (office_data, office_datasize); free (office_data); }
  
  mobile_data = i_data_get_chunk (data, dataptr, datasize, &mobile_datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_contact_profile_data_to_struct failed to get mobile_data from data"); i_contact_profile_free (profile); return NULL; }
  dataptr += offset;
  if (mobile_data)
  { profile->mobile = i_contact_info_data_to_struct (mobile_data, mobile_datasize); free (mobile_data); }

  ah_data = i_data_get_chunk (data, dataptr, datasize, &ah_datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_contact_profile_data_to_struct failed to get ah_data from data"); i_contact_profile_free (profile); return NULL; }
  dataptr += offset;
  if (ah_data)
  { profile->after_hours = i_contact_info_data_to_struct (ah_data, ah_datasize); free (ah_data); }

  others_count = (unsigned long) i_data_get_long (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_contact_profile_data_to_struct failed to get others_count from data"); i_contact_profile_free (profile); return NULL; }
  dataptr += offset;

  for (i=0; i < others_count; i++)
  {
    /* Loop for each 'other' in the data */
    char *other_data;
    int other_datasize;
  
    other_data = i_data_get_chunk (data, dataptr, datasize, &other_datasize, &offset);
    if (offset < 1) 
    { i_printf (1, "i_contact_profile_data_to_struct failed to get other_data from data (instance %i)", i); i_contact_profile_free (profile); return NULL; }
    dataptr += offset;
    
    if (other_data)
    {
      i_contact_info *other;

      other = i_contact_info_data_to_struct (other_data, other_datasize); 
      free (other_data);
      if (!other)
      {
        i_printf (1, "i_contact_profile_data_to_struct failed to convert other_data to info struct (instance %i). continuing", i);
        i_contact_profile_free (profile);
        continue;
      }
      i_list_enqueue (profile->others, other);
    }
  }

  return profile;
}
