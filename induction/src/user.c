#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "user.h"
#include "auth.h"
#include "contact.h"
#include "data.h"

void i_user_free (void *userptr)
{
  i_user *user = userptr;

  if (!user) return;

  if (user->fullname) free (user->fullname);
  if (user->title) free (user->title);
  if (user->auth) i_authentication_free (user->auth);
  if (user->contact) i_contact_profile_free (user->contact);

  free (user);
}

i_user* i_user_create ()
{
  i_user *user;

  user = (i_user *) malloc (sizeof(i_user));
  if (!user)
  {
    i_printf (1, "i_user_create failed to malloc user");
    return NULL;
  }
  memset (user, 0, sizeof(i_user));

  user->auth = i_authentication_create ();
  if (!user->auth)
  {
    i_printf (1, "i_user_create failed to create user->auth");
    i_user_free (user);
    return NULL;
  }

  return user;
}

char* i_user_struct_to_data (i_user *user, int *datasizeptr)
{
  /* Data format is as follows
   *
   * int auth_datasize
   * void *auth_data
   *
   * int contact_datasize
   * void *contact_data
   *
   * int fullname_datasize
   * char *fullname
   * int title_datasize
   * char *title
   *
   */

  int datasize;
  int auth_datasize;
  int contact_datasize;
  char *auth_data;
  char *contact_data;
  char *data;
  char *dataptr;

  if (!datasizeptr) return NULL;
  memset (datasizeptr, 0, sizeof(int));
  if (!user || !user->auth) return NULL;
  
  auth_data = i_authentication_struct_to_data (user->auth, &auth_datasize);
  if (!auth_data || auth_datasize < 1)
  {
    i_printf (1, "i_user_struct_to_data failed to convert user->auth to data");
    return NULL;
  }

  contact_data = i_contact_profile_struct_to_data (user->contact, &contact_datasize);
  if (!contact_data || contact_datasize < 1)
  {
    i_printf (1, "i_user_struct_to_data failed to convert user->contact to data");
    if (auth_data) free (auth_data);
    return NULL;
  }

  datasize = (4*sizeof(int)) + auth_datasize + contact_datasize;
  if (user->fullname) datasize += strlen(user->fullname)+1;
  if (user->title) datasize += strlen(user->title)+1;
  
  data = (char *) malloc (datasize);
  if (!data)
  {
    i_printf (1, "i_user_struct_to_data failed to malloc data (%i bytes)", datasize);
    if (auth_data) free (auth_data);
    if (contact_data) free (contact_data);
    return NULL;
  }
  dataptr = data;

 
  dataptr = i_data_add_chunk (data, dataptr, datasize, auth_data, auth_datasize);
  free (auth_data);
  if (!dataptr)
  {
    i_printf (1, "i_user_struct_to_data failed to copy auth_data into data block");
    free (data);
    if (contact_data) free (contact_data);
    return NULL;
  }

  dataptr = i_data_add_chunk (data, dataptr, datasize, contact_data, contact_datasize);
  free (contact_data);
  if (!dataptr)
  {
    i_printf (1, "i_user_struct_to_data failed to copy contact_data into data block");
    free (data);
    return NULL;
  }
  
  dataptr = i_data_add_string (data, dataptr, datasize, user->fullname);
  if (!data)
  { i_printf (1, "i_user_struct_to_data failed to add user->fullname to data block"); free (data); return NULL; }
 
  dataptr = i_data_add_string (data, dataptr, datasize, user->title);
  if (!data)
  { i_printf (1, "i_user_struct_to_data failed to add user->fullname to data block"); free (data); return NULL; }
 
  memcpy (datasizeptr, &datasize, sizeof(int));

  return data;
}

i_user* i_user_data_to_struct (char *data, unsigned int datasize)
{
  int offset;
  int auth_datasize;
  int contact_datasize;
  char *auth_data;
  char *contact_data;
  char *dataptr = data;
  i_user *user;

  if (!data || datasize < sizeof(int)) return NULL;

  user = i_user_create ();
  if (!user)
  {
    i_printf (1, "i_user_data_to_struct failed to create user struct");
    return NULL;
  }

  /* Auth Data */
  
  auth_data = i_data_get_chunk (data, dataptr, datasize, &auth_datasize, &offset);
  if (offset < 1)
  {
    i_printf (1, "i_user_data_to_struct failed to get auth_data from data block");
    i_user_free (user);
    return NULL;
  }
  dataptr += offset;

  if (user->auth) { i_authentication_free (user->auth); user->auth = NULL; }
  user->auth = i_authentication_data_to_struct (auth_data, auth_datasize);
  free (auth_data);
  if (!user->auth)
  {
    i_printf (1, "i_user_data_to_struct failed to convert auth_data to auth struct");
    i_user_free (user);
    return NULL;
  }

  /* Contact profile */

  contact_data = i_data_get_chunk (data, dataptr, datasize, &contact_datasize, &offset);
  if (offset < 1)
  {
    i_printf (1, "i_user_data_to_struct failed to get contact_data from data block");
    i_user_free (user);
    return NULL;
  }
  dataptr += offset;

  user->contact = i_contact_profile_data_to_struct (contact_data, contact_datasize);
  free (contact_data);
  if (!user->contact)
  {
    i_printf (1, "i_user_data_to_struct failed to convert contact_data to contact profile");
    i_user_free (user);
    return NULL;
  }

  /* Username / Title / Etc */

  user->fullname = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_user_data_to_struct failed to get fullname from data block"); i_user_free (user); return NULL; }
  dataptr += offset;

  user->title = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_user_data_to_struct failed to get title from data block"); i_user_free (user); return NULL; }
  dataptr += offset;

  /* Finished */

  return user;  
}

i_user* i_user_duplicate (i_user *user)
{
  char *data;
  int datasize;

  data = i_user_struct_to_data (user, &datasize);
  if (!data) return NULL;
  user = i_user_data_to_struct (data, datasize);
  free (data);

  return user;
}
