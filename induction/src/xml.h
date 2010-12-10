typedef struct i_xml_s
{
  xmlDocPtr doc;
  char *raw_data;
  
  unsigned short state;
  unsigned int notready_count;

  struct i_xml_request_s *req;
} i_xml;

typedef struct i_xml_request_s
{
  unsigned short type;
  char *xml_name;
  struct i_resource_address_s *resaddr;
  struct i_entity_address_s *entaddr;
  time_t ref_sec;
  struct i_xml_s *xml_in;
  struct i_xml_s *xml_out;

  int (*cbfunc) ();
  void *passdata;

  int (*xml_func) ();

  struct timeval req_tv;
  struct i_socket_s *socket;
  struct i_message_s *msg_in;
  struct i_authentication_s *auth;
  struct i_msgproc_callback_s *msg_callback;
} i_xml_request;

#define XMLSTATE_DELIVERABLE 1

#define XML_REQ_GET 1
#define XML_REQ_SEND 2

#define XML_RESULT_OK 1
#define XML_RESULT_ERROR 2
#define XML_RESULT_DENIED 3

/* xml.c */
i_xml* i_xml_create ();
void i_xml_free (void *xmlptr);

/* xml_data.c */
char* i_xml_data (i_xml *xml, int *datasizeptr);
i_xml* i_xml_struct (char *data, int datasize, char *xml_name);

/* xml_deliver.c */
int i_xml_deliver (i_resource *self, i_xml_request *reqdata);

/* xml_func.c */
void* i_xml_func_get (i_resource *self, char *xml_name_raw, int *resultptr);

/* xml_get.c */
i_xml_request* i_xml_get (i_resource *self, struct i_resource_address_s *resaddr, struct i_entity_address_s *entaddr, char *xml_name, time_t ref_sec, struct i_xml_s *xml_out, int (*cbfunc) (), void *passdata);
int i_xml_get_msgcb (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);
int i_xml_get_handler (i_resource *self, struct i_socket_s *sock, struct i_message_s *msg, void *passdata);

/* xml_handler.c */
int i_xml_handler_failed (i_resource *self, struct i_message_s *msg, i_xml_request *reqdata);

/* xml_request.c */
i_xml_request* i_xml_request_create ();
void i_xml_request_free (void *reqptr);

/* xml_auth.c */
i_xml* i_xml_denied ();
