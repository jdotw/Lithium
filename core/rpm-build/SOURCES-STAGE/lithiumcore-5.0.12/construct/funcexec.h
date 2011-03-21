typedef struct
{
  int (*function) (i_resource *self, int argc, char *argv[], int optind);
  int argc;
  char **argv;
  int optind;
  struct i_module_s *module;
} c_funcexec_cbdata;

c_funcexec_cbdata* c_funcexec_cbdata_create ();
void c_funcexec_cbdata_free (void *cbdataptr);
int c_funcexec (i_resource *self, char *node_ip, int argc, char *argv[], int optind);
int c_funcexec_connect_callback (i_resource *self, struct i_socket_s *sock, void *passdata);
int c_funcexec_msgproc_default (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);
