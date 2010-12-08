/* vendor.c */

int l_vendor_init (i_resource *self);
int l_vendor_init_failed (i_resource *self, char *error);
int l_vendor_init_retry_callback (i_resource *self, struct i_timer_s *timer, void *data);

/* vendor_load.c */

int l_vendor_load (i_resource *self, char *vendor_id);
int l_vendor_unload (i_resource *self, i_vendor *vendor);

