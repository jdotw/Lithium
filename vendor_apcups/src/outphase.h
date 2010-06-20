typedef struct v_outphase_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *voltage;
  struct i_metric_s *current;
  struct i_metric_s *load;
  struct i_metric_s *load_pc;
  struct i_metric_s *power;
  struct i_metric_s *power_pc;

} v_outphase_item;

/* outphase.c */
struct i_container_s* v_outphase_cnt ();
int v_outphase_enable (i_resource *self);
int v_outphase_disable (i_resource *self);

/* outphase_item.c */
v_outphase_item* v_outphase_item_create ();
void v_outphase_item_free (void *itemptr);

/* outphase_objfact.c */
int v_outphase_objfact_fab ();
int v_outphase_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_outphase_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);
