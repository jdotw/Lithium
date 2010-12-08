#include <stdlib.h>
#include <string.h>

#include <induction.h>
#include <induction/auth.h>
#include <induction/files.h>
#include <induction/configfile.h>
#include <induction/user.h>
#include <induction/userdb.h>

#include "authentication.h"

int l_authentication_verify (i_resource *self, i_authentication *passauth, AUTH_CALLBACK, void *passdata)
{
  /*
   * The callback function will either be called immediately 
   * if the user is verified locally, or it will be added
   * as a callback to the authentication referral
   * 
   * NOTE : THE RETURN VALUE DOES NOT RELATE TO THE AUTHENTICATION SUCCESS
   */

  int num;
  i_user *user;
  i_authentication *auth;

  if (!self || !passauth || !passauth->username) return -1;

  /* Create our local duplicate of auth for callback use */
  auth = i_authentication_duplicate (passauth);
  if (!auth)
  {
    i_printf (1, "l_authentication_verify failed to create duplicate of passauth");
    return -1;
  }
  
  /* Check for external auth */
  char *str = i_configfile_get (self, NODECONF_FILE, "authentication", "external", 0);
  if (str && strcmp(str, "1") == 0)
  {
    /* External auth enabled */
    auth->level = AUTH_LEVEL_MASTER;
    num = callback_func (self, auth, AUTH_RESULT_OK, passdata);
    i_authentication_free (auth);
    return 0;
  }
  if (str) free (str);

  /* Get the user record from the DB */
  user = i_userdb_get (self, auth->username); 
  if (!user)
  {
    /* Username not found in the local user database.
     * At this point we refer the authentication request to
     * the NOC resource to see if the authentication credentials
     * can be verified by it.
     */

    int num;

    num = i_authentication_verify (self, self->owner, auth, callback_func, passdata);
    i_authentication_free (auth); /* Duped by i_authentication_verify */
    if (num != 0)
    {
      i_printf (1, "l_authentication_verify failed to refer authentication to NOC");
      return -1;
    }

    return 1;   /* Referred */
  }

  /* The username in the auth credentials was found in the use
   * database. Authentication is attempted on these credentials
   * 
   * Compare the user authentication data to the supplied auth data
   */

  num = i_authentication_compare (user->auth, auth);
  if (num == 0)
  {
    i_printf (2, "l_authentication_verify successfully authenticated username %s", auth->username);
  
    auth->level = user->auth->level;
    num = callback_func (self, auth, AUTH_RESULT_OK, passdata);
    i_authentication_free (auth); /* our local copy */
    i_user_free (user);
    return 0;
  }
  
  i_printf (2, "l_authentication_verify password incorrect for %s", auth->username);
  auth->level = 0;
  num = callback_func (self, auth, AUTH_RESULT_DENIED, passdata);
  i_authentication_free (auth); /* our local copy */
  i_user_free (user);

  return 0; 
}

