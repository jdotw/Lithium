/* Defaults */

#define DEFAULT_MSG_TIMEOUT_SEC 2
#define DEFAULT_MSG_TIMEOUT_USEC 0

/* Structs */

typedef struct i_msgproc_s
{
  struct i_socket_s *socket;
  struct i_hashtable_s *handler_table;
  struct i_hashtable_s *callback_table;
  
  struct i_msgproc_callback_s *default_callback;
  struct i_msgproc_handler_s *default_handler;
  struct i_msgproc_handler_s *fatal_handler;
  
  struct i_socket_callback_s *read_preempt_callback;
  struct i_message_read_cbdata *msg_read_cbdata;
} i_msgproc;

typedef struct i_msgproc_callback_s
{
  long type;
  long reqid;
  struct i_socket_s *socket;
  struct timeval timeout;
  struct i_timer_s *timeout_timer;

  int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
  void *passdata;
} i_msgproc_callback;

typedef struct i_msgproc_handler_s
{
  int type;
  i_socket *socket;

  int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata);
  void *passdata;

  int uses;
} i_msgproc_handler;


/* Prototypes */

/* msgproc.c */

i_msgproc* i_msgproc_create ();
void i_msgproc_free (void *msgprocptr);
int i_msgproc_enable (i_resource *self, i_socket *sock);
int i_msgproc_disable (i_resource *self, i_socket *sock);
int i_msgproc_fatal (i_resource *self, i_socket *sock);

/* msgproc_callback.c */

i_msgproc_callback* i_msgproc_callback_create ();
void i_msgproc_callback_free (void *cbptr);
i_msgproc_callback* i_msgproc_callback_add (i_resource *self, i_socket *sock, long reqid, time_t timeout_sec, time_t timeout_usec, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *data), void *passdata);
int i_msgproc_callback_set_default (i_resource *self, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *data), void *passdata);
int i_msgproc_callback_remove_by_reqid (i_socket *sock, long reqid);
int i_msgproc_callback_remove (i_msgproc_callback *cb);
int i_msgproc_callback_timeout_callback (i_resource *self, struct i_timer_s *timer, void *passdata);

/* msgproc_handler.c */

i_msgproc_handler* i_msgproc_handler_create ();
void i_msgproc_handler_free (void *hdlrptr);
i_msgproc_handler* i_msgproc_handler_add (i_resource *self, i_socket *sock, int type, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata), void *passdata);
int i_msgproc_handler_set_default (i_resource *self, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata), void *passdata);
int i_msgproc_handler_set_fatal (i_resource *self, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *passdata), void *passdata);
int i_msgproc_handler_remove_by_type (i_resource *self, i_socket *sock, int type);
int i_msgproc_handler_remove (i_resource *self, i_msgproc_handler *hdlr);

/* msgproc_read.c */

int i_msgproc_read_preempt_callback (i_resource *self, i_socket *sock, void *passdata);
int i_msgproc_read_msg_callback (i_resource *self, i_message *msg, void *passdata);

/* msgproc_process.c */

int i_msgproc_process (i_resource *self, i_socket *sock, i_message *msg);
int i_msgproc_process_request (i_resource *self, i_socket *sock, i_message *msg);
int i_msgproc_process_response (i_resource *self, i_socket *sock, i_message *msg);
