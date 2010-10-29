#include <sys/un.h>
#include <netinet/in.h>

#define SOCKET_LISTEN 0
#define SOCKET_CONNECT 1

#define SOCKET_RESULT_SUCCESS 1
#define SOCKET_RESULT_TIMEOUT 2
#define SOCKET_RESULT_FAILED -1

#define SOCKET_GENERIC 0
#define SOCKET_TCP 1
#define SOCKET_UDP 2
#define SOCKET_UNIX 3

#define SOCKET_TCP_CONNECT_TIMEOUT_SEC 30

typedef struct i_socket_s
{
  int sockfd;
  int type;
  
  struct sockaddr_in addr_in;       /* Inet address struct */
  struct sockaddr_un addr_un;       /* Unix-domain address struct */

  struct i_list_s *read_pending;             /* Pending read operations */
  struct i_socket_callback_s *read_socket_callback;
  struct i_socket_callback_s *read_preempt_callback;
  
  struct i_list_s *write_pending;            /* Pending write operations */
  struct i_socket_callback_s *write_socket_callback;
  struct i_socket_callback_s *write_preempt_callback;

  struct i_socket_connect_data_s *connect_data;
  struct i_message_read_cbdata_s *msg_read_cbdata;

  struct i_msgproc_s *msgproc;
} i_socket;

typedef struct i_socket_connect_data_s
{
  struct i_socket_s *socket;
  struct i_socket_callback_s *socket_callback;
  struct i_timer_s *timeout_timer;

  int (*callback_func) ();
  void *passdata;
} i_socket_connect_data;

typedef struct i_socket_data_s
{
  struct i_socket_s *socket;        /* Socket being used */
  void *data;                       /* Data being sent/recvd */
  unsigned int datasize;                     /* Total datasize */

  unsigned int offset;                       /* Current offset */

  int (*callback_func) ();          /* Callback */
  void *passdata;                   /* Callback data */

  struct timeval queued_tstamp;            /* The time the write op was queued */
} i_socket_data;

/* socket.c */

i_socket* i_socket_create ();
void i_socket_free (void *socketptr);
i_socket_data* i_socket_data_create ();
void i_socket_data_free (void *dataptr);
i_socket_connect_data* i_socket_connect_data_create ();
i_socket_data* i_socket_data_duplicate (i_socket_data *orig);
void i_socket_connect_data_free (void *dataptr);
void i_socket_connect_cancel (i_socket *sock);

/* socket_tcp.c */

i_socket* i_socket_create_tcp (i_resource *self, char *address, int port, int (*callback_func) (i_resource *self, i_socket *sock, void *passdata), void *passdata);
int i_socket_create_tcp_socket_callback (i_resource *self, i_socket *sock, void *passdata);
int i_socket_create_tcp_timeout_callback (i_resource *self, struct i_timer_s *timer, void *passdata);
i_socket* i_socket_accept_tcp (int listenfd);

/* socket_unix.c */
i_socket* i_socket_create_unix (char *path, int type);
i_socket* i_socket_accept_unix (int listenfd);

/* socket_read.c */
i_socket_data* i_socket_read (i_resource *self, i_socket *sock, int datasize, int (*callback_func) (i_resource *self, i_socket *sock, i_socket_data *data, int result, void *passdata), void *passdata);
int i_socket_read_socket_callback (i_resource *self, i_socket *sock, void *passdata);
int i_socket_read_cancel (i_socket_data *op);

/* socket_write.c */
i_socket_data* i_socket_write (i_resource *self, i_socket *sock, void *data, int datasize, int (*callback_func) (i_resource *self, i_socket *sock, int result, void *passdata), void *passdata);
int i_socket_write_socket_callback (i_resource *self, i_socket *sock, void *passdata);
int i_socket_write_cancel (i_socket_data *op);

/* socket_callback.c */

#define SOCKET_CALLBACK_DISCARD 0
#define SOCKET_CALLBACK_READ 1
#define SOCKET_CALLBACK_READ_PREEMPT 2
#define SOCKET_CALLBACK_WRITE 3
#define SOCKET_CALLBACK_WRITE_PREEMPT 4
#define SOCKET_CALLBACK_EXCEPT 5

typedef struct i_socket_callback_s
{
  i_resource *resource;
  int type;
  i_socket *socket;
  int (*callback_func) (i_resource *self, i_socket *sock, void *passdata);
  void *passdata;
} i_socket_callback;

int i_socket_callback_init (i_resource *self);
i_socket_callback* i_socket_callback_create ();
void i_socket_callback_free (void *cb_ptr);
struct i_list_s* i_socket_callback_list ();
int i_socket_callback_list_free ();
i_socket_callback* i_socket_callback_add (i_resource *self, int type, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, void *passdata), void *passdata);
int i_socket_callback_remove (i_socket_callback *callback);
int i_socket_callback_remove_by_socket (i_socket *sock);

/* socket_callback_fdset.c */
void i_socket_callback_fdset_invalidate ();
int i_socket_callback_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int i_socket_callback_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *passdata);


