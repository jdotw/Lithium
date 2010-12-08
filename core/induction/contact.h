/* Hours definitions */

#define HOURS_24x7 1
#define HOURS_8_6x5 2

typedef struct i_contact_info_s
{
  char *phone;
  char *address_1;
  char *address_2;
  char *address_3;
  char *notes;
} i_contact_info;

typedef struct i_contact_profile_s
{
  int hours;
  char *email;
  struct i_contact_info_s *office;
  struct i_contact_info_s *mobile;
  struct i_contact_info_s *after_hours;
  struct i_list_s *others;
} i_contact_profile;

/* Contact info functions - contact_info.c */

void i_contact_info_free (void *contactptr);
i_contact_info* i_contact_info_create ();
char* i_contact_info_struct_to_data (i_contact_info *info, int *datasizeptr);
i_contact_info* i_contact_info_data_to_struct (char *data, int datasize);

/* Contact profile functions - contact_profile.c */

void i_contact_profile_free (void *profileptr);
i_contact_profile* i_contact_profile_create ();
char* i_contact_profile_struct_to_data (i_contact_profile *profile, int *datasizeptr);
i_contact_profile* i_contact_profile_data_to_struct (char *data, int datasize);
