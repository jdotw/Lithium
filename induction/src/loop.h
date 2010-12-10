/* loop.c */

int i_loop_init ();
int i_loop_select_timeout_set (time_t sec, time_t usec);
int i_loop_select_timeout_get (struct timeval *timeout);
int i_loop (struct i_resource_s *self);

/* loop_fdset.c */

int i_loop_fdset_init ();
struct i_callback_s* i_loop_fdset_preprocessor_add (i_resource *self, int (*callback_func) (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data), void *data);
int i_loop_fdset_preprocessor_remove (i_resource *self, struct i_callback_s *cb);
struct i_callback_s* i_loop_fdset_postprocessor_add (i_resource *self, int (*callback_func) (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset, void *data), void *data);
int i_loop_fdset_postprocessor_remove (i_resource *self, struct i_callback_s *cb);
int i_loop_fdset_preprocessor (i_resource *self, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset);
int i_loop_fdset_postprocessor (i_resource *self, int select_num, fd_set *read_fdset, fd_set *write_fdset, fd_set *except_fdset);


