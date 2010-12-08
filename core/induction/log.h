/* log.c */
int i_log_enable (struct i_resource_s *self, char *log_filename);
int i_log_disable (struct i_resource_s *self);
int i_log_init (i_resource *self);
int i_log_string (int level, char *string);
