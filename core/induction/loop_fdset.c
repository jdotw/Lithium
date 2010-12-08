#include <stdlib.h>

#include "induction.h"
#include "loop.h"
#include "list.h"
#include "callback.h"

static i_list *static_fdset_preprocessors = NULL;
static i_list *static_fdset_postprocessors = NULL;

/* Initialisation */

int i_loop_fdset_init ()
{
  /* Preprocesor */
  
  if (static_fdset_preprocessors)
  { i_list_free (static_fdset_preprocessors); }
  static_fdset_preprocessors = i_list_create ();
  i_list_set_destructor (static_fdset_preprocessors, i_callback_free);

  /* Postprocessor */

  if (static_fdset_postprocessors)
  { i_list_free (static_fdset_postprocessors); }
  static_fdset_postprocessors = i_list_create ();
  i_list_set_destructor (static_fdset_postprocessors, i_callback_free);

  /* Finished */

  return 0;
}

/* Preprocessor Callback manipulation */

i_callback* i_loop_fdset_preprocessor_add (i_resource *self, int (*callback_func) (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data), void *data)
{
  /* Add a pre-processor callback */
  i_callback *cb;

  cb = i_callback_create ();
  if (!cb)
  { i_printf (1, "i_loop_fdset_preprocessor_add failed to create cb"); return NULL; }

  cb->func = callback_func;
  cb->data = data;
  i_list_enqueue (static_fdset_preprocessors, cb);

  return cb;
}

int i_loop_fdset_preprocessor_remove (i_resource *self, i_callback *cb)
{
  int num;

  num = i_list_search (static_fdset_preprocessors, cb);
  if (num != 0) return -1;

  i_list_delete (static_fdset_preprocessors);

  return 0;
}

/* Postprocessor Callback manipulation */

i_callback* i_loop_fdset_postprocessor_add (i_resource *self, int (*callback_func) (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data), void *data)
{
  /* Add a postprocessor callback */
  i_callback *cb;

  cb = i_callback_create ();
  if (!cb)
  { i_printf (1, "i_loop_fdset_postprocessor_add failed to create cb"); return NULL; }

  cb->func = callback_func;
  cb->data = data;
  i_list_enqueue (static_fdset_postprocessors, cb);

  return cb;
}

int i_loop_fdset_postprocessor_remove (i_resource *self, i_callback *cb)
{
  int num;

  num = i_list_search (static_fdset_postprocessors, cb);
  if (num != 0) return -1;

  i_list_delete (static_fdset_postprocessors);

  return 0;
}


/* The actual preprocessor and postprocessor functions */

int i_loop_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset)
{
  int highestsockfd = 0;
  i_callback *callback;
  
  for (i_list_move_head(static_fdset_preprocessors); (callback=i_list_restore(static_fdset_preprocessors))!=NULL; i_list_move_next(static_fdset_preprocessors))
  {
    if (callback->func)
    {
      /* 
       * Run the callback, it should return -1 on error or 
       * the highest sockfd that was added to the fdset 
       */

      int num;

      num = callback->func (self, read_fdset, write_fdset, except_fdset, callback->data);
      if (num < 0)
      {
        /* Returned <0, remove the callback */
        i_list_delete (static_fdset_preprocessors); 
      }
      else
      {
        /* Returned 0 or more, check highestsockfd */
        if (num > highestsockfd) highestsockfd = num; 
      }

    }
    else
    {
      /* No callback func, remove the callback */
      i_list_delete (static_fdset_preprocessors);
    }
  }

  return highestsockfd;
}

int i_loop_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset)
{
  i_callback *callback;
  
  for (i_list_move_head(static_fdset_postprocessors); (callback=i_list_restore(static_fdset_postprocessors))!=NULL; i_list_move_next(static_fdset_postprocessors))
  {
    if (callback->func)
    {
      /* 
       * Run the callback, it should return -1 on error or 
       * the highest sockfd that was added to the fdset 
       */

      int num;

      num = callback->func (self, select_num, read_fdset, write_fdset, except_fdset, callback->data);
      if (num < 0)
      {
        /* Returned <0, remove the callback */
        i_list_delete (static_fdset_postprocessors); 
      }
    }
    else
    {
      /* No callback func, remove the callback */
      i_list_delete (static_fdset_postprocessors);
    }
  }

  return 0;
}
