#define CALLBACK_FUNC int (*callback_func) (i_resource *self, i_message *msg, void *passdata)

typedef struct i_message_callback_s
{
  long reqid;
  
  int (*callback_func) (i_resource *self, i_message *msg, void *data);
  void *data;

  i_message *msg;
  struct timeval timeout;
  i_timer *timer;
  
} i_message_callback;

void i_message_callback_free (void *msg_cb_ptr);
i_message_callback* i_message_callback_create ();
