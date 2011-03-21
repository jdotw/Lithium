typedef struct v_volume_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *status;
  struct i_metric_s *raidset;
  struct i_metric_s *level;
  struct i_metric_s *stripesize;
  struct i_metric_s *cachepolicy;
  struct i_metric_s *size;
  struct i_metric_s *readmaxtime;

} v_volume_item;

/* volume.c */
struct i_container_s* v_volume_cnt ();
int v_volume_enable (i_resource *self);

/* volume_item.c */
v_volume_item* v_volume_item_create ();
void v_volume_item_free (void *itemptr);

/* volume_objfact.c */
int v_volume_objfact_fab ();
int v_volume_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_volume_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

