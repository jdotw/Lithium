/* refresh.c */
int l_refresh_init (i_resource *self, struct i_device_s *dev);
int l_refresh_device_refcb (i_resource *self, struct i_entity_s *ent, void *passdata);
int l_refresh_backtoback_timercb (i_resource *self, i_timer *timer, void *passdata);
