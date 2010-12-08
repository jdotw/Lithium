#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "socket.h"
#include "message.h"
#include "msgproc.h"
#include "timer.h"
#include "hashtable.h"

/* Struct Manipulation */

i_msgproc_callback* i_msgproc_callback_create ()
{
  i_msgproc_callback *cb;

  cb = (i_msgproc_callback *) malloc (sizeof(i_msgproc_callback));
  if (!cb)
  { i_printf (1, "i_msgproc_callback_create failed to malloc cb"); return NULL; }
  memset (cb, 0, sizeof(i_msgproc_callback));

  return cb;
}

void i_msgproc_callback_free (void *cbptr)
{
  i_msgproc_callback *cb = cbptr;

  if (!cb) return;
  if (cb->timeout_timer)
  { i_timer_remove (cb->timeout_timer); }

  free (cb);
}

/* Callback manipulation */

i_msgproc_callback* i_msgproc_callback_add (i_resource *self, i_socket *sock, long reqid, time_t timeout_sec, time_t timeout_usec, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *data), void *passdata)
{
  /* Adds a callback for the given reqid.
   *
   * The callback is added to the callback_table and a timer
   * is installed to allow for a 'time-out' for the callback
   */

  int num;
  i_msgproc_callback *cb;
  i_hashtable_key *key;

  if (!self || !sock || !callback_func) return NULL;
  if (!sock->msgproc || !sock->msgproc->callback_table)
  { i_printf (1, "i_msgproc_callback_add failed, sock->msgproc or sock->msgproc->callback_table not present"); return NULL; }
  
  cb = i_msgproc_callback_create ();
  if (!cb)
  { i_printf (1, "i_msgproc_callback_add failed to create cb struct"); return NULL; }

  cb->reqid = reqid;
  cb->socket = sock;
  cb->timeout.tv_sec = timeout_sec;
  cb->timeout.tv_usec = timeout_usec;
  cb->callback_func = callback_func;
  cb->passdata = passdata;

  cb->timeout_timer = i_timer_add (self, timeout_sec, timeout_usec, i_msgproc_callback_timeout_callback, cb);
  if (!cb->timeout_timer)
  { i_printf (1, "i_msgproc_callback_add failed to add timeout timer"); i_msgproc_callback_free (cb); return NULL; }

  key = i_hashtable_create_key_long (reqid, sock->msgproc->callback_table->size);
  if (!key)
  { i_printf (1, "l_msgproc_callback_add failed to create hashtable key"); i_msgproc_callback_free (cb); return NULL; }

  num = i_hashtable_put (sock->msgproc->callback_table, key, cb);
  i_hashtable_free_key (key);
  if (num != 0)
  { i_printf (1, "l_msgproc_callback_add failed to put cb in callback table"); i_msgproc_callback_free (cb); return NULL; }

  return cb;
}

int i_msgproc_callback_set_default (i_resource *self, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, i_message *msg, void *data), void *passdata)
{
  /* The default callback is a callback-of-last-resort */

  i_msgproc_callback *cb;

  if (!sock || !sock->msgproc || !sock->msgproc->callback_table || !callback_func) return -1;

  cb = i_msgproc_callback_create ();
  if (!cb)
  { i_printf (1, "i_msgproc_callback_set_default failed to create cb struct"); return -1; }

  cb->socket = sock;
  cb->callback_func = callback_func;
  cb->passdata = passdata;

  if (sock->msgproc->default_callback) 
  { i_socket_callback_free (sock->msgproc->default_callback); sock->msgproc->default_callback = NULL; }
  sock->msgproc->default_callback = cb;

  return 0;
}

int i_msgproc_callback_remove_by_reqid (i_socket *sock, long reqid)
{
  int num;
  i_hashtable_key *key;

  if (!sock || !sock->msgproc || !sock->msgproc->callback_table) return -1;
  
  key = i_hashtable_create_key_long (reqid, sock->msgproc->callback_table->size);
  if (!key)
  { i_printf (1, "i_msgproc_callback_remove_by_reqid failed to create key"); return -1; }

  num = i_hashtable_remove (sock->msgproc->callback_table, key);
  i_hashtable_free_key (key);

  return num;
}

int i_msgproc_callback_remove (i_msgproc_callback *cb)
{ return i_msgproc_callback_remove_by_reqid (cb->socket, cb->reqid); }

/* Timeout Callback */

int i_msgproc_callback_timeout_callback (i_resource *self, i_timer *timer, void *passdata)
{
  /* This callback is called when a message callback's timeout 
   * timer is fired.
   *
   * The callback is called with a NULL msg
   *
   * A callback must NOT free the timer . Hence -1 is returned
   * to ensure the timers removal
   */

  i_hashtable_key *key;
  i_msgproc_callback *cb = passdata;

  if (!self || !passdata) return -1;

  cb->timeout_timer = NULL;

  key = i_hashtable_create_key_long (cb->reqid, cb->socket->msgproc->callback_table->size);
  if (!key)
  { i_printf (1, "i_msgproc_callback_timeout_callback failed to create hash key"); return -1; }
  
  /* Run the callback with a NULL message to indicate
   * a timeout has occurred 
   */
  
  if (cb->callback_func) cb->callback_func (self, cb->socket, NULL, cb->passdata);

  /* Remove the callback from the table */

  i_hashtable_remove (cb->socket->msgproc->callback_table, key);
  i_hashtable_free_key (key);    

  return -1;
}


