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
#include <induction/callback.h>
#include <induction/path.h>
#include <induction/loop.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "devtest.h"

/*
 * Xraid plist retrieval
 */

static CURLM *static_handle = NULL;
static i_callback* static_fdset_preprocessor_cb = NULL;
static i_callback* static_fdset_postprocessor_cb = NULL;
static i_timer* static_timer = NULL;
static i_timer* static_snmptimeout_timer = NULL;
static i_list* static_handle_list = NULL;

CURLM* l_devtest_handle ()
{ return static_handle; }

i_list* l_devtest_handle_list ()
{ return static_handle_list; }

int l_devtest_enable (i_resource *self)
{
  /* Xserve G5 Test Enable */
  static_handle = curl_multi_init ();
  static_handle_list = i_list_create ();

  /* SNMP Init */
  const char *type = "lithium_customer";
#ifdef NETSNMP_DS_LIB_DONT_PERSIST_STATE
  netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_DONT_PERSIST_STATE, 1);
#endif
  init_snmp (type);
  
  /* Add i_loop_fdset pre/post processor functions */
  static_fdset_preprocessor_cb = i_loop_fdset_preprocessor_add (self, l_devtest_fdset_preprocessor, NULL);
  if (!static_fdset_preprocessor_cb)
  { i_printf (1, "l_devtest_enable failed to add fdset preprocessor"); return -1; }
  static_fdset_postprocessor_cb = i_loop_fdset_postprocessor_add (self, l_devtest_fdset_postprocessor, NULL);
  if (!static_fdset_postprocessor_cb)
  { i_printf (1, "l_devtest_enable failed to add fdset postprocessor"); return -1; }

  return 0;
}

int l_devtest_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  int max_fd = 0;

  /* 
   * libcurl pre-processing 
   */

  /* Get fds from curl */
  curl_multi_fdset (l_devtest_handle(), read_fdset, write_fdset, except_fdset, &max_fd);

  /* Check if any were added */
  if (max_fd != -1)
  {
    /* Atleast 1 FD was set. The curl docs recommend a low, single-digit
     * timeout be used to ensure proper processing of timeouts etc.
     * We go with a 5 second timeout
     */
    struct timeval timeout = { 5, 0 };
    if (!static_timer)
    { static_timer = i_timer_add (self, timeout.tv_sec, timeout.tv_usec, l_devtest_fdset_timeoutcb, NULL); }
    else
    { i_timer_set_timeout (static_timer, timeout.tv_sec, timeout.tv_usec); }
  }

  /* 
   * libsnmp pre-processing
   */

  int block=1;
  int fds=0;
  struct timeval timeout = {0, 0};
  snmp_select_info (&fds, read_fdset, &timeout, &block);
  if (block == 0)
  {
    /* Add timeout */
    if (!static_snmptimeout_timer)      
    { static_snmptimeout_timer = i_timer_add (self, timeout.tv_sec, timeout.tv_usec, l_devtest_snmptimeout_callback, NULL); }      
    else     
    { i_timer_set_timeout (static_snmptimeout_timer, timeout.tv_sec, timeout.tv_usec); }
  }
  
  if ((fds-1) > max_fd) return fds-1;
  else if (max_fd >= 0) return max_fd;
  else return 0;
}

int l_devtest_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  l_devtest_fdset_perform (self);

  if (select_num > 0)
  { 
    snmp_read (read_fdset); 
  }

  return 0;
}

int l_devtest_fdset_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  static_timer = NULL;
  l_devtest_fdset_perform (self);
  return -1;
}

int l_devtest_snmptimeout_callback (i_resource *self, i_timer *timer, void *passdata)
{
  snmp_timeout ();
  static_snmptimeout_timer = NULL;
  return -1;
}

int l_devtest_fdset_perform (i_resource *self)
{
  /* Call curl_multi_perform */
  int handles;
  while (curl_multi_perform (l_devtest_handle(), &handles) == CURLM_CALL_MULTI_PERFORM);

  /* Check finished handles */
  struct CURLMsg *msg;
  int queue_len;
  while ((msg=curl_multi_info_read(static_handle, &queue_len)))
  {
    if (msg->msg == CURLMSG_DONE)
    {
      l_devtest_servermgr_req *req;
      for (i_list_move_head(static_handle_list); (req=i_list_restore(static_handle_list))!=NULL; i_list_move_next(static_handle_list))
      {
        if (req->curl == msg->easy_handle)
        { 
          l_devtest_servermgr_finished (self, req); 
          break;
        }
      }
    }
  }

  return 0;
}


