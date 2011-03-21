typedef struct v_inphase_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *voltage;

} v_inphase_item;

/* inphase.c */
struct i_container_s* v_inphase_cnt ();
int v_inphase_enable (i_resource *self);
int v_inphase_disable (i_resource *self);

/* inphase_item.c */
v_inphase_item* v_inphase_item_create ();
void v_inphase_item_free (void *itemptr);

/* inphase_objfact.c */
int v_inphase_objfact_fab ();
int v_inphase_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_inphase_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);
