#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

#include "induction.h"
#include "induction/list.h"
#include "induction/timer.h"
#include "induction/form.h"
#include "induction/auth.h"
#include "induction/cement.h"
#include "induction/loop.h"
#include "induction/callback.h"
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
#include "plist.h"

/*
 * Xraid plist retrieval - Uses libcurl
 */

static i_callback* static_fdset_preprocessor_cb = NULL;
static i_callback* static_fdset_postprocessor_cb = NULL;
static CURLM *static_handle;
static char* static_plist_ip = NULL;
extern i_resource *global_self;

CURLM* v_plist_handle ()
{ return static_handle; }

char* v_plist_ip ()
{
  if (global_self->hierarchy->dev->profile_str && strlen(global_self->hierarchy->dev->profile_str) > 1)
  { return global_self->hierarchy->dev->profile_str; }
  else if (static_plist_ip)
  { return static_plist_ip; }
  else 
  { return global_self->hierarchy->dev->ip_str; }
}

void v_plist_set_ip (char *ip_str)
{
  if (static_plist_ip) free (static_plist_ip);
  static_plist_ip = strdup (ip_str);
}

/* Enable */

int v_plist_enable (i_resource *self)
{
  /* Create static handle */
  static_handle = curl_multi_init ();
  //curl_multi_setopt (static_handle, CURLMOPT_PIPELINING, 1);
  
  /* Add i_loop_fdset pre/post processor functions */
  static_fdset_preprocessor_cb = i_loop_fdset_preprocessor_add (self, v_plist_fdset_preprocessor, NULL);
  if (!static_fdset_preprocessor_cb)
  { i_printf (1, "v_plist_enable failed to add fdset preprocessor"); return -1; }
  static_fdset_postprocessor_cb = i_loop_fdset_postprocessor_add (self, v_plist_fdset_postprocessor, NULL);
  if (!static_fdset_postprocessor_cb)
  { i_printf (1, "v_plist_enable failed to add fdset postprocessor"); return -1; }

  return 0;
}
