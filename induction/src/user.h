typedef struct i_user_s
{
  char *fullname;
  char *title;
  struct i_authentication_s *auth;
  struct i_contact_profile_s *contact;
} i_user;

void i_user_free (void *userptr);
i_user* i_user_create ();

char* i_user_struct_to_data (i_user *user, int *datasizeptr);
i_user* i_user_data_to_struct (char *data, unsigned int datasize);

i_user* i_user_duplicate (i_user *user);

