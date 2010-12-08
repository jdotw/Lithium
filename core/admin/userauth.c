#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/form.h>
#include <induction/list.h>
#include <induction/auth.h>
#include <induction/timer.h>
#include <induction/site.h>
#include <induction/device.h>
#include <induction/path.h>
#include <induction/hierarchy.h>
#include <induction/socket.h>
#include <induction/message.h>
#include <induction/user.h>
#include <induction/userdb.h>
#include <induction/configfile.h>
#include <induction/files.h>
#include <induction/respond.h>

#include "userauth.h"

int l_userauth_handler (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* 
   * Called to process an authentication check message 
   *
   * Checks the local user DB first, and the config file (master user)
   * as a last resort.
   *
   */

  int num;
  i_user *user;
  i_authentication *userauth = NULL;
  i_authentication *ctrlauth = NULL;

  /* Check msg and data */
  if (!msg || !msg->data)
  {
    i_printf (1, "l_userauth_handler did not receive any authentication info");
    i_respond_failed (self, msg, 0);
    return -1;
  }

  /* Convert supplied auth credentials to struct */
  userauth = i_authentication_data_to_struct (msg->data, msg->datasize);
  if (!userauth)
  {
    i_printf (1, "l_userauth_handler failed to convert msg->data to auth data");
    i_respond_failed (self, msg, 0);
    return -1;
  }

  /* Attempt to retrieve user from db */
  user = i_userdb_get (self, userauth->username);
  if (!user)
  {
    /* Username not found in NOC user database. 
     * Attempt to get the master login/password 
     * from the node config file
     */

    ctrlauth = i_authentication_create ();
    if (!ctrlauth)
    {
      i_printf (1, "l_userauth_handler failed to create auth ");
      i_respond_failed (self, msg, 0);
      i_authentication_free (userauth);
      i_authentication_free (ctrlauth);
      return -1;
    }

    ctrlauth->username = i_configfile_get (self, NODECONF_FILE, "master_user", "username", 0);
    if (!ctrlauth->username) 
    {
      i_printf (1, "l_userauth_handler failed to find master_user username");
      i_respond_denied (self, msg);   /* Denied because there is no master user and the username isnt in the local userdb */
      i_authentication_free (userauth);
      i_authentication_free (ctrlauth);
      return 0;
    }

    ctrlauth->password = i_configfile_get (self, NODECONF_FILE, "master_user", "password", 0);
    if (!ctrlauth->password) 
    {
      i_printf (1, "l_userauth_handler failed to find master_user password");
      i_respond_denied (self, msg);   /* Denied because there is no master user and the username isnt in the local userdb */
      i_authentication_free (userauth);
      i_authentication_free (ctrlauth);
      return 0;
    } 
     
    ctrlauth->level = AUTH_LEVEL_MASTER;
    ctrlauth->customer_id = strdup (userauth->customer_id);
  }
  else
  {
    /* User was found, duplicate the auth and free the user */
    ctrlauth = i_authentication_duplicate (user->auth);
    i_user_free (user);
  }
    
  /* The username was found in either the user database or
   * in the config file
   * 
   * Compare the user authentication data to the supplied auth data
   */

  num = i_authentication_compare (ctrlauth, userauth);
  if (num == 0)
  {
    long msgid; 

    i_printf (2, "l_userauth_handler successfully authenticated username %s", userauth->username);   
    msgid = i_message_send (self, MSG_AUTH_VERIFY, &ctrlauth->level, sizeof(int), msg->src, MSG_FLAG_RESP, msg->msgid);
    if (msgid == -1) i_printf (1, "l_userauth_handler failed to send success message");
  }
  else
  {
    i_printf (2, "l_authentication_verify password incorrect for %s", userauth->username);
    i_respond_denied (self, msg);
  }

  if (userauth) i_authentication_free (userauth);
  if (ctrlauth) i_authentication_free (ctrlauth);
  
  return 0;
}

