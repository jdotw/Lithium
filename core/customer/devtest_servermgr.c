#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <induction.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/auth.h>
#include <induction/cement.h>
#include <induction/socket.h>
#include <induction/entity.h>
#include <induction/navtree.h>
#include <induction/navform.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <induction/container.h>
#include <induction/object.h>
#include <induction/metric.h>
#include <induction/value.h>
#include <induction/trigger.h>
#include <induction/triggerset.h>
#include <induction/path.h>

#include "devtest.h"

/*
 * Servermgr plist retrieval
 */

l_devtest_servermgr_req* l_devtest_servermgr_req_create ()
{
  l_devtest_servermgr_req *req;
  req = (l_devtest_servermgr_req *) malloc (sizeof(l_devtest_servermgr_req));
  memset (req, 0, sizeof(l_devtest_servermgr_req));
  return req;
}

void l_devtest_servermgr_req_free (void *reqptr)
{
  l_devtest_servermgr_req *req = reqptr;
  if (!req) return;
  if (req->url) free (req->url);
  if (req->plistbuf) free (req->plistbuf);
  if (req->plist) xmlFreeDoc (req->plist);
  free (req);
}

int l_devtest_servermgr (i_resource *self, i_device *dev, int (*cbfunc) (), void *passdata)
{
  l_devtest_servermgr_req *req;

  req = l_devtest_servermgr_req_create ();
  req->cbfunc = cbfunc;
  req->passdata = passdata;
  asprintf (&req->url, "https://%s:311/", dev->ip_str);

  /* Use cURL */
  req->curl = curl_easy_init();
  if (req->curl)
  {
    /* Create cURL Req */
    static char cred_str[1024];
    curl_easy_setopt (req->curl, CURLOPT_URL, req->url);
    curl_easy_setopt (req->curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt (req->curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt (req->curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt (req->curl, CURLOPT_CONNECTTIMEOUT, 20);
    curl_easy_setopt (req->curl, CURLOPT_TIMEOUT, 30);
    curl_easy_setopt (req->curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt (req->curl, CURLOPT_WRITEFUNCTION, l_devtest_servermgr_curlcb);
    curl_easy_setopt (req->curl, CURLOPT_WRITEDATA, req);
    snprintf (cred_str, 1023, "%s:%s", dev->username_str, dev->password_str);
    curl_easy_setopt (req->curl, CURLOPT_USERPWD, cred_str);

    /* Add to multi handle */
    curl_multi_add_handle ((l_devtest_handle()), req->curl);
    i_list_enqueue (l_devtest_handle_list(), req);

    /* Call perform */
    int handles;
    while (curl_multi_perform (l_devtest_handle(), &handles) == CURLM_CALL_MULTI_PERFORM);
  }
  else
  { i_printf (1, "l_devtest_servermgr failed to initialise curl handle"); l_devtest_servermgr_req_free (req); return -1; }

  return 0;
}

size_t l_devtest_servermgr_curlcb (void *ptr, size_t size, size_t nmemb, void *data)
{
  /* Called by cURL internally when there's something to read */
  size_t realsize = size * nmemb;
  l_devtest_servermgr_req *req = data;

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

int l_devtest_servermgr_finished (i_resource *self, l_devtest_servermgr_req *req)
{
  /* Called by our fdset post-processor when the 
   * curl request is finished
   */

  /* Clear curl easy handle */
  curl_multi_remove_handle (l_devtest_handle(), req->curl);

  /* Null terminate the data */
  if (req->plistbuf)
  {
    req->plistbuf = realloc (req->plistbuf, req->plistbuf_size + 1);
    req->plistbuf[req->plistbuf_size] = '\0';
    req->plistbuf_size++;
  }

  /* Check for end of plist */
  i_printf (0, "l_devtest_servermgr_finished has req->plistbuf as %s", req->plistbuf);
  if (req->plistbuf && strlen(req->plistbuf) > 1 && strstr(req->plistbuf, "Server Admin module list"))
  {
    /* Success! */
    req->cbfunc (self, 1, req->passdata);
  }
  else
  {
    /* Failed */ 
    req->cbfunc (self, 0, req->passdata); 
  }
    
  /* Cleanup */
  curl_easy_cleanup (req->curl);
  if (i_list_search(l_devtest_handle_list(), req) == 0)
  { i_list_delete (l_devtest_handle_list()); }
  l_devtest_servermgr_req_free (req);

  return 0;
}

