#include <stdlib.h>
#include <string.h>

#include "induction.h"
#include "callback.h"

void i_callback_free (void *callbackptr)
{
  i_callback *callback = callbackptr;

  if (!callback) return;

  free (callback);
}

i_callback* i_callback_create ()
{
  i_callback *cb;

  cb = (i_callback *) malloc (sizeof(i_callback));
  if (!cb) 
  { i_printf (1, "i_callback_create failed to malloc cb"); return NULL; }
  memset (cb, 0, sizeof(i_callback));

  return cb;
}


