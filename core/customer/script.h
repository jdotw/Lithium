/*
 * Actions - User-defined scripts to be run in the event
 *           of an incident occuring.
 */

#define SCRIPT_UNKNOWN 0
#define SCRIPT_OK 1
#define SCRIPT_ERROR 2

typedef struct l_script_proc_s
{
  /* Process */
  int pid;
  int readfd;
  int writefd;

  /* Socket */
  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
  struct i_timer_s *timeout_timer;

  /* Script */
  char *script_file;            /* File to execute */
  char *output_str;              /* The output from the script */

  /* Command line args */
  char *command_str;            /* Command to be executed */

  /* Callbacks */
  struct i_callback_s *cb;            /* Callback to be called after execution */
  struct i_callback_s *configvar_cb;  /* Callback from the configvar load operation */

} l_script_proc;

#define ACTION_DAY_MON 1

/* script_exec.c */
struct l_script_proc_s* l_script_proc_create ();
void l_script_proc_free (void *procptr);
struct l_script_proc_s* l_script_exec (i_resource *self, char *script_file, char *command_str, int (*cbfunc) (), void *passdata);
int l_script_exec_configvar_cb (i_resource *self, struct i_list_s *list, void *passdata);
int l_script_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_script_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_script_exec_timeoutcb (i_resource *self, struct i_timer_s *timer, void *passdata);

/* script_xml_upload.c */
int l_script_upload_repaircb (i_resource *self, struct l_script_proc_s *proc, void *passdata);

/* script_xml_repair.c */
int l_script_repair_repaircb (i_resource *self, l_script_proc *proc, void *passdata);

