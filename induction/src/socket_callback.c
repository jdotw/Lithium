#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "socket.h"
#include "callback.h"
#include "list.h"
#include "loop.h"

static i_list *static_callback_list = NULL;
static i_callback *static_fdset_preprocessor = NULL;
static i_callback *static_fdset_postprocessor = NULL;
extern i_resource *global_self;

/* Init */

int i_socket_callback_init (i_resource *self)
{
  i_socket_callback_fdset_invalidate ();
  
  if (static_callback_list)
  { i_list_free (static_callback_list); static_callback_list = NULL; }

  static_fdset_preprocessor = i_loop_fdset_preprocessor_add (self, i_socket_callback_fdset_preprocessor, NULL);
  if (!static_fdset_preprocessor)
  { i_printf (1, "i_socket_callback_init failed to add fdset preprocessor"); return -1; }

  static_fdset_postprocessor = i_loop_fdset_postprocessor_add (self, i_socket_callback_fdset_postprocessor, NULL);
  if (!static_fdset_postprocessor)
  { i_printf (1, "i_socket_callback_init failed to add fdset postprocessor"); return -1; }
  
  return 0;
}

/* Struct Manipulation */

i_socket_callback* i_socket_callback_create ()
{
  i_socket_callback *cb;

  cb = (i_socket_callback *) malloc (sizeof(i_socket_callback));
  if (!cb)
  { i_printf (1, "i_socket_callback_create failed to malloc cb"); return NULL; }
  memset (cb, 0, sizeof(i_socket_callback));

  return cb;
}

void i_socket_callback_free (void *cb_ptr)
{
  i_socket_callback *cb = cb_ptr;

  if (!cb) return;

  free (cb);
}

/* Misc List Manipulation */

i_list* i_socket_callback_list ()
{ return static_callback_list; }

int i_socket_callback_list_free ()
{
  if (static_callback_list)
  { i_list_free (static_callback_list); static_callback_list = NULL; }
  return 0;
}

/* Add */

i_socket_callback* i_socket_callback_add (i_resource *resource, int type, i_socket *sock, int (*callback_func) (i_resource *self, i_socket *sock, void *passdata), void *passdata)
{
  /* Adds a callback */
  int num;
  i_socket_callback *callback;

  if (!resource || !sock) return NULL;

  callback = i_socket_callback_create ();
  if (!callback)
  { i_printf (1, "i_socket_callback_add failed to create callback struct"); return NULL; }
  callback->resource = resource;
  callback->type = type;
  callback->socket = sock;
  callback->callback_func = callback_func;
  callback->passdata = passdata;

  if (!static_callback_list)
  {
    static_callback_list = i_list_create ();
    if (!static_callback_list)
    { i_printf (1, "i_socket_callback_add failed to create static_callback_list"); i_socket_callback_free (callback); return NULL; }
    i_list_set_destructor (static_callback_list, i_socket_callback_free);
  }
  
  num = i_list_enqueue (static_callback_list, callback);
  if (num != 0)
  { i_printf (1, "i_socket_callback_add failed to enqueue callback"); i_socket_callback_free (callback); return NULL; }
  i_socket_callback_fdset_invalidate ();

  return callback;
}

/* Remove */

int i_socket_callback_remove (i_socket_callback *callback)
{
  if (!callback) return -1;

  callback->resource = NULL;
  callback->type = 0;
  callback->callback_func = NULL;
  callback->passdata = NULL;
  callback->socket = NULL;
  i_socket_callback_fdset_invalidate ();

  return 0;
}

int i_socket_callback_remove_by_socket (i_socket *sock)
{
  int num_removed = 0;
  i_socket_callback *callback;
  void *p;

  if (static_callback_list)
  {
    p = static_callback_list->p;
    for (i_list_move_head(static_callback_list); (callback=i_list_restore(static_callback_list))!=NULL; i_list_move_next(static_callback_list))
    { if (callback->socket == sock) i_socket_callback_remove (callback); num_removed++; }
    static_callback_list->p = p;
  }

  if (num_removed < 1) return -1;
  
  i_socket_callback_fdset_invalidate ();
  
  return 0;
}
