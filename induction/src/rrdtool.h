/* rrdtool.c */

typedef struct i_rrdtool_proc_s
{
  int pid;
  int readfd;
  int writefd;
  
  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
  struct i_timer_s *timeout_timer;

  struct i_rrdtool_cmd_s *curcmd;
  int timeout_count;
} i_rrdtool_proc;

typedef struct i_rrdtool_cmd_s
{
  char *fullpath_str;           /* Full path the the file resulting from the command */
  char *command_str;            /* Command to be executed */
  char *output_str;             /* The output from RRDtool */
  struct i_callback_s *cb;      /* Callback to be called after execution */
} i_rrdtool_cmd;

#define RRDRESULT_UNKNOWN 0
#define RRDRESULT_OK 1
#define RRDRESULT_FAILED 2
#define RRDRESULT_TIMEOUT 3

#define RRDFLAG_URGENT 1

struct i_list_s* i_rrdtool_proc_cmdqueue ();
i_rrdtool_proc* i_rrdtool_proc_create ();
void i_rrdtool_proc_free (void *procptr);
i_rrdtool_cmd* i_rrdtool_cmd_create ();
void i_rrdtool_cmd_free (void *cmdptr);
struct i_rrdtool_cmd_s* i_rrdtool_exec (i_resource *self, char *fullpath_str, char *command_str, unsigned short flags, int (*cbfunc) (i_resource *self, i_rrdtool_cmd *cmd, int result, void *passdata), void *passdata);
int i_rrdtool_exec_next (i_resource *self);
int i_rrdtool_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int i_rrdtool_exec_timeoutcb (i_resource *self, struct i_timer_s *timer, void *passdata);
int i_rrdtool_spawn (i_resource *self);
int i_rrdtool_terminate (i_resource *self);
