#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

#include <induction.h>
#include <induction/callback.h>
#include <induction/socket.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <lithium/avail.h>

#include "xserve_intel.h"
#include "ipmitool.h"

/* IPMItool Apple Proprietary */

v_ipmitool_apple_req* v_ipmitool_apple_req_create ()
{
  v_ipmitool_apple_req *req = malloc (sizeof(v_ipmitool_apple_req));
  memset (req, 0, sizeof(v_ipmitool_apple_req));
  return req;
}

void v_ipmitool_apple_req_free (void *reqptr)
{
  v_ipmitool_apple_req *req = reqptr;
  if (req->data) free (req->data);
  if (req->type_hex) free (req->type_hex);
  if (req->item_hex) free (req->item_hex);
  free (req); 
}

