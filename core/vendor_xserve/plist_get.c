#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/socket.h"
#include "induction/entity.h"
#include "induction/navtree.h"
#include "induction/navform.h"
#include "induction/hierarchy.h"
#include "induction/device.h"
#include "induction/container.h"
#include "induction/object.h"
#include "induction/metric.h"
#include "induction/value.h"
#include "induction/trigger.h"
#include "induction/triggerset.h"
#include "induction/path.h"
#include "device/avail.h"

#include "osx_server.h"
#include "data.h"
#include "plist.h"

/*
 * Xraid plist retrieval
 */

static v_plist_req *static_req = NULL;
static char *static_session_id = NULL;
struct curl_slist *static_headers = NULL;

v_plist_req* v_plist_get (i_resource *self, char *url, int (*cbfunc) (), void *passdata)
{
  v_plist_req *req;

  /* Check state */
  if (static_req)
  {
    i_printf (1, "v_plist_get called with request already in progress. please report this to support@lithiumcorp.com");
    return NULL; 
  }

  /* Check marking */
  if (self->hierarchy->dev->mark == ENTSTATE_OUTOFSERVICE) return NULL;

  /* Create req struct */
  req = v_plist_req_create ();
  req->cbfunc = cbfunc;
  req->passdata = passdata;
  req->url = strdup (url);
  gettimeofday (&req->req_tv, NULL);

  /* Use cURL */
  req->curl = curl_easy_init();
  if (req->curl)
  {
    /* Determine timeouts */
    int timeout_sec = self->hierarchy->dev->refresh_interval / 2;
    int conn_timeout_sec = self->hierarchy->dev->refresh_interval / 3;
    if (timeout_sec < 10) timeout_sec = 10;
    if (conn_timeout_sec < 10) conn_timeout_sec = 10;

    /* Create cURL Req */
    static char cred_str[1024];
    memset (cred_str, 0, 1024);
    curl_easy_setopt (req->curl, CURLOPT_URL, url);
    curl_easy_setopt (req->curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt (req->curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt (req->curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt (req->curl, CURLOPT_CONNECTTIMEOUT, conn_timeout_sec);
    curl_easy_setopt (req->curl, CURLOPT_TIMEOUT, timeout_sec);
    curl_easy_setopt (req->curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt (req->curl, CURLOPT_FRESH_CONNECT, 1);
    curl_easy_setopt (req->curl, CURLOPT_WRITEFUNCTION, v_plist_get_curlcb);
    curl_easy_setopt (req->curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt (req->curl, CURLOPT_HEADERFUNCTION, v_plist_get_headercb);
    snprintf (cred_str, 1023, "%s:%s", self->hierarchy->dev->username_str, self->hierarchy->dev->password_str);
    curl_easy_setopt (req->curl, CURLOPT_USERPWD, cred_str);
    if (static_session_id)
    {
      curl_slist_free_all (static_headers);
      static_headers = NULL;
      static_headers = curl_slist_append (static_headers, static_session_id);
      curl_easy_setopt (req->curl, CURLOPT_HTTPHEADER, static_headers);
    }

    /* Add to multi handle */
    curl_multi_add_handle (v_plist_handle(), req->curl);

    /* Call perform */
    int handles;
    while (curl_multi_perform (v_plist_handle(), &handles) == CURLM_CALL_MULTI_PERFORM);
  }
  else
  { i_printf (1, "v_plist_get failed to initialise curl handle"); v_plist_req_free (req); return NULL; }

  /* Set static */
  static_req = req;

  return req;
}

size_t v_plist_get_headercb (void *ptr, size_t size, size_t nmemb, void *stream)
{ 
  char *str = (char *) malloc ((size * nmemb)+1);
  snprintf (str, size * nmemb, "%s", (char *) ptr);

  if (strstr(str, "ServerManagerSessionID"))
  {
    if (static_session_id) free (static_session_id);
    static_session_id = strdup (str);
  }

  free (str);

  return size * nmemb;
}

size_t v_plist_get_curlcb (void *ptr, size_t size, size_t nmemb, void *data)
{
  /* Called by cURL internally when there's something to read */
  size_t realsize = size * nmemb;
  v_plist_req *req = data;

  if (req->plistbuf)
  { req->plistbuf = realloc (req->plistbuf, req->plistbuf_size + realsize); }
  else
  { req->plistbuf = malloc (realsize); }
  
  if (req->plistbuf)
  {
    memcpy (req->plistbuf + req->plistbuf_size, ptr, realsize);
    req->plistbuf_size += realsize;
  }
  else
  { req->plistbuf_size = 0; }

  return realsize;
}

int v_plist_get_finished (i_resource *self)
{
  /* Called by our fdset post-processor when the 
   * curl request is finished
   */

  v_plist_req *req = static_req;

  /* Check a request is actually in progress */
  if (!static_req)
  {
    /* Nothing to be done */
    return 0; 
  }

  /* Clear static, to make way for next call */
  curl_multi_remove_handle (v_plist_handle(), req->curl);
  static_req = NULL;

  /* Null terminate the data */
  if (req->plistbuf)
  {
    req->plistbuf = realloc (req->plistbuf, req->plistbuf_size + 1);
    req->plistbuf[req->plistbuf_size] = '\0';
    req->plistbuf_size++;
  }

  /* Check for end of plist */
  if (req->plistbuf && strstr (req->plistbuf, "</plist>"))
  {
    /* End of plist, process it */
    gettimeofday (&req->resp_tv, NULL);

    /* Move to start of XML */
    char *startptr = strstr (req->plistbuf, "\n<?xml");
    if (startptr)
    {
      *startptr = '\0';
      req->plistbuf = startptr + 1;
    }

    /* Create xmlDoc */
    req->plist = xmlReadMemory (req->plistbuf, strlen(req->plistbuf)+1, "plist.xml", "UTF-8", 0);
    req->root_node = xmlDocGetRootElement (req->plist);

    /* Fire callback */
    req->cbfunc (self, req, req->passdata);

    /* Record availability */
    l_avail_record_ok (v_osx_availobj(), &req->req_tv, &req->resp_tv);
  }
  else
  {
    /* Malformed/Missing response */
    l_avail_record_fail (v_osx_availobj());

    /* Check if we're an intel xserve */
    if (v_xserve_intel_extras())
    {
      /* Force a chassis refresh, the device might
       * have been switched off
       */
      v_data_ipmi_item *data = v_data_static_ipmi_item ();
      if (data->chassis)
      { i_entity_refresh (self, ENTITY(data->chassis), REFFLAG_MANUAL, NULL, NULL); }
    }
  }
    
  /* Cleanup */
  curl_easy_cleanup (req->curl);
  v_plist_req_free (req);

  return 0;
}


void v_plist_get_cancel (i_resource *self, v_plist_req *req)
{
  if (static_req)
  {
    curl_multi_remove_handle (v_plist_handle(), static_req->curl);
    curl_easy_cleanup (static_req->curl);
    v_plist_req_free (static_req);
    static_req = NULL;
  }
}


