typedef struct v_plist_req_s
{
  char *action;
  char *plistout;
  
  struct i_socket_s *sock;
  char *plistbuf;
  xmlDocPtr plist;
  xmlNodePtr root_node;

  struct timeval req_tv;
  struct timeval resp_tv;

  int (*cbfunc) ();
  void *passdata;
} v_plist_req;

#define BASE64_LENGTH(inlen) ((((inlen) + 2) / 3) * 4)

/* plist.c */
v_plist_req* v_plist_get (i_resource *self, char *action, char *plistout, int (*cbfunc) (), void *passdata);
void v_plist_get_cancel (i_resource *self, v_plist_req *req);
int v_plist_get_conncb (i_resource *self, struct i_socket_s *sock, void *passdata);
int v_plist_get_sockreadcb (i_resource *self, struct i_socket_s *sock, void *passdata);
char* v_plist_acp_crypt (char *password_str);
void base64_encode (const char *in, size_t inlen, char *out, size_t outlen);
size_t base64_encode_alloc (const char *in, size_t inlen, char **out);


/* plist_req.c */
v_plist_req* v_plist_req_create ();
void v_plist_req_free (void *reqptr);

/* plist_util.c */
char* v_plist_data_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);
xmlNodePtr v_plist_node_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);
int v_plist_bool_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);
int v_plist_int_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);
