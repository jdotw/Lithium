#include <curl/curl.h>
#include <libxml/parser.h>
#include <curl/types.h>
#include <curl/easy.h>

/* devtest.c */

CURLM* l_devtest_handle ();
i_list* l_devtest_handle_list ();
int l_devtest_enable (i_resource *self);
int l_devtest_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int l_devtest_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int l_devtest_fdset_timeoutcb (i_resource *self, i_timer *timer, void *passdata);
int l_devtest_fdset_perform (i_resource *self);
int l_devtest_snmptimeout_callback (i_resource *self, i_timer *timer, void *passdata);

/* devtest_xraid.c */

typedef struct l_devtest_xraid_req_s
{
  char *action;
  char *plistout;

  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
  struct i_timer_s *timeout_timer;
  char *plistbuf;

  int (*cbfunc) ();
  void *passdata;
} l_devtest_xraid_req;

#define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

l_devtest_xraid_req* l_devtest_xraid_req_create ();
void l_devtest_xraid_req_free (void *reqptr);
int l_devtest_xraid (i_resource *self, struct i_device_s *dev, int (*cbfunc) (i_resource *self, int result, void *passdata), void *passdata);
int l_devtest_xraid_conncb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_devtest_xraid_sockreadcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_devtest_xraid_timeout (i_resource *self, struct i_timer_s *timer, void *passdata);
size_t base64_encode_alloc (const char *in, size_t inlen, char **out);
void base64_encode (const char *in, size_t inlen, char *out, size_t outlen);
char* l_devtest_xraid_acp_crypt (char *password_str);

/* devtest_servermgr.c */

typedef struct l_devtest_servermgr_req_s
{
  char *url;

  CURL *curl;

  char *plistbuf;
  size_t plistbuf_size;

  xmlDocPtr plist;
  xmlNodePtr root_node;

  struct timeval req_tv;
  struct timeval resp_tv;

  int (*cbfunc) ();
  void *passdata;
} l_devtest_servermgr_req;

int l_devtest_servermgr (i_resource *self, struct i_device_s *dev, int (*cbfunc) (), void *passdata);
size_t l_devtest_servermgr_curlcb (void *ptr, size_t size, size_t nmemb, void *data);
int l_devtest_servermgr_finished (i_resource *self, l_devtest_servermgr_req *req);

/* devtest_xintel.c */

typedef struct l_devtest_xintel_proc_s
{
  int pid;
  int readfd;
  int writefd;

  struct i_socket_s *sock;
  struct i_socket_callback_s *sockcb;
  struct i_timer_s *timeout_timer;

  struct l_devtest_xintel_cmd_s *curcmd;
  int timeout_count;
} l_devtest_xintel_proc;

typedef struct l_devtest_xintel_cmd_s
{
  char *command_str;            /* Command to be executed */
  char *args_str;               /* Optional args */
  char *output_str;             /* The output from ipmitool */
  void *raw_data;               /* Raw data */
  size_t raw_datasize;         /* Raw data size */
  struct i_callback_s *cb;      /* Callback to be called after execution */
  struct timeval req_time;      /* Request time stamp */
  struct timeval resp_time;     /* Response time stamp */
} l_devtest_xintel_cmd;

l_devtest_xintel_proc* l_devtest_xintel_proc_create ();
void l_devtest_xintel_proc_free (void *procptr);
l_devtest_xintel_cmd* l_devtest_xintel_cmd_create ();
void l_devtest_xintel_cmd_free (void *cmdptr);
int l_devtest_xintel (i_resource *self, struct i_device_s *dev, int (*cbfunc) (i_resource *self, int result, void *passdata), void *passdata);
int l_devtest_xintel_socketcb (i_resource *self, struct i_socket_s *sock, void *passdata);
int l_devtest_xintel_timeoutcb (i_resource *self, i_timer *timer, void *passdata);
l_devtest_xintel_proc *l_devtest_xintel_spawn (i_resource *self, struct i_device_s *dev);

/* devtest_snmp.h */

int l_devtest_snmp (i_resource *self, struct i_device_s *dev, int (*cbfunc) (i_resource *self, int result, void *passdata), void *passdata);
int l_devtest_snmp_parse ();
int l_devtest_snmp_process_pdu ();
