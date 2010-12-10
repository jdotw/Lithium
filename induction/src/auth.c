#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "auth.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "data.h"

/** \addtogroup backend Back-End APIs
 * @{
 *
 * @}
 */

/** \addtogroup auth Authentication
 * @ingroup backend
 * @{
 */

void i_authentication_free (void *authptr)
{
  i_authentication *auth = authptr;

  if (!auth) return;

  if (auth->username) free (auth->username);
  if (auth->password) free (auth->password);
  if (auth->customer_id) free (auth->customer_id);

  free (auth);
}

i_authentication* i_authentication_create ()
{
  i_authentication *auth;

  auth = (i_authentication *) malloc (sizeof(i_authentication));
  if (!auth)
  {
    i_printf (1, "i_authentication_create failed to malloc auth");
    return NULL;
  }
  memset (auth, 0, sizeof(i_authentication));

  return auth;
}

i_authentication* i_authentication_duplicate (i_authentication *auth)
{
  i_authentication *dup;

  if (!auth) return NULL;

  dup = i_authentication_create ();
  if (!dup) 
  {
    i_printf (1, "i_authentication_duplicate failed to create dup");
    return NULL;
  }

  if (auth->username) dup->username = strdup (auth->username);
  if (auth->password) dup->password = strdup (auth->password);
  if (auth->customer_id) dup->customer_id = strdup (auth->customer_id);
  dup->level = auth->level;
  
  return dup;
}

void i_authentication_verify_free_passdata (void *passptr)
{
  i_authentication_verify_passdata *pass = passptr;

  if (!pass) return;

  if (pass->auth) i_authentication_free (pass->auth);

  free (pass);
}

int i_authentication_verify_callback (i_resource *self, i_socket *sock, i_message *msg, void *passdata)
{
  /* Called when a response to a MSG_AUTH_VERIFY request is recieved
   *
   * The message passed to the function will contain the 'level' int
   *
   */

  i_authentication_verify_passdata *pass = passdata;

  if (!self || !pass)
  { i_printf (1, "i_authentication_verify_callback either self or pass were NULL (%p and %p)", self, pass); return -1; }
  if (!pass->callback_func)
  { i_printf (1, "i_authentication_verify_callback called with NULL pass->callback_func"); return -1; }
  
  if (!msg)
  {
    /* Timeout occurred */
    i_printf (1, "i_authentication_verify_callback timeout occurred waiting for authentication response");
    pass->callback_func (self, pass->auth, AUTH_RESULT_TIMEOUT, pass->passdata);   /* Call the callback with the TIMEOUT result */
    i_authentication_verify_free_passdata (pass);
    return 0;
  }

  /* Got a response message, interpret it */

  if (msg->flags & MSG_FLAG_ERROR)
  {
    i_printf (1, "i_authentication_verify_callback a failure was encountered during authentication");
    pass->callback_func (self, pass->auth, AUTH_RESULT_ERROR, pass->passdata);
    i_authentication_verify_free_passdata (pass);
    return 0;
  }

  if (msg->flags & MSG_FLAG_DENIED)
  {
    i_printf (1, "i_authentication_verify_callback authentication was denied");
    pass->callback_func (self, pass->auth, AUTH_RESULT_DENIED, pass->passdata);
    i_authentication_verify_free_passdata (pass);
    return 0;
  }

  /* No error, no denied, continue with message processing */

  if (!msg->data || msg->datasize != sizeof(int))
  {
    i_printf (1, "i_authentication_verify_callback msg->data or datasize incorrect (data=%p size=%i)", msg->data, msg->datasize);
    pass->callback_func (self, pass->auth, AUTH_RESULT_ERROR, pass->passdata);
    i_authentication_verify_free_passdata (pass);
    return 0;
  }

  memcpy (&pass->auth->level, msg->data, sizeof(int));  /* Update the auth level with the msg data */
  pass->callback_func (self, pass->auth, AUTH_RESULT_OK, pass->passdata);  /* Call the callback */
  i_authentication_verify_free_passdata (pass);

  return 0;
}

