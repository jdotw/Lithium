#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <libxml/parser.h>

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
#include "device/snmp.h"

#include "plist.h"

/*
 * Xraid plist request struct
 */

v_plist_req* v_plist_req_create ()
{
  v_plist_req *req;
  req = (v_plist_req *) malloc (sizeof(v_plist_req));
  memset (req, 0, sizeof(v_plist_req));
  return req;
}

void v_plist_req_free (void *reqptr)
{
  v_plist_req *req = reqptr;
  if (!req) return;
  if (req->url) free (req->url);
  if (req->plistbuf) free (req->plistbuf);
  if (req->plist) xmlFreeDoc (req->plist);
  free (req);
}

