typedef struct v_xsanvoldetail_item_s
{
  int index;
  struct i_container_s *cnt;
  struct i_object_s *obj;

  /* Config */
  struct i_metric_s *allocation_strategy;
  struct i_metric_s *buffer_cache_size;
  struct i_metric_s *debug;
  struct i_metric_s *enforce_acls;
  struct i_metric_s *file_locks;
  struct i_metric_s *force_stripe_alignment;
  struct i_metric_s *fs_block_size;
  struct i_metric_s *global_super_user;
  struct i_metric_s *inode_cache_size;
  struct i_metric_s *inode_expand_inc;
  struct i_metric_s *inode_expand_max;
  struct i_metric_s *inode_expand_min;
  struct i_metric_s *journal_size;
  struct i_metric_s *max_connections;
  struct i_metric_s *max_logsize;
  struct i_metric_s *quotas;
  struct i_metric_s *threadpool_size;
  struct i_metric_s *unix_fab_window;
  struct i_metric_s *windows_security;

} v_xsanvoldetail_item;

v_xsanvoldetail_item* v_xsanvoldetail_get (char *name_str);
int v_xsanvoldetail_enable (i_resource *self);
v_xsanvoldetail_item* v_xsanvoldetail_create (i_resource *self, char *vol_name);
int v_xsanvoldetail_disable (i_resource *self);

v_xsanvoldetail_item* v_xsanvoldetail_item_create ();
void v_xsanvoldetail_item_free (void *itemptr);

