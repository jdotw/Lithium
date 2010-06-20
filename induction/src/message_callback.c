#include <stdlib.h>

#include "induction.h"

void i_message_callback_free (void *msg_cb_ptr)
{
  i_message_callback *msg_cb = msg_cb_ptr;

  if (!msg_cb) return;

  free (msg_cb);
}

i_message_callback* i_message_callback_create ()
{
  i_message_callback *msg_cb;

  msg_cb = (i_message_callback *) malloc (sizeof(i_message_callback));
  if (!msg_cb)
  { i_printf (1, "i_message_callback_create failed to malloc msg_cb"); return NULL; }
  memset (msg_cb, 0, sizeof(i_message_callback));

  return msg_cb;
}
