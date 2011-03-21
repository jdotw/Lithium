void l_spawn_schedule_free_callbackdata (void *dataptr);
int l_spawn_scheduled_callback (i_resource *self, i_timer *timer, void *dataptr);
int l_spawn_scheduled (i_resource *self, i_hashtable *res_table, int type, int ident_int, char *ident_str, char *mod_str, char *root, i_form *config_form, int no_remove);
