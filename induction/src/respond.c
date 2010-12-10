#include <stdlib.h>

#include "induction.h"
#include "message.h"
#include "respond.h"

/*
 * The respond functions are used to response to requests 
 * in the v2 messaging system.
 */

void i_respond_ok (i_resource *self, i_message *original_msg)
{
  long msgid;
  msgid = i_message_send (self, MSG_OK, NULL, 0, original_msg->src, MSG_FLAG_RESP, original_msg->msgid);
}

void i_respond_failed (i_resource *self, i_message *original_msg, int errcode)
{
  long msgid;
  msgid = i_message_send (self, MSG_FAIL, &errcode, sizeof(int), original_msg->src, MSG_FLAG_RESP|MSG_FLAG_ERROR, original_msg->msgid);
}

void i_respond_denied (i_resource *self, i_message *original_msg)
{
  long msgid;
  msgid = i_message_send (self, MSG_AUTH_DENIED, NULL, 0, original_msg->src, MSG_FLAG_RESP|MSG_FLAG_DENIED, original_msg->msgid);
}

void i_respond_nodata (i_resource *self, i_message *original_msg)
{
  long msgid;
  msgid = i_message_send (self, MSG_NO_DATA, NULL, 0, original_msg->src, MSG_FLAG_RESP|MSG_FLAG_ERROR, original_msg->msgid);
}

void i_respond_route_failed (i_resource *self, i_message *original_msg)
{
  long msgid;
  msgid = i_message_send (self, MSG_ROUTE_FAILED, NULL, 0, original_msg->src, MSG_FLAG_RESP|MSG_FLAG_ERROR, original_msg->msgid);
}
