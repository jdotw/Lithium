typedef struct l_icmp_item_s
{
  struct i_object_s *obj;
  struct i_metric_s *reachable;
  struct i_metric_s *response;

  unsigned short avail_flag;         /* 0 = dont record avail&rt / 1 = record avail&rt */

} l_icmp_item;

/* icmp.c */

#define ICMPFLAG_SNMPOBJFACT 1

struct i_container_s* l_icmp_cnt ();
struct i_object_s* l_icmp_availobj ();
struct i_object_s* l_icmp_masterobj ();
int l_icmp_enable (i_resource *self, unsigned short flags);
int l_icmp_disable (i_resource *self);

/* icmp_item.c */

l_icmp_item* l_icmp_item_create ();
void l_icmp_item_free (void *itemptr);

/* icmp_objfact.c */

int l_icmp_objfact_fab (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj, struct snmp_pdu *pdu, char *index_oidstr, void *passdata);
int l_icmp_objfact_createmets (i_resource *self, struct i_object_s *obj, l_icmp_item *icmp);
int l_icmp_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int l_icmp_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* icmp_cntform.c */

int l_icmp_cntform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* icmp_objform.c */

int l_icmp_objform (i_resource *self, struct i_entity_s *ent, struct i_form_reqdata_s *reqdata);

/* icmp_echo.c */

#define ECHORES_OK 100
#define ECHORES_FAIL 101
#define ECHORES_TIMEOUT 102
#define ECHORES_SOURCEQUENCH 103
#define ECHORES_REDIRECT 104
#define ECHORES_TIMXCEED 105
#define ECHORES_PARAMPROB 106

typedef struct l_icmp_echo_callback_s
{
  int echo_id;
  int echo_datasize;
  struct i_socket_s *socket;
  struct i_socket_callback_s *socketcb;
  struct i_timer_s *timeout_timer;

  struct timeval req_tv;
  struct timeval resp_tv;

  int (*callback_func) ();
  void *passdata;
} l_icmp_echo_callback;

l_icmp_echo_callback* l_icmp_echo_send (i_resource *self, char *host_str, size_t datasize, time_t timeout_sec, int (*callback_func) (i_resource *self, int result, l_icmp_echo_callback *callback, void *passdata), void *passdata);
int l_icmp_echo_send_socketcb (i_resource *self, struct i_socket_s *socket, void *passdata);
int l_icmp_echo_send_timeout (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_icmp_echo_checksum (u_short *echoptr, int echo_datasize);
l_icmp_echo_callback* l_icmp_echo_callback_create ();
void l_icmp_echo_callback_free (void *cbptr);

/* icmp_refresh.c */

int l_icmp_refresh (i_resource *self, struct i_metric_s *met, int opcode);
int l_icmp_refresh_echocb (i_resource *self, int result, l_icmp_echo_callback *callback, void *passdata);

