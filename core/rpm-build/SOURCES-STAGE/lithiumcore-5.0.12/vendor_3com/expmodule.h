typedef struct v_expmodule_item_s
{
  struct i_object_s *obj;
  unsigned long index;

  
  struct i_metric_s *ent_type;
  struct i_metric_s *hw_ver;
  struct i_metric_s *sw_ver;
  struct i_metric_s *state;
  struct i_metric_s *fault;

} v_expmodule_item;

/* expmodule.c */
struct i_container_s* v_expmodule_cnt ();
int v_expmodule_enable (i_resource *self);
int v_expmodule_disable (i_resource *self);

/* expmodule_item.c */
v_expmodule_item* v_expmodule_item_create ();
void v_expmodule_item_free (void *itemptr);

/* expmodule_objfact.c */
int v_expmodule_objfact_fab ();
int v_expmodule_objfact_ctrl (i_resource *self, struct i_container_s *cnt, int result, void *passdata);
int v_expmodule_objfact_clean (i_resource *self, struct i_container_s *cnt, struct i_object_s *obj);

/* expmodule_cntform.c */
int v_expmodule_cntform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

/* expmodule_objform.c */
int v_expmodule_objform (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);
int v_expmodule_objform_hist (i_resource *self, struct i_entity_s *ent, i_form_reqdata *reqdata);

