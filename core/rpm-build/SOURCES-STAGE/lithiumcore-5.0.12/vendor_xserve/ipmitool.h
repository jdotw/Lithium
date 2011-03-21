/* ipmitool.c */

typedef struct v_ipmitool_proc_s
{
  int pid;
  int readfd;
  int writefd;
  int errfd;
  
  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
  struct i_socket_s *errsock;
  struct i_socket_callback_s *errsockcb;
  struct i_timer_s *timeout_timer;

  struct v_ipmitool_cmd_s *curcmd;
  int timeout_count;
} v_ipmitool_proc;

typedef struct v_ipmitool_cmd_s
{
  char *command_str;            /* Command to be executed */
  char *args_str;               /* Optional args */
  char *output_str;             /* The output from ipmitool */
  void *raw_data;               /* Raw data */
  size_t raw_datasize;         /* Raw data size */
  struct i_callback_s *cb;      /* Callback to be called after execution */
  struct timeval req_time;      /* Request time stamp */
  struct timeval resp_time;     /* Response time stamp */
} v_ipmitool_cmd;

typedef struct v_ipmitool_apple_req_s
{
  /* Req info */
  char *type_hex;
  char *item_hex;

  /* Op variables */
  int index;
  struct v_ipmitool_cmd_s *curcmd;

  /* Data */
  char *data;
  size_t datasize;

  /* Callback */ 
  int (*cbfunc) ();
  void *passdata;
} v_ipmitool_apple_req;

#define IPMIRESULT_UNKNOWN 0
#define IPMIRESULT_OK 1
#define IPMIRESULT_FAILED 2
#define IPMIRESULT_TIMEOUT 3

#define IPMIFLAG_URGENT 1

struct i_list_s* v_ipmitool_proc_cmdqueue ();

v_ipmitool_proc* v_ipmitool_proc_create ();
void v_ipmitool_proc_free (void *procptr);

v_ipmitool_cmd* v_ipmitool_cmd_create ();
void v_ipmitool_cmd_free (void *cmdptr);

struct v_ipmitool_cmd_s* v_ipmitool_exec (i_resource *self, char *command_str, char *args_str, unsigned short flags, int (*cbfunc) (i_resource *self, v_ipmitool_cmd *cmd, int result, void *passdata), void *passdata);
int v_ipmitool_cancel (i_resource *self, v_ipmitool_cmd *cmd);
int v_ipmitool_exec_next (i_resource *self);
int v_ipmitool_exec_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int v_ipmitool_exec_timeoutcb (i_resource *self, struct i_timer_s *timer, void *passdata);

int v_ipmitool_spawn (i_resource *self);
int v_ipmitool_terminate (i_resource *self);

/* ipmitool_apple.c */
v_ipmitool_apple_req* v_ipmitool_apple_get (i_resource *self, char *type_hex, char *item_hex, int (*cbfunc) (), void *passdata);
int v_ipmitool_apple_ipmicb (i_resource *self, v_ipmitool_cmd *cmd, int result, void *passdata);
void v_ipmitool_apple_dump (v_ipmitool_apple_req *req);

/* ipmitool_apple_req.c */
v_ipmitool_apple_req* v_ipmitool_apple_req_create ();
void v_ipmitool_apple_req_free (void *reqptr);

/* ipmitool_util.c */
char* v_ipmitool_string (void *data, size_t datasize, size_t offset);
uint32_t v_ipmitool_uint32 (void *data, size_t datasize, size_t offset);
int v_ipmitool_octet (void *data, size_t datasize, size_t offset);


