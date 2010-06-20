typedef struct
{
  i_socket_data *read_op;
  i_timer *timeout_timer;
  struct l_gsmdevice_s *device;
  char *data;
  int datasize;
  int (*callback_func) (i_resource *self, char *line, void *passdata);
  void *passdata;
} l_atcommand_readln_cbdata;

int l_atcommand_exec (i_resource *self, l_gsmdevice *device, char *command_raw);
int l_atcommand_exec_write_callback (i_resource *self, i_socket *sock, int result, void *passdata);
int l_atcommand_exec_timeout_callback (i_resource *self, i_timer *timer, void *passdata);

l_atcommand_readln_cbdata* l_atcommand_readln_cbdata_create ();
void l_atcommand_readln_cbdata_free (void *cbdataptr);
int l_atcommand_readln (i_resource *self, l_gsmdevice *device, int (*callback_func) (i_resource *self, char *line, void *passdata), void *passdata);
int l_atcommand_readln_read_callback (i_resource *self, i_socket *sock, i_socket_data *data, int result, void *passdata);
int l_atcommand_readln_timeout_callback (i_resource *self, i_timer *timer, void *passdata);
int l_atcommand_readln_failed (i_resource *self, l_atcommand_readln_cbdata *cbdata);

int l_atcommand_readln_expect (i_resource *self, l_gsmdevice *device, char *expect, int (*callback_func) (i_resource *self, char *line, void *passdata), void *passdata);
int l_atcommand_readln_expect_read_callback (i_resource *self, char *line, void *passdata);

