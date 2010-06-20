/* device.c */

#define DEVICE_MODULE "device.so"

struct i_device_s* l_device_add (i_resource *self, struct i_site_s *site, char *name_str, char *desc_str, char *ip_str, char *lom_ip_str, int snmpversion, char *snmpcomm_str, char *snmpauthpass_str, char *snmpprivpass_str, int snmpauthmethod, int snmpprivenc, char *username_str, char *password_str, char *lom_username_str, char *lom_password_str, char *vendor_str, char *profile_str, long refresh_interval, int protocol, int icmp, int lithiumsnmp, int swrun, int nagios, int lom, int xsan);
int l_device_update (i_resource *self, struct i_device_s *dev);
int l_device_remove (i_resource *self, struct i_device_s *dev);
int l_device_sqlcb ();
int l_device_loadall (i_resource *self, struct i_site_s *site);
int l_device_loadall_timercb (i_resource *self, struct i_timer_s *timer, void *passdata);
int l_device_initsql (i_resource *self);

/* device_move.c */
int l_device_move (i_resource *self, struct i_device_s *dev, struct i_site_s *dest_site);

/* device_sort.c */
int l_device_sortfunc_desc (void *curptr, void *nextptr);
int l_device_sortfunc_name (void *curptr, void *nextptr);
int l_device_sortfunc_ip (void *curptr, void *nextptr);

/* device_resource.c */
int l_device_res_queue_depth ();
int l_device_res_restart (i_resource *self, struct i_device_s *dev);
int l_device_res_restart_spawncb (i_resource *self, i_resource_address *addr, void *passdata);
int l_device_res_destroy (i_resource *self, struct i_device_s *dev);

/* device_form_list.c */
int form_device_list (i_resource *self, struct i_form_reqdata_s *reqdata);
