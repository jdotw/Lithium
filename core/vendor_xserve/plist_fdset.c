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
#include <induction/path.h>
#include "device/avail.h"

#include "osx_server.h"
#include "plist.h"

/*
 * Xraid plist retrieval - Uses libcurl
 */

static i_timer* static_timer = NULL;

int v_plist_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  int max_fd;

  /* Get fds from curl */
  curl_multi_fdset (v_plist_handle(), read_fdset, write_fdset, except_fdset, &max_fd);

  /* Check if any were added */
  if (max_fd != -1)
  {
    /* Atleast 1 FD was set. The curl docs recommend a low, single-digit
     * timeout be used to ensure proper processing of timeouts etc.
     * We go with a 5 second timeout
     */
    
    struct timeval timeout = { 5, 0 };
    if (!static_timer)
    { static_timer = i_timer_add (self, timeout.tv_sec, timeout.tv_usec, v_plist_fdset_timeoutcb, NULL); }
    else
    { i_timer_set_timeout (static_timer, timeout.tv_sec, timeout.tv_usec); }

    return max_fd;
  }
  else
  { return 0; }
}

int v_plist_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data)
{
  v_plist_fdset_perform (self);
  return 0;
}

int v_plist_fdset_timeoutcb (i_resource *self, i_timer *timer, void *passdata)
{
  static_timer = NULL;
  v_plist_fdset_perform (self);
  return -1;
}

int v_plist_fdset_perform (i_resource *self)
{
  /* Call curl_multi_perform */
  int handles;
  while (curl_multi_perform (v_plist_handle(), &handles) == CURLM_CALL_MULTI_PERFORM);

  /* Check in-progress handle count */
  if (handles == 0)
  {
    /* All transfers are done! */
    v_plist_get_finished (self);
  }

  return 0;
}
