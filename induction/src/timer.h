typedef struct i_timer_s
{
  i_resource *resource;
  struct timeval last_run;
  struct timeval timeout;
  int (*callback_func) (i_resource *self, struct i_timer_s *timer, void *data);
  void *data;
  int uses;
  long tag;
} i_timer;

/* timer.c */

int i_timer_init ();
i_timer* i_timer_create ();
void i_timer_free (void *timer_ptr);
struct i_list_s* i_timer_same_interval_list (time_t sec);
int i_timer_process (i_resource *self);
int i_timer_select_timeout (i_resource *self, struct timeval *select_timeout);
i_timer* i_timer_add (i_resource *self, time_t sec, time_t usec, int (*callback_func) (i_resource *self, i_timer *timer, void *passdata), void *data);
int i_timer_remove (i_timer *timer);
int i_timer_set_timeout (i_timer *timer, time_t sec, time_t usec);
int i_timer_reset (i_timer *timer);
int i_timer_listsort_nextrun (void *curptr, void *nextptr);
int i_timer_listsort_nextrun_list (void *curptr, void *nextptr);
