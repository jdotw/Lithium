#define AUTH_RESULT_UNKNOWN 0
#define AUTH_RESULT_TIMEOUT 1
#define AUTH_RESULT_ERROR 2
#define AUTH_RESULT_DENIED 4
#define AUTH_RESULT_OK 8

/* LEGACY */
#define AUTH_LEVEL_NONE 0
#define AUTH_LEVEL_INFSTAT 10
#define AUTH_LEVEL_CLIENT 20
#define AUTH_LEVEL_STAFF 30
//#define AUTH_LEVEL_ADMIN 40

/* 4.9 */
#define AUTH_LEVEL_RO 20
#define AUTH_LEVEL_USER 30
#define AUTH_LEVEL_ADMIN 40

#define AUTH_LEVEL_MASTER 1024

#define AUTH_LEVEL_INTERNAL 4096

typedef struct i_authentication_s
{
  char *username;
  char *password;
  char *customer_id;
  int level;
} i_authentication;

#define AUTH_CALLBACK int (*callback_func) (i_resource *self, i_authentication *auth, int result, void *passdata)

typedef struct i_authentication_verify_passdata_s
{
  struct i_authentication_s *auth;
  AUTH_CALLBACK;
  void *passdata;
} i_authentication_verify_passdata;


/* authentication.c */

void i_authentication_free (void *authptr);
i_authentication* i_authentication_create ();
i_authentication* i_authentication_duplicate (i_authentication *auth);
void i_authentication_verify_free_passdata (void *passptr);
int i_authentication_verify (i_resource *self, struct i_resource_address_s *authres, struct i_authentication_s *creds, AUTH_CALLBACK, void *passdata);
char* i_authentication_struct_to_data (i_authentication *auth, int *datasizeptr);
i_authentication* i_authentication_data_to_struct (char *data, unsigned int datasize);
int i_authentication_compare (i_authentication *control, i_authentication *auth);

