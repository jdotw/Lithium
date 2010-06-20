#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#include <induction.h>
#include <induction/callback.h>
#include <induction/socket.h>
#include <induction/list.h>
#include <induction/timer.h>
#include <induction/form.h>
#include <induction/cement.h>
#include <induction/entity.h>
#include <induction/hierarchy.h>
#include <induction/device.h>
#include <lithium/avail.h>

#include "xserve_intel.h"
#include "ipmitool.h"

/* IPMItool Apple Proprietary */

v_ipmitool_apple_req* v_ipmitool_apple_get (i_resource *self, char *type_hex, char *item_hex, int (*cbfunc) (), void *passdata)
{
  /* Check marking */
  if (self->hierarchy->dev->mark == ENTSTATE_OUTOFSERVICE) return NULL;

  /* Create req */
  v_ipmitool_apple_req *req = v_ipmitool_apple_req_create ();
  if (!req) return NULL;
  req->cbfunc = cbfunc;
  req->passdata = passdata;
  req->type_hex = strdup (type_hex);
  req->item_hex = strdup (item_hex);

  /* Fire off first request */
  char *args_str;
  asprintf (&args_str, "0x36 0x02 0x00 %s %s 0x00", type_hex, item_hex); 
  req->curcmd = v_ipmitool_exec (self, "raw", args_str, 0, v_ipmitool_apple_ipmicb, req);
  free (args_str);
  if (!req->curcmd)
  {
    i_printf (1, "v_ipmitool_apple_get failed to execute first ipmi raw command");
    v_ipmitool_apple_req_free (req);
    return NULL;
  }

  return req;  
}

int v_ipmitool_apple_ipmicb (i_resource *self, v_ipmitool_cmd *cmd, int result, void *passdata)
{
  v_ipmitool_apple_req *req = passdata;

  /* NULL curcmd */
  req->curcmd = NULL;

  /* Check state */
  if (result == IPMIRESULT_OK && cmd->raw_data && cmd->raw_datasize > 1)
  {
    /* Offset */
    int left_offset = 1;    /* Start of data offset */
    int right_offset = 0;   /* End of data offset */

    /* Result is good, append to data */
    if (req->data)
    {
      req->data = realloc (req->data, req->datasize + (cmd->raw_datasize - (left_offset + right_offset)));
    }
    else
    {
      req->data = malloc (cmd->raw_datasize - (left_offset + right_offset));
    }
    memcpy ((req->data + req->datasize), (cmd->raw_data + left_offset), (cmd->raw_datasize - (left_offset + right_offset)));
    req->datasize += (cmd->raw_datasize - (left_offset + right_offset));
  }
  else
  {
    /* Error occurred */
    if (req->cbfunc)
    { req->cbfunc (self, req, result, req->passdata); }

    /* Free req */
    v_ipmitool_apple_req_free (req);

    return -1;
  }

  /*
   * Check to see if the data is all 0's, skipping the first and last bytes 
   * which will be 0x11 and 0x0a respectively due to line feeds, etc
   */

  unsigned int i;
  int has_data = 0;
  for (i=1; i < (cmd->raw_datasize - 1); i++)
  {
    char *data = cmd->raw_data;
    if (data[i] != '\0')
    { 
      has_data = 1;
      break;
    }
  }

  /* If the packet has data, continue to the next data block */
  if (has_data)
  {
    /* Get next data block */
    char *args_str;
    req->index++;
    asprintf (&args_str, "0x36 0x02 0x00 %s %s %.2x", req->type_hex, req->item_hex, req->index); 
    req->curcmd = v_ipmitool_exec (self, "raw", args_str, 0, v_ipmitool_apple_ipmicb, req);
    free (args_str);
    if (!req->curcmd)
    {
      i_printf (1, "v_ipmitool_apple_get failed to execute subsequent ipmi raw command");
      if (req->cbfunc)
      { req->cbfunc (self, req, IPMIRESULT_FAILED, req->passdata); }
      
      v_ipmitool_apple_req_free (req);
      return -1;
    }
  }
  else
  {
    /* Packet is empty, fire callback */
    if (req->cbfunc)
    { req->cbfunc (self, req, result, req->passdata); }

    /* Free req */
    v_ipmitool_apple_req_free (req);

    return 0;
  }
  
  return 0;
}

void v_ipmitool_apple_dump (v_ipmitool_apple_req *req)
{
    unsigned int i;
    for (i=0; i < req->datasize; i++)
    {
      i_printf (0, "data[%i] = %c (0x%2x)", i, isprint(req->data[i]) ? req->data[i] : '.' , req->data[i]);
    }
}
