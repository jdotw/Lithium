typedef struct v_raidset_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *status;
  struct i_metric_s *drivecount_orig;
  struct i_metric_s *drivecount_cur;
  struct i_metric_s *drivecount_spare;
  struct i_metric_s *size;

} v_raidset_item;

/* raidset.c */
struct i_container_s* v_raidset_cnt ();
int v_raidset_enable (i_resource *self);

/* raidset_item.c */
v_raidset_item* v_raidset_item_create ();
void v_raidset_item_free (void *itemptr);

/* raidset_objfact.c */
int v_raidset_objfact_fab ();
int v_raidset_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_raidset_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

