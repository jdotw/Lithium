typedef struct v_setup_item_s
{
  struct i_object_s *obj;

  struct i_metric_s *stripesize;
  struct i_metric_s *writebuffer_enabled;
  struct i_metric_s *memorysize;
  struct i_metric_s *accessmode;
  struct i_metric_s *prefetchsize;
  struct i_metric_s *autorebuild_enabled;
  struct i_metric_s *rebuildrate;
  struct i_metric_s *checkdata_enabled;
  struct i_metric_s *lunmask_enabled;
  struct i_metric_s *jbodmask_enabled;

} v_setup_item;

struct i_container_s* v_setup_cnt ();
v_setup_item* v_setup_static_top_item ();
v_setup_item* v_setup_static_bottom_item ();
int v_setup_enable (i_resource *self);
int v_setup_disable (i_resource *self);

v_setup_item* v_setup_item_create ();
void v_setup_item_free (void *itemptr);