int i_authentication_verify (i_resource *self, i_resource_address *authres, i_authentication *creds, AUTH_CALLBACK, void *passdata)
{
  /* Takes the given credentials and sends the appropriate
   * verification request to the authres. 
   *
   * If the callback func is provided a callback will be installed. 
   *
   * Note, the value returned here does _NOT_ indicate the 
   * success of the verification. 
   *
   * Returns 0 on Request proceeding
   * Returns -1 on failed to send request
   */

  int datasize;
  char *data;
  long msgid;
  i_msgproc_callback *cb;
  i_authentication_verify_passdata *pass;
  
  if (!creds || !creds->customer_id || !creds->username)
  {
    i_printf (1, "i_authentication_verify failed due to insufficient arguments (%p, %p, %p)", creds, creds->customer_id, creds->username);
    return -1;
  }

  /* Send the MSG_AUTH_VERIFY request */

  data = i_authentication_struct_to_data (creds, &datasize);
  if (!data)
  {
    i_printf (1, "i_authentication_verify failed to convert auth creds to data");
    return -1;
  }

  msgid = i_message_send (self, MSG_AUTH_VERIFY, data, datasize, authres, MSG_FLAG_REQ, 0);
  free (data);
  if (msgid == -1)
  {
    i_printf (1, "i_authentication_verify failed to send MSG_AUTH_VERIFY message");
    return -1;
  }

  /* Prepare callback passdata */
  
  pass = (i_authentication_verify_passdata *) malloc (sizeof(i_authentication_verify_passdata));
  if (!pass)
  {
    i_printf (1, "i_authentication_verify failed to malloc i_authentication_veryify_passdata");
    return -1;
  }
  memset (pass, 0, sizeof(i_authentication_verify_passdata));

  pass->auth = i_authentication_duplicate (creds);
  if (!pass->auth)
  {
    i_printf (1, "i_authentication_verify failed to duplicate credentials into pass struct");
    i_authentication_verify_free_passdata (pass);
    return -1;
  }
  pass->callback_func = callback_func;
  pass->passdata = passdata;

  /* Install the callback */
    
  cb = i_msgproc_callback_add (self, self->core_socket, msgid, MSG_TIMEOUT_SEC, MSG_TIMEOUT_USEC, i_authentication_verify_callback, pass);
  if (!cb)
  {
    i_printf (1, "i_authentication_verify sent verify message but failed to add callback");
    i_authentication_verify_free_passdata (pass);
    return -1;
  }

  /* Finished for now */

  return 0;
}

char* i_authentication_struct_to_data (i_authentication *auth, int *datasizeptr)
{
  /* Data format is as follows:
   *
   * int username_size;
   * char *username;
   * int password_size;
   * char *password;
   * int customer_id_size;
   * char *customer_id;
   * int level;
   *
   */

  int datasize;
  char *data;
  char *dataptr;

  if (!datasizeptr) return NULL;
  memset (datasizeptr, 0, sizeof(int));
  if (!auth) return NULL;
  
  datasize = 4*sizeof(int);
  if (auth->username) datasize += strlen(auth->username)+1;
  if (auth->password) datasize += strlen(auth->password)+1;
  if (auth->customer_id) datasize += strlen(auth->customer_id)+1;

  data = (char *) malloc (datasize);
  if (!data)
  {
    i_printf (1, "i_authentication_struct_to_data failed to malloc data");
    return NULL;
  }
  dataptr = data;

  dataptr = i_data_add_string (data, dataptr, datasize, auth->username);
  if (!dataptr)
  { i_printf (1, "i_authentication_struct_to_data failed to add username to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, auth->password);
  if (!dataptr)
  { i_printf (1, "i_authentication_struct_to_data failed to add password to data"); free (data); return NULL; }

  dataptr = i_data_add_string (data, dataptr, datasize, auth->customer_id);
  if (!dataptr)
  { i_printf (1, "i_authentication_struct_to_data failed to add customer_id to data"); free (data); return NULL; }

  dataptr = i_data_add_int (data, dataptr, datasize, &auth->level);
  if (!dataptr)
  { i_printf (1, "i_authentication_struct_to_data failed to add level to data"); free (data); return NULL; }

  memcpy (datasizeptr, &datasize, sizeof(int));
  
  return data;
}

i_authentication* i_authentication_data_to_struct (char *data, unsigned int datasize)
{
  int offset;
  char *dataptr = data;
  i_authentication *auth;

  if (!data || datasize < 1) return NULL;

  auth = i_authentication_create ();
  if (!auth)
  { 
    i_printf (1, "i_authentication_data_to_struct failed to create auth");
    return NULL;
  }

  auth->username = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_authentication_data_to_struct failed to get username from data"); i_authentication_free (auth); return NULL; }
  dataptr += offset;

  auth->password = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1)
  { i_printf (1, "i_authentication_data_to_struct failed to get password from data"); i_authentication_free (auth); return NULL; }
  dataptr += offset;

  auth->customer_id = i_data_get_string (data, dataptr, datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_authentication_data_to_struct failed to get customer_id from data"); i_authentication_free (auth); return NULL; }
  dataptr += offset;

  auth->level = i_data_get_int (data, dataptr, datasize, &offset);
  if (offset < 1) 
  { i_printf (1, "i_authentication_data_to_struct failed to get level from data"); i_authentication_free (auth); return NULL; }
  dataptr += offset;

  return auth;
}

int i_authentication_compare (i_authentication *control, i_authentication *auth)
{
  /* Verifies the authenticity of control and auth 
   *
   * Return 0 on match, -1 on fail/no-match
   */

  if (control->customer_id && auth->customer_id)
  {
    if ((strcmp(control->customer_id, auth->customer_id)) != 0)
    {
      /* Customer id doesnt match */
      return -1;
    }
  }
  else if (control->customer_id && !auth->customer_id)
  {
    /* Customer ID required but none supplied */
    return -1;
  }

  if (control->username && auth->username)
  {
    if ((strcmp(control->username, auth->username)) != 0)
    {
      /* Username does not match */
      return -1;
    }
  }
  else if (control->username && !auth->username)
  {
    /* Username required but none supplied */
    return -1;
  }

  if (control->password && auth->password)
  { 
    if ((strcmp(control->password, auth->password)) != 0)
    {
      /* Password does not match */
      return -1;
    }
  }
  else if (control->password && !auth->password)
  {
    /* Password required but none supplied */
    return -1;
  }

  /* All matched / All OK */

  return 0;
}

/* @} */
