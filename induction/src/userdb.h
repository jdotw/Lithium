struct i_user_s* i_userdb_get (i_resource *self, char *username);
void i_userdb_clear_cache ();
struct i_list_s* i_userdb_get_all (i_resource *self);
int i_userdb_put (i_resource *self, struct i_user_s *user);
int i_userdb_del (i_resource *self, char *username);
