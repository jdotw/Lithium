typedef struct i_callback_s
{
  int id_int;
  long id_long;
  int (*func) ();
  void *data;
  void *passdata;
} i_callback;

void i_callback_free (void *callbackptr);
i_callback* i_callback_create ();

