#include <stdlib.h>

#include <induction.h>
#include <induction/auth.h>
#include <induction/message.h>
#include <induction/socket.h>
#include <induction/respond.h>
#include <induction/list.h>
#include <induction/user.h>

#include "authentication.h"

int l_authentication_check_verification_callback (i_resource *self, i_authentication *auth, int result, void *passdata)
{
  /* This callback is called by the l_authentication_verify function */

  int flags = 0;
  long msgid;
  i_message *msg = passdata;

  if (!msg)
  {
    i_printf (1, "l_authentication_check_verification_callback called with NULL msg");
    return -1;
  }
  
  if (result == AUTH_RESULT_OK)
  {
    /* Successful authentication */

    msgid = i_message_send (self, MSG_AUTH_VERIFY, &auth->level, sizeof(int), msg->src, MSG_FLAG_RESP, msg->msgid);
    i_message_free (msg);
    if (msgid == -1)
    {
      i_printf (1, "l_authentication_check_verification_callback failed to send success response for username %s", auth->username);
      return -1;
    }

    return 0;
  }

  /* Unsuccessful authentication */

  switch (result)
  {
    case AUTH_RESULT_DENIED: flags = MSG_FLAG_DENIED;
                             break;
    default: flags = MSG_FLAG_ERROR;
             break;
  }

  msgid = i_message_send (self, MSG_AUTH_VERIFY, NULL, 0, msg->src, MSG_FLAG_RESP|flags, msg->msgid);
  i_message_free (msg);  
  if (msgid == -1)  
  {    
    i_printf (1, "l_authentication_check_verification_callback failed to send failure response for username %s", auth->username);    
    return -1;  
  }

  return 0;
}

int l_authentication_check_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* 
   * Called to process an authentication check message 
   *
   * l_authentication_very is called which will call the above
   * callback function when a response is available
   *
   */

  int result;
  i_message *dup_msg;
  i_authentication *auth;

  if (!msg || !msg->data)
  {
    i_printf (1, "l_authentication_check_handler did not receive any authentication info");
    i_respond_failed (self, msg, 0);
    return -1;
  }

  auth = i_authentication_data_to_struct (msg->data, msg->datasize);
  if (!auth)
  {
    i_printf (1, "l_authentication_check_handler failed to convert msg->data to auth data");
    i_respond_failed (self, msg, 0);
    return -1;
  }

  dup_msg = i_message_duplicate (msg);
  if (!dup_msg)
  {
    i_printf (1, "l_authentication_check_handler failed to create duplicate message");
    i_respond_failed (self, msg, 0);
    return -1;
  }

  result = l_authentication_verify (self, auth, l_authentication_check_verification_callback, dup_msg);
  if (result == -1)
  {
    i_printf (1, "l_authentication_check_handler failed to request verification for username %s", auth->username);
    i_respond_failed (self, msg, 0);
    i_authentication_free (auth);
    i_message_free (dup_msg);
    return -1;
  }
  i_authentication_free (auth); /* Duplicated in l_authentication_verify for callback use */

  return 0;
}

int l_authentication_required_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Check to see if authentication is required for this customer. If it is, 
   * return a 0 integer in the data. If auth is not required, return a 5
   * integer in the data
   */

  int response = 0;
  i_list *user_list = i_user_sql_list(self);
  if (!user_list || user_list->size == 0)
  {
    /* No users, no auth */
    response = 5;
  }

  long msgid = i_message_send (self, MSG_AUTH_REQUIRED, &response, sizeof(int), msg->src, MSG_FLAG_RESP, msg->msgid);
  if (msgid == -1)
  {
    i_printf(1, "l_authentication_required_handler failed to response to a MSG_AUTH_REQUIRED request");
  }

  return 0; // Keep handler alive
}
