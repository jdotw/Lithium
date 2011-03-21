typedef struct v_intenv_item_s
{
  struct i_object_s *obj;
  
  struct i_metric_s *temp;
  struct i_metric_s *units;

} v_intenv_item;

/* intenv.c */
struct i_container_s* v_intenv_cnt ();
int v_intenv_enable (i_resource *self);
int v_intenv_disable (i_resource *self);

/* intenv_item.c */
v_intenv_item* v_intenv_item_create ();
void v_intenv_item_free (void *itemptr);

/* intenv_objfact.c */
int v_intenv_objfact_fab ();
int v_intenv_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_intenv_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

