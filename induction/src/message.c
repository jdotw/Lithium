#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "message.h"
#include "auth.h"

/* Struct Manipulation */

i_message* i_message_create ()
{
  i_message *msg;

  msg = (i_message *) malloc (sizeof(i_message));
  if (!msg)
  { i_printf (1, "i_message_create_blank failed to malloc message struct"); return NULL; }
  memset (msg, 0, sizeof(i_message));

  return msg;
}

void i_message_free (void *msgptr)
{
  i_message *msg = msgptr;

  if (!msg) return;

  if (msg->dst) i_resource_free_address (msg->dst);
  if (msg->src) i_resource_free_address (msg->src);
  if (msg->data) free (msg->data);
  if (msg->auth) i_authentication_free (msg->auth);

  free (msg);
}

i_message* i_message_duplicate (i_message *msg)
{
  i_message *dup;

  dup = (i_message *) malloc (sizeof(i_message));
  if (!msg)
  {
    i_printf (1, "i_message_duplicate failed to malloc dup");
    return NULL;
  }
  memset (dup, 0, sizeof(i_message));

  dup->type = msg->type;
  dup->flags = msg->flags;
  dup->msgid = msg->msgid;
  dup->reqid= msg->reqid;
  if (msg->auth)
  { 
    dup->auth = i_authentication_duplicate (msg->auth);
    if (!dup->auth)
    { i_printf (1, "i_message_duplicate failed to duplicate msg->auth"); i_message_free (dup); return NULL; }
  }
  if (msg->src) dup->src = i_resource_address_duplicate (msg->src);
  if (msg->dst) dup->dst = i_resource_address_duplicate (msg->dst);
  dup->datasize = msg->datasize;
  if (msg->data)
  { 
    dup->data = malloc (msg->datasize);
    memcpy (dup->data, msg->data, msg->datasize);
  }
  
  return dup;
}

/* Addressing */

int i_message_address_set (i_message *msg, i_resource_address *src, i_resource_address *dst)
{
  if (!msg) return -1;

  if (src)
  {
    if (msg->src)
    { i_resource_free_address (src); }
    msg->src = i_resource_address_duplicate (src);
  }

  if (dst)
  {
    if (msg->dst) 
    { i_resource_free_address (dst); }
    msg->dst = i_resource_address_duplicate (dst);
  }

  return 0;
}

int i_message_verify_self_destination (i_resource *self, i_message *msg)    /* Returns 0 if the message is destined for 'self' */
{
  if (!self || !msg) return -1;                                   /* Error! */

  if (self->type == RES_EXEC) return 0;
  
  if (!msg->dst || !msg->src) 
  { 
    /* Non-addressed, must be local */
    i_printf (2, "i_message_verify_self_destination found local message"); 
    return 0; 
  }

  i_printf (2, "i_message_verify_self_destination verifying %s:%s:%i:%i:%s (msg) and %s:%s:%i:%i:%s (self)",
    msg->dst->plexus, msg->dst->node, msg->dst->type, msg->dst->ident_int, msg->dst->ident_str,
    self->plexus, self->node, self->type, self->ident_int, self->ident_str);

  if (i_resource_address_compare(msg->dst, RES_ADDR(self)) != 0) return -1;
  
  i_printf (2, "i_message_verify_self_destination verified destination is self");

  return 0;       /* All match! */
}

