/* Message Types */

#define MSG_UNKNOWN 0                       /* Basic types 0-19 */
#define MSG_RES_ID 1
#define MSG_TERMINATE 2
#define MSG_RES_REMOVE 3
#define MSG_RES_SPAWN 4
#define MSG_RES_TERMINATE 5
#define MSG_RES_STATUS 6                    /* Request a resources status */
#define MSG_RES_STATUS_RESP 7
#define MSG_RES_DESTROY 8                   /* Terminate and remove */
#define MSG_RES_PING 9                      /* Request an echo from the res */
#define MSG_RES_READY 10                    /* Signals a resource is ready */
#define MSG_HEARTBEAT 11                    /* Heartbeat message */

#define MSG_FORM_GET 20                     /* Form related 20-29 */
#define MSG_FORM_GET_RESP 21                     /* Form related 20-29 */
#define MSG_FORM_SEND 22
#define MSG_FORM_SEND_RESP 23
#define MSG_FORM_GET_V2 24

#define MSG_HUB_CREATE 30                     /* Hub related 30-39 */
#define MSG_HUB_REMOVE 31                     /* Remove a hub */
#define MSG_HUB_MSG 32                        /* A message to be broadcast sent to the hub controller */
#define MSG_HUB_BROADCAST 33                  /* A broadcast message sent from the hub controller to subscribers */
#define MSG_HUB_SUBSCRIBE 34                  /* A subscription request */
#define MSG_HUB_UNSUBSCRIBE 35                /* A request to unsubscribe */

#define MSG_IF_GET 40                         /* Interface related 40-49 */
#define MSG_IF_GET_RESP 41                    /* Used to communication with the status module */
#define MSG_IF_GET_ALL 42
#define MSG_IF_GET_STATE 43

/* DEFUNCT */
#define MSG_STATUS_GET_INT 50                 /* Status related, 50-59 */
#define MSG_STATUS_GET_INT_RESP 51            /* Status related, 50-59 */
#define MSG_STATUS_GET_INT_V2 52              /* Status related, 50-59 */
#define MSG_STATUS_UPDATE 53
/* END DEFUNCT */

#define MSG_AUTH_VERIFY 60                    /* Authentication related 60-69 */
#define MSG_AUTH_DENIED 61
#define MSG_AUTH_REQUIRED 62                  /* Queries whether or not credentials are required */

#define MSG_IP_REGISTER 70                    /* IP Registry Related 70-79 */
#define MSG_IP_SETNETMASK 71
#define MSG_IP_SETIFACE 72
#define MSG_IP_DEREGISTER 73

#define MSG_NOTIFICATION 80                   /* Notification codes 80-89 */

#define MSG_SMS_SEND 90                       /* SMS codes */

#define MSG_INV_REGISTER 100                  /* Inventory related 100-101 */
#define MSG_INV_DEREGISTER 101
#define MSG_INV_STATIC_REGALL 102

#define MSG_INC_REPORT 110                    /* Incident related 110-119 */
#define MSG_INC_CLEAR 111
#define MSG_INC_CLEARALL 112
#define MSG_INC_TRANSITION 113

#define MSG_SITESTATE 120                     /* FIX HACK Site state related 120-129 */

#define MSG_XML_GET 130                       /* XML Related 130-139 */
#define MSG_XML_SEND 131

#define MSG_SVC_REGISTER 140                  /* Service Registry 140-149 */
#define MSG_SVC_DEREGISTER 141

#define MSG_XSAN_REGISTER 150                  /* Service Registry 150-159 */
#define MSG_XSAN_DEREGISTER 151

#define MSG_LUN_REGISTER 160                  /* Service Registry 160-159 */
#define MSG_LUN_DEREGISTER 161

#define MSG_DEVNORMAL_REPORT 170             /* Devices 170-179 */

#define MSG_PROC_REGISTER 180                  /* Service Registry 180-189 */
#define MSG_PROC_DEREGISTER 181

#define MSG_OK 4096                           /* Info codes 100-199 */
#define MSG_FAIL 4097
#define MSG_NO_DATA 4098
#define MSG_MODULE_NOT_FOUND 4099
#define MSG_ROUTE_FAILED 4100
#define MSG_DENIED 4101

/* Message Parameters */

#define MAX_MSG_TTL 50                      /* Maximum of 50 hops before a message is destroyed */

/* Message flags */

#define MSG_FLAG_REQ 2048
#define MSG_FLAG_RESP 4096 
#define MSG_FLAG_ACK 8192
#define MSG_FLAG_ERROR 16384
#define MSG_FLAG_DENIED 32768

/* MISC */

#define MSG_TIMEOUT_SEC 45
#define MSG_TIMEOUT_USEC 0

/* Structs */

typedef struct i_message_s
{
  int type;
  int flags;

  long msgid;                               /* The unique/random ID of this message */
  long reqid;                               /* The request msgid that this message pertains to */

  struct i_authentication_s *auth;
  
  i_resource_address *src;
  i_resource_address *dst;

  unsigned int datasize;
  void *data;
	
} i_message;

typedef struct i_message_read_cbdata_s
{
  int (*callback_func) (i_resource *self, i_message *msg, void *passdata);
  void *passdata;
  struct i_socket_data_s *size_op;
  struct i_socket_data_s *data_op;
  int msg_datasize;
} i_message_read_cbdata;

/* message.c */

i_message* i_message_create ();
void i_message_free (void *msgptr);
i_message* i_message_duplicate (i_message *msg);
int i_message_address_set (i_message *msg, i_resource_address *src, i_resource_address *dst);
int i_message_verify_self_destination (i_resource *self, i_message *msg);

/* message_send.c */

long i_message_send (i_resource *self, int type, void *data, int datasize, i_resource_address *dst, int flags, long reqid);

/* message_write.c */

int i_message_write_sockfd (i_resource *self, struct i_socket_s *sock_s, i_message *msg);

/* message_read.c */

i_message_read_cbdata* i_message_read_cbdata_create ();
void i_message_read_cbdata_free (void *cbdataptr);
int i_message_read (i_resource *self, struct i_socket_s *sock, int (*callback_func) (i_resource *self, i_message *msg, void *passdata), void *passdata);
int i_message_read_socketread_callback (i_resource *self, struct i_socket_s *sock, struct i_socket_data_s *op, int result, void *passdata);
void i_message_read_cancel (struct i_socket_s *sock);

