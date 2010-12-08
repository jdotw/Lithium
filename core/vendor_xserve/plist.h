#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

typedef struct v_plist_req_s
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
} v_plist_req;

/* plist.c */
CURLM* v_plist_handle ();
char* v_plist_ip ();
void v_plist_set_ip (char *ip_str);
int v_plist_enable (i_resource *self);

/* plist_get.c */
v_plist_req* v_plist_get (i_resource *self, char *url, int (*cbfunc) (), void *passdata);
size_t v_plist_get_curlcb (void *ptr, size_t size, size_t nmemb, void *data);
int v_plist_get_finished (i_resource *self);
void v_plist_get_cancel (i_resource *self, v_plist_req *req);
size_t v_plist_get_headercb (void *ptr, size_t size, size_t nmemb, void *stream);

/* plist_fdset.c */
int v_plist_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int v_plist_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data);
int v_plist_fdset_timeoutcb (i_resource *self, i_timer *timer, void *passdata);
int v_plist_fdset_perform (i_resource *self);

/* plist_req.c */
v_plist_req* v_plist_req_create ();
void v_plist_req_free (void *reqptr);

/* plist_util.c */
char* v_plist_data_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);
xmlNodePtr v_plist_node_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);
int v_plist_bool_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);
int v_plist_int_from_dict (xmlDocPtr plist, xmlNodePtr dictNode, char *key);

