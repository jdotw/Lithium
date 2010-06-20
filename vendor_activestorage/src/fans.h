typedef struct v_fans_item_s
{
  struct i_object_s *obj;
  unsigned long index;
  
  struct i_metric_s *status;
  struct i_metric_s *speed;

} v_fans_item;

/* fans.c */
struct i_container_s* v_fans_cnt ();
int v_fans_enable (i_resource *self);

/* fans_item.c */
v_fans_item* v_fans_item_create ();
void v_fans_item_free (void *itemptr);

/* fans_objfact.c */
int v_fans_objfact_fab ();
int v_fans_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_fans_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

